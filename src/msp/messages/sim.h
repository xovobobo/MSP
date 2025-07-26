#ifndef SIM_H
#define SIM_H

#include "base.h"
#include <cstdint>

namespace MSP {

namespace Messages {

typedef enum {
    HITL_RESET_FLAGS = (0 << 0),
    HITL_ENABLE = (1 << 0),
    HITL_SIMULATE_BATTERY = (1 << 1),
    HITL_MUTE_BEEPER = (1 << 2),
    HITL_USE_IMU = (1 << 3),
    HITL_HAS_NEW_GPS_DATA = (1 << 4),
    HITL_EXT_BATTERY_VOLTAGE = (1 << 5),
    HITL_AIRSPEED = (1 << 6),
    HITL_EXTENDED_FLAGS = (1 << 7),
    HITL_GPS_TIMEOUT = (1 << 8),
    HITL_PITOT_FAILURE = (1 << 9),
} simulatorFlags_t;

class SIMULATOR_REQUEST : public MSPBaseV2 {
  public:
    SIMULATOR_REQUEST() : MSPBaseV2(0x201f) {}

#pragma pack(push, 1)
    struct Data {
        uint8_t version = 2;
        uint8_t flags = 0;
        uint8_t fix = 0;
        uint8_t numSat = 0;
        int lat = 0;
        int lon = 0;
        int alt = 0;
        int16_t speed = 0;
        int16_t course = 0;
        int16_t velNED_N = 0;
        int16_t velNED_E = 0;
        int16_t velNED_D = 0;
        int16_t roll = 0;
        int16_t pitch = 0;
        int16_t yaw = 0;
        int16_t accel_x = 0;
        int16_t accel_y = 0;
        int16_t accel_z = 0;
        int16_t gyro_x = 0;
        int16_t gyro_y = 0;
        int16_t gyro_z = 0;
        int baro = 0;
        int16_t mag_x = 0;
        int16_t mag_y = 0;
        int16_t mag_z = 0;
        uint8_t vbat = 0;
        uint16_t airspeed = 0;
        uint8_t flags2 = 0;
    } data;
#pragma pack(pop)

    const char *payload() const override {
        return reinterpret_cast<const char *>(&data);
    };

    size_t payloadSize() const override { return sizeof(Data); };
};

class SIMULATOR_RESPONSE : public MSPBaseV2 {
  public:
    SIMULATOR_RESPONSE() : MSPBaseV2(0x201f) {}

#pragma pack(push, 1)
    struct Data {
        uint16_t stabilizedRoll;
        uint16_t stabilizedPitch;
        uint16_t stabilizedYaw;
        uint16_t stabilizedThrottle;
        uint8_t debugFlags;
        uint32_t debugValue;
        int16_t attitudeRoll;
        int16_t attitudePitch;
        int16_t attitudeYaw;
        uint8_t osdHeader;
        uint8_t osdRows;
        uint8_t osdCols;
        uint8_t osdStartY;
        uint8_t osdStartX;
        uint8_t osdRleData[];

    } data;
#pragma pack(pop)

    const char *payload() const override {
        return reinterpret_cast<const char *>(&data);
    };

    size_t payloadSize() const override { return sizeof(Data); };
};

} // namespace Messages
} // namespace MSP
#endif // SIM_H