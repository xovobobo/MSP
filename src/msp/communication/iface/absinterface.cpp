#include "absinterface.h"
#include "msp/messages/base.h"

AbsIface::AbsIface() = default;
AbsIface::~AbsIface() = default;

bool AbsIface::writeMSP(const MSP::Messages::MSPBaseV2 &data) {
    if (!isConnected()) {
        std::cerr << "not connected" << std::endl;
        return -1;
    }

    // $ + X + < + 0 + ID + payload_size + payload + crc

    char msp_frame[8 + data.payloadSize() + 1];
    msp_frame[0] = '$';
    msp_frame[1] = 'X';
    msp_frame[2] = '<';
    msp_frame[3] = 0;

    msp_frame[4] = data.id & 0xFF;
    msp_frame[5] = (data.id >> 8) & 0xFF;

    msp_frame[6] = data.payloadSize() & 0xFF;
    msp_frame[7] = (data.payloadSize() >> 8) & 0xFF;

    memcpy(msp_frame + 8, data.payload(), data.payloadSize());
    // memcpy(msp_frame + 8, &data, data.payloadSize());

    msp_frame[8 + data.payloadSize()] =
        compute_crc8_dvb_s2(msp_frame, 3, 8 + data.payloadSize());

    // std::cout << "size to send" << sizeof(msp_frame) << std::endl;
    // for (size_t i = 0; i < sizeof(msp_frame); i++) {
    //     printf("%02X ", static_cast<unsigned char>(msp_frame[i]));
    // }
    // std::cout << std::endl;

    return sizeof(msp_frame) == write(msp_frame, sizeof(msp_frame));
};
