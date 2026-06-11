#ifndef CAN_HPP
#define CAN_HPP

#include <cstdint>
#include <atomic>

class CanSignal {
public:
    CanSignal(uint16_t id, uint8_t size, uint8_t start, uint8_t freq)
        : m_can_id(id)
        , m_size(size)
        , m_bit_start(start)
        , m_frequency(freq)
        , m_payload(0)
    {}

    CanSignal(CanSignal&& other) noexcept
        : m_can_id(other.m_can_id)
        , m_size(other.m_size)
        , m_bit_start(other.m_bit_start)
        , m_frequency(other.m_frequency)
        , m_payload(other.m_payload.load())
    {}

    uint16_t getId()        const { return m_can_id; }
    uint8_t  getStart()     const { return m_bit_start; }
    uint8_t  getSize()      const { return m_size; }
    uint8_t  getFrequency() const { return m_frequency; }
    uint64_t getPayload()   const { return m_payload.load(); }

    void setPayload(uint64_t payload) { m_payload.store(payload); }

private:
    const uint16_t m_can_id;
    const uint8_t m_size;
    const uint8_t m_bit_start;
    const uint8_t m_frequency;

    std::atomic<uint64_t> m_payload;
};

#endif // CAN_HPP