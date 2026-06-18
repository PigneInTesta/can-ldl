#include "can_receiver.hpp"
#include <cstdio>
#include <cstring>
#include <unordered_map>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

static void decodePacket(const uint8_t* data, uint16_t length, FrequencyGroup& group) {
    // Skip header: 1 (frequency) + 4 (timestamp)
    uint32_t timestamp = (data[1] << 24) | (data[2] << 16) | (data[3] << 8) | data[4];
    uint16_t idx = 5;

    printf("[RX] %d Hz | ts=%u ms | %hu bytes\n",
           group.getFrequency(), timestamp, length);

    for (auto& signal : group.m_signals) {
        uint8_t byte_count = (signal.getSize() + 7) / 8;

        if (idx + byte_count > length) {
            printf("[WARN] Packet too short for signal 0x%03X\n", signal.getId());
            break;
        }

        uint64_t value = 0;
        for (int i = byte_count - 1; i >= 0; i--) {
            value |= ((uint64_t)data[idx++]) << (i * 8);
        }

        signal.setPayload(value);

        const char* unit = signal.getUnit();

        if (unit[0] != '\0') {
            printf("  %-30s = %10.2f %s\n", signal.getName(), signal.getPhysicalValue(), unit);
        } else {
            printf("  %-30s = %10lu\n", signal.getName(), (unsigned long)value);
        }
    }
}

static std::unordered_map<uint8_t, FrequencyGroup*>
buildFrequencyMap(std::vector<FrequencyGroup>& groups) {
    std::unordered_map<uint8_t, FrequencyGroup*> map;

    for (auto& group : groups) {
        map[group.getFrequency()] = &group;
    }

    printf("[INFO] Frequency map built with %zu groups\n", map.size());
    return map;
}

void CAN_ReceiverThread(std::vector<FrequencyGroup>& groups,
                        uint16_t port,
                        std::atomic<bool>& stop) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        printf("[ERROR] Failed to create UDP socket\n");
        stop.store(true);
        return;
    }

    // Set receive timeout so the loop can check stop flag
    struct timeval timeout = {1, 0};  // 1 second
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    struct sockaddr_in bind_addr;
    memset(&bind_addr, 0, sizeof(bind_addr));
    bind_addr.sin_family      = AF_INET;
    bind_addr.sin_port        = htons(port);
    bind_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr*)&bind_addr, sizeof(bind_addr)) < 0) {
        printf("[ERROR] Failed to bind to port %d\n", port);
        stop.store(true);
        close(sock);
        return;
    }

    printf("[INFO] Receiver listening on port %d\n", port);

    auto freq_map = buildFrequencyMap(groups);

    // TODO: calculate max size for a packet
    uint8_t buffer[512];

    while (!stop.load()) {
        ssize_t nbytes = recvfrom(sock, buffer, sizeof(buffer), 0, nullptr, nullptr);

        if (nbytes < 0) {
            // Timeout — just check stop flag and continue
            continue;
        }

        if (nbytes < 5) {
            printf("[WARN] Packet too short: %zd bytes\n", nbytes);
            continue;
        }

        uint8_t frequency = buffer[0];

        auto it = freq_map.find(frequency);
        if (it == freq_map.end()) {
            printf("[WARN] Unknown frequency: %d Hz\n", frequency);
            continue;
        }

        decodePacket(buffer, (uint16_t)nbytes, *(it->second));
    }

    printf("[INFO] Receiver thread shutting down\n");
    close(sock);
}