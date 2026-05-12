#ifndef CAN_SENDER_HPP
#define CAN_SENDER_HPP

#include <vector>
#include "can_config.hpp"

void CAN_SenderThread(std::vector<FrequencyGroup>& groups);

#endif // CAN_SENDER_HPP