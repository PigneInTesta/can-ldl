#include "can_config.hpp"

/**
 * Load the .csv file containing all the CAN messages with their respective
 * frequency.
 * 
 * The input file is expected to have the following structure:
 * | can_id | size | bit_start | frequency |
 * 
 */
bool CAN_LoadConfig(const std::string& filename, std::vector<FrequencyGroup>& groups) {
    FILE* file = fopen(filename.c_str(), "r");
    if (!file) {
        printf("[ERROR] Can't open config file: %s\n", filename.c_str());
        return false;
    }

    char line[64];
    unsigned int line_number = 0;

    // Skip header row
    fgets(line, sizeof(line), file);
    line_number++;

    while(fgets(line, sizeof(line), file)) {
        line_number++;

        uint16_t can_id;
        uint8_t size, start, freq;

        int ret = sscanf(line, "0x%hx,%hhu,%hhu,%hhu", &can_id, &size, &start, &freq);

        if (ret != 4) {
            // Not an error - could be an empty line
            continue;
        }

        if (size == 0 || size > 64) {
            printf("[WARN] Line %d: invalid size=%d, skipping\n", line_number, size);
            continue;
        } else if (start + size > 64) {
            printf("[WARN] Line %d, start=%d + size=%d exceeds 64 bits, skipping\n", line_number, start, size);
            continue;
        } else if (freq == 0){
            printf("[WARN] Line %d, frequency=0, skipping\n", line_number);
            continue;
        }

        // printf("[DEBUG] Line %d: 0x%hX,%hhu,%hhu,%hhu\n", line_number, can_id, size, start, freq);

        // Checks if there is an existing group with same frequency as the current signal
        auto it = std::find_if(groups.begin(), groups.end(),
            [freq](const FrequencyGroup& g) {
                return g.frequency == freq;
            }
        );

        if (it == groups.end()) {
            // Create a new group
            FrequencyGroup group;
            group.frequency = freq;
            group.signals.emplace_back(can_id, size, start, freq);
            groups.push_back(group);
        } else {
            // Append the current signal to an existing one
            it->signals.emplace_back(can_id, size, start, freq);
        }
    }

    fclose(file);

    // printf("[INFO] Loaded %zu frequency groups:\n", groups.size());
    // for (const auto& group : groups) {
    //     printf("[INFO]  %d Hz -> %zu signals\n", group.frequency, group.signals.size());
    // }

    return !groups.empty();
}

/**
 * This function is supposed to be called after the CAN_LoadConfig in order to
 * give to the UDP sender the max packet size that is expected to send.
 */
uint16_t CAN_CalcMaxPacketSize(const std::vector<FrequencyGroup>& groups) {
    uint16_t max_size = 0;

    for (const auto& group: groups) {
        // header: 1 (frequency) + 4 (timestamp)
        uint16_t packet_size = 5;

        for (const auto& signal : group.signals) {
            // equivalent to ceil(size / 8)
            packet_size += (signal.getSize() + 7) / 8; 
        }

        if (packet_size > max_size) {
            max_size = packet_size;
        }
    }

    printf("[INFO] Max packet size: %hu bytes\n", max_size);
    return max_size;
}