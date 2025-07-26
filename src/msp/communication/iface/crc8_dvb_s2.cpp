#include "crc8_dvb_s2.h"

uint8_t compute_crc8_dvb_s2(const char* data, size_t start_idx, size_t end_idx) {
    uint8_t crc = 0;
    const uint8_t poly = 0xD5;

    for (size_t i = start_idx; i < end_idx; i++) {
        crc ^= static_cast<uint8_t>(data[i]);
        for (int j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ poly;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}