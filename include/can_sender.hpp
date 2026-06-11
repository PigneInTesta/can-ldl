#ifndef CAN_SENDER_HPP
#define CAN_SENDER_HPP

#include "can_config.hpp"
#include <vector>
#include <atomic>

void CAN_SenderThread(std::vector<FrequencyGroup>& groups,
                      const char* ip,
                      uint16_t port,
                      std::atomic<bool>& stop);

#endif // CAN_SENDER_HPP