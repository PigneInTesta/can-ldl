#ifndef CAN_CONFIG_HPP
#define CAN_CONFIG_HPP

#include <vector>
#include <string>
#include <cstdio>
#include <chrono>
#include <algorithm>
#include "can.hpp"

struct FrequencyGroup {
    uint8_t    frequency;
    std::chrono::steady_clock::time_point last_sent;
    std::vector<CanSignal> signals;
};

bool CAN_LoadConfig(const std::string& filename, std::vector<FrequencyGroup>& signals);
uint16_t CAN_CalcMaxPacketSize(const std::vector<FrequencyGroup>& groups);

#endif // CAN_CONFIG_HPP