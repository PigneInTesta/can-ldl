#ifndef CAN_READER_HPP
#define CAN_READER_HPP

#include "can_config.hpp"
#include <vector>
#include <atomic>

void CAN_ReaderThread(std::vector<FrequencyGroup>& groups,
                      const char* can_interface,
                      std::atomic<bool>& stop);

#endif // CAN_READER_HPP