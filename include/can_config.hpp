#ifndef CAN_CONFIG_HPP
#define CAN_CONFIG_HPP

#include <vector>
#include <string>
#include <cstdio>
#include <chrono>
#include <algorithm>
#include <cstdint>
#include "can.hpp"

class FrequencyGroup {
public:
    FrequencyGroup(uint8_t frequency)
        : m_frequency(frequency)
        , m_packet_size(0)
    {}

    FrequencyGroup(FrequencyGroup&& other) noexcept
        : last_sent(other.last_sent)
        , m_signals(std::move(other.m_signals))
        , m_frequency(other.m_frequency)
        , m_packet_size(other.m_packet_size)
        , m_packet(std::move(other.m_packet))
    {}

    uint8_t getFrequency()      const { return m_frequency; }
    uint16_t getPacketSize()    const { return m_packet_size; }

    // Called once after all signals are loaded
    void init() {
        // header: 1 (frequency) + 4 (timestamp)
        m_packet_size = 5;
        for (const auto& signal : m_signals) {
            m_packet_size += (signal.getSize() + 7) / 8;
        }
        m_packet.resize(m_packet_size);
    }

    // Fills m_packet and returns pointer and size ready for sendto
    const uint8_t* buildPacket(uint32_t timestamp_ms) {
        uint16_t idx = 0;

        // Header
        m_packet[idx++] = m_frequency;
        m_packet[idx++] = (timestamp_ms >> 24) & 0xFF;
        m_packet[idx++] = (timestamp_ms >> 16) & 0xFF;
        m_packet[idx++] = (timestamp_ms >> 8) & 0xFF;
        m_packet[idx++] = (timestamp_ms >> 0) & 0xFF;

        // Payload
        for (const auto& signal : m_signals) {
            uint64_t value = signal.getPayload();
            uint8_t byte_count = (signal.getSize() + 7) / 8;

            for (int i = byte_count - 1; i >= 0; i--) {
                m_packet[idx++] = (value >> (i * 8)) & 0xFF;
            }
        }

        return m_packet.data();
    }

    std::chrono::steady_clock::time_point last_sent;
    std::vector<CanSignal> m_signals;

private:
    const uint8_t m_frequency;
    uint16_t m_packet_size;
    std::vector<uint8_t> m_packet;
};

bool CAN_LoadConfig(const std::string& filename, std::vector<FrequencyGroup>& signals);

#endif // CAN_CONFIG_HPP