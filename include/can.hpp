#ifndef CAN_HPP
#define CAN_HPP

#include <cstdint>
#include <cstring>
#include <atomic>

#define CAN_SIGNAL_NAME_MAX 32
#define CAN_SIGNAL_UNIT_MAX 8
#define CAN_SIGNAL_TYPE_MAX 8

enum class SignalType : uint8_t {
    UINT8,
    INT8,
    UINT16,
    INT16,
    UINT32,
    INT32
};

class CanSignal {
public:
    CanSignal(uint16_t id, uint8_t size, uint8_t start, uint8_t freq, 
        const char* name, double offset, double scaler, const char* unit, SignalType type)
        : m_can_id(id)
        , m_size(size)
        , m_bit_start(start)
        , m_frequency(freq)
        , m_offset(offset)
        , m_scaler(scaler)
        , m_type(type)
        , m_payload(0)
    {
        #ifdef DEBUG
        printf("[CTOR] %s scaler=%f\n", name, scaler);
        #endif
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
        , m_type(other.m_type)
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
    SignalType getType()    const { return m_type; }
    uint64_t getPayload()   const { return m_payload.load(); }

    void setPayload(uint64_t payload) { m_payload.store(payload); }

    double getPhysicalValue() const {
        uint64_t raw = m_payload.load();
        double value;

        switch (m_type){
            case SignalType::INT8:  value = static_cast<double>(static_cast<int8_t>(raw)); break;
            case SignalType::INT16: value = static_cast<double>(static_cast<int16_t>(raw)); break;
            case SignalType::INT32: value = static_cast<double>(static_cast<int32_t>(raw)); break;
            default:                value = static_cast<double>(raw);
        }

        return (value * m_scaler) + m_offset;
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
    const SignalType m_type;

    std::atomic<uint64_t> m_payload;
};

#endif // CAN_HPP