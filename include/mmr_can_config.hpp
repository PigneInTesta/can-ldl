#ifndef MMR_CAN_CONFIG_HPP
#define MMR_CAN_CONFIG_HPP

#include <vector>
#include <string>
#include <cstdio>
#include <chrono>
#include <algorithm>
#include "mmr_can.hpp"

struct FrequencyGroup {
    uint8_t    frequency;
    std::chrono::steady_clock::time_point last_sent;
    std::vector<MmrCanSignal> signals;
};

bool MMR_CAN_LoadConfig(const std::string& filename, std::vector<FrequencyGroup>& signals);

#endif // MMR_CAN_CONFIG_HPP