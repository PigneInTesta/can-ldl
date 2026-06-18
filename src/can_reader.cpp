#include "can_reader.hpp"
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <unordered_map>
#include <vector>

static uint64_t extractBits(const uint8_t* data, uint8_t start, uint8_t size) {
    uint64_t value = 0;
    for (uint8_t i = 0; i < size; ++i) {
        uint8_t bit_pos = start + i;
        uint8_t byte_idx = bit_pos / 8;
        uint8_t bit_idx = 7 - (bit_pos % 8);
        
        value |= ((data[byte_idx] >> bit_idx) & 0x01ULL) << (size - 1 - i);
    }

    return value;
}

static std::unordered_map<uint16_t, std::vector<CanSignal*>> buildLookupMap(std::vector<FrequencyGroup>& groups) {
    std::unordered_map<uint16_t, std::vector<CanSignal*>> map;
    for (auto& group : groups) {
        for (auto& signal : group.m_signals) {
            map[signal.getId()].push_back(&signal);
        }
    }

    printf("[INFO] Built lookup map with %zu unique CAN IDs\n", map.size());
    return map;
}

void CAN_ReaderThread(std::vector<FrequencyGroup>& groups, const char* can_interface, std::atomic<bool>& stop) {
    int sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (sock < 0) {
        printf("[ERROR] Failed to create CAN socket\n");
        stop.store(true);
        return;
    }

    struct ifreq ifr;
    strncpy(ifr.ifr_name, can_interface, IFNAMSIZ - 1);
    if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0) {
        printf("[ERROR] Failed to get CAN interface index\n");
        stop.store(true);
        close(sock);
        return;
    }

    // Set receive timeout so the loop can check stop flag
    struct timeval timeout = {1, 0};  // 1 second
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("[ERROR] Failed to bind CAN socket\n");
        stop.store(true);
        close(sock);
        return;
    }

    printf("[INFO] CAN reader thread started on interface %s\n", can_interface);

    auto lookup_map = buildLookupMap(groups);

    struct can_frame frame;
    while (!stop.load()) {
        ssize_t nbytes = read(sock, &frame, sizeof(frame));

        if (nbytes < 0) {

            printf("[ERROR] Failed to read from CAN socket\n");
            stop.store(true);
            break;
        }

        if (nbytes < (ssize_t)sizeof(struct can_frame)) {
            printf("[WARN] Incomplete CAN frame\n");
            continue;
        }

        uint16_t can_id = frame.can_id & CAN_EFF_MASK;

        auto it = lookup_map.find(can_id);
        if (it == lookup_map.end()) continue;

        for (CanSignal* signal : it->second) {
            uint64_t value = extractBits(frame.data, signal->getStart(), signal->getSize());
            signal->setPayload(value);
        }
    }

    printf("[INFO] Reader thread shutting down\n");
    close(sock);
} 