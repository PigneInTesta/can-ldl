#ifndef MMR_CAN_CONFIG_HPP
#define MMR_CAN_CONFIG_HPP

#include <vector>
#include <string>
#include <cstdio>
#include "mmr_can.hpp"

bool MMR_CAN_LoadConfig(const std::string& filename, std::vector<MmrCanSignal>& signals);

#endif // MMR_CAN_CONFIG_HPP