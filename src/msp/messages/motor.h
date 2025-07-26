#ifndef MOTOR_H
#define MOTOR_H

#include "base.h"
#include <cstdint>

namespace MSP {

namespace Messages {

class MOTOR_REQUEST : public MSPBaseV2 {
  public:
    MOTOR_REQUEST() : MSPBaseV2(0x68) {}

#pragma pack(push, 1)
    struct Data {
    } data;
#pragma pack(pop)

    const char *payload() const override {
        return reinterpret_cast<const char *>(&data);
    };

    size_t payloadSize() const override { return sizeof(Data); }
};

class MOTOR_RESPONSE : public MSPBaseV2 {
  public:
    MOTOR_RESPONSE() : MSPBaseV2(0x68) {}

#pragma pack(push, 1)
    struct Data {
        uint16_t motorOutputs[8];
    } data;
#pragma pack(pop)

    const char *payload() const override {
        return reinterpret_cast<const char *>(&data);
    };

    size_t payloadSize() const override { return sizeof(Data); }
};

} // namespace Messages
} // namespace MSP

#endif // MOTOR_H