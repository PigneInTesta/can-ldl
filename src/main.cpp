#include "can_config.hpp"
#include "can_sender.hpp"
#include "can_reader.hpp"
#include <thread>
#include <csignal>
#include <atomic>

static std::atomic<bool> stop(false);

void signalHandler(int) {
    stop.store(true);
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        printf("Usage: %s <config_file> <can_interface> <udp_ip> <udp_port>\n", argv[0]);
        return 1;
    }

    const char* config_file = argv[1];
    const char* can_interface = argv[2];
    const char* udp_ip = argv[3];

    unsigned long raw_port = std::stoul(argv[4]);
    if (raw_port > 65535) {
        printf("[ERROR] Invalid UDP port: %lu\n", raw_port);
        return 1;
    }
    uint16_t udp_port = static_cast<uint16_t>(raw_port);
    
    
    std::vector<FrequencyGroup> groups;
    if (!CAN_LoadConfig(config_file, groups)) {
        printf("[ERROR] Failed to load config\n");
        return 1;
    }

    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    // WIP, reader thread not closing properly 
    std::thread reader(CAN_ReaderThread, std::ref(groups), can_interface, std::ref(stop));
    std::thread sender(CAN_SenderThread, std::ref(groups), udp_ip, udp_port, std::ref(stop));

    reader.join();
    sender.join();

    printf("[INFO] Shutdown complete\n");
    return 0;
}