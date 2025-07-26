#ifndef CRC8_DVB_S2_H
#define CRC8_DVB_S2_H

#include <cstddef>
#include <cstdint>

uint8_t compute_crc8_dvb_s2(const char* data, size_t start_idx, size_t end_idx);


#endif // CRC8_DVB_S2_H