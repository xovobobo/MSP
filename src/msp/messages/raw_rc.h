#ifndef RAW_RC_H
#define RAW_RC_H

#include "base.h"

#ifndef MSP_RC_CHANNEL_COUNT
#define MSP_RC_CHANNEL_COUNT 16
#endif

namespace MSP {

namespace Messages {

class SET_RAW_RC_Request : public MSPBaseV2 {
  public:
    SET_RAW_RC_Request() : MSPBaseV2(0xC8) {}

#pragma pack(push, 1)
    struct Data {
        uint16_t channels[MSP_RC_CHANNEL_COUNT] = {1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000};
    } data;
#pragma pack(pop)

    const char *payload() const override {
        return reinterpret_cast<const char *>(&data);
    };

    size_t payloadSize() const override { return sizeof(Data); }
};

class SET_RAW_RC_Response : public MSPBaseV2 {
  public:
    SET_RAW_RC_Response() : MSPBaseV2(0xC8) {}

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

#endif // RAW_RC_H