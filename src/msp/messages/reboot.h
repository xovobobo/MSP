#ifndef REBOOT_H
#define REBOOT_H

#include "base.h"

namespace MSP {

namespace Messages {

class REBOOT_REQUEST : public MSPBaseV2 {
  public:
    REBOOT_REQUEST() : MSPBaseV2(0x44) {}

#pragma pack(push, 1)
    struct Data {
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