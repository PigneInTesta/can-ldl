#include "mmr_can_config.hpp"

int main(void) {

    std::vector<FrequencyGroup> groups;

    if (!MMR_CAN_LoadConfig("config/can_table.csv", groups)) {
        printf("[ERROR] Failed to load config\n");
        return 1;
    }

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