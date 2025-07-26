#ifndef BASE_H
#define BASE_H

#include <cstddef>
#include <cstdint>

namespace MSP {

namespace Messages {

class MSPBaseV2 {
  public:
    MSPBaseV2(uint16_t msg_id) { id = msg_id; }

    uint16_t id;

    virtual const char *payload() const = 0;

    virtual size_t payloadSize() const = 0;
};

} // namespace Messages
} // namespace MSP

#endif // BASE_H