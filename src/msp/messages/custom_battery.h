#ifndef REBOOT_H
#define REBOOT_H

#include "base.h"
#include <cstdint>

namespace MSP {

namespace Messages {

class BATTERY_REQUEST : public MSPBaseV2 {
  public:
    BATTERY_REQUEST() : MSPBaseV2(0x4000) {}

#pragma pack(push, 1)
    struct Data {
        uint16_t voltage;
        uint16_t current;
    } data;
#pragma pack(pop)

    const char *payload() const override {
        return reinterpret_cast<const char *>(&data);
    };

    size_t payloadSize() const override { return sizeof(Data); }
};
} // namespace Messages
} // namespace MSP

#endif // REBOOT_H