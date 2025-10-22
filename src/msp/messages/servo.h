#ifndef SERVO_H
#define SERVO_H

#include "base.h"
#include <cstdint>

#ifndef MSP_MAX_SUPPORTED_SERVOS
#define MSP_MAX_SUPPORTED_SERVOS 18
#endif

namespace MSP {

namespace Messages {

class SERVO_REQUEST : public MSPBaseV2 {
  public:
    SERVO_REQUEST() : MSPBaseV2(0x67) {}

#pragma pack(push, 1)
    struct Data {
    } data;
#pragma pack(pop)

    const char *payload() const override {
        return reinterpret_cast<const char *>(&data);
    };

    size_t payloadSize() const override { return sizeof(Data); }
};

class SERVO_RESPONSE : public MSPBaseV2 {
  public:
    SERVO_RESPONSE() : MSPBaseV2(0x67) {}

#pragma pack(push, 1)
    struct Data {
        uint16_t servoOutputs[MSP_MAX_SUPPORTED_SERVOS];
    } data;
#pragma pack(pop)

    const char *payload() const override {
        return reinterpret_cast<const char *>(&data);
    };

    size_t payloadSize() const override { return sizeof(Data); }
};

} // namespace Messages
} // namespace MSP

#endif // SERVO_H