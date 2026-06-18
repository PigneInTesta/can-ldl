#ifndef CAN_HPP
#define CAN_HPP

#include <cstdint>
#include <cstring>
#include <atomic>

#define CAN_SIGNAL_NAME_MAX 32
#define CAN_SIGNAL_UNIT_MAX 8

class CanSignal {
public:
    CanSignal(uint16_t id, uint8_t size, uint8_t start, uint8_t freq, 
        const char* name, double offset, double scaler, const char* unit)
        : m_can_id(id)
        , m_size(size)
        , m_bit_start(start)
        , m_frequency(freq)
        , m_offset(offset)
        , m_scaler(scaler)
        , m_payload(0)
    {
        printf("[CTOR] %s scaler=%f\n", name, scaler);
        strncpy(m_name, name, CAN_SIGNAL_NAME_MAX-1);
        m_name[CAN_SIGNAL_NAME_MAX-1] = '\0';
        strncpy(m_unit, unit, CAN_SIGNAL_UNIT_MAX-1);
        m_unit[CAN_SIGNAL_UNIT_MAX-1] = '\0';
    }

    CanSignal(CanSignal&& other) noexcept
        : m_can_id(other.m_can_id)
        , m_size(other.m_size)
        , m_bit_start(other.m_bit_start)
        , m_frequency(other.m_frequency)
        , m_offset(other.m_offset)
        , m_scaler(other.m_scaler)
        , m_payload(other.m_payload.load())
    {
        strncpy(m_name, other.m_name, CAN_SIGNAL_NAME_MAX);
        strncpy(m_unit, other.m_unit, CAN_SIGNAL_UNIT_MAX);
    }

    uint16_t getId()        const { return m_can_id; }
    uint8_t  getStart()     const { return m_bit_start; }
    uint8_t  getSize()      const { return m_size; }
    uint8_t  getFrequency() const { return m_frequency; }
    const char* getName()   const { return m_name; }
    double getOffset()      const { return m_offset; }
    double getScaler()      const { return m_scaler; }
    const char* getUnit()   const { return m_unit; }
    uint64_t getPayload()   const { return m_payload.load(); }

    void setPayload(uint64_t payload) { m_payload.store(payload); }

    double getPhysicalValue() const {
        return (static_cast<double>(m_payload.load()) * m_scaler) + m_offset;
    }

private:
    const uint16_t m_can_id;
    const uint8_t m_size;
    const uint8_t m_bit_start;
    const uint8_t m_frequency;
    char m_name[CAN_SIGNAL_NAME_MAX];
    const double m_offset;
    const double m_scaler;
    char m_unit[CAN_SIGNAL_UNIT_MAX];

    std::atomic<uint64_t> m_payload;
};

#endif // CAN_HPP