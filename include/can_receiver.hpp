#ifndef CAN_RECEIVER_HPP
#define CAN_RECEIVER_HPP

#include <vector>
#include <atomic>
#include <cstdint>
#include "can_config.hpp"

void CAN_ReceiverThread(std::vector<FrequencyGroup>& groups,
                        uint16_t port,
                        std::atomic<bool>& stop);

#endif // CAN_RECEIVER_HPP