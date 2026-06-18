#include "can_config.hpp"
#include "can_receiver.hpp"
#include <cstdio>
#include <vector>
#include <thread>
#include <atomic>
#include <csignal>

static std::atomic<bool> stop(false);

void signalHandler(int) {
    stop.store(true);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <config_file> <udp_port>\n", argv[0]);
        return 1;
    }

    const char* config_file = argv[1];

    unsigned long raw_port = std::stoul(argv[2]);
    if (raw_port > 65535) {
        printf("[ERROR] Invalid UDP port: %lu\n", raw_port);
        return 1;
    }
    uint16_t udp_port = static_cast<uint16_t>(raw_port);

    std::signal(SIGINT,  signalHandler);
    std::signal(SIGTERM, signalHandler);

    std::vector<FrequencyGroup> groups;
    if (!CAN_LoadConfig(config_file, groups)) {
        printf("[ERROR] Failed to load config\n");
        return 1;
    }

    std::thread receiver(CAN_ReceiverThread, std::ref(groups), udp_port, std::ref(stop));

    receiver.join();

    printf("[INFO] Shutdown complete\n");
    return 0;
}