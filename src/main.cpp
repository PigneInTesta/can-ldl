#include "can_config.hpp"

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

    uint16_t max_packet_size = CAN_CalcMaxPacketSize(groups);

    // unsigned int count = 0;
    // for (const auto& group: groups) {
    //     for (const auto& signal : group.signals) {
    //         printf("[DEBUG] Signal %u: 0x%hX,%hhu,%hhu,%hhu\n", 
    //             count, 
    //             signal.getId(), 
    //             signal.getSize(),
    //             signal.getStart(),
    //             signal.getFrequency());
    //         count++;
    //     }
    // }

    return 0;
}