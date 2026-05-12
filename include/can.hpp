#ifndef CAN_HPP
#define CAN_HPP

#include <cstdint>

class CanSignal {
public:
    CanSignal(uint16_t id, uint8_t size, uint8_t start, uint8_t freq) 
        : m_can_id(id)
        , m_size(size)
        , m_bit_start(start)
        , m_frequency(freq)
        , m_payload(0)
    {}

    uint16_t getId()        const { return m_can_id; }
    uint8_t  getStart()     const { return m_bit_start; }
    uint8_t  getSize()      const { return m_size; }
    uint8_t  getFrequency() const { return m_frequency; }

    uint64_t getPayload()   const { return m_payload; }

    void setPayload(uint64_t payload) {
        m_payload = payload;
    }

private:
    const uint16_t m_can_id;
    const uint8_t m_size;
    const uint8_t m_bit_start;
    const uint8_t m_frequency;

    uint64_t m_payload;
};

#endif // CAN_HPP