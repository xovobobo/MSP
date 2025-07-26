#ifndef ABSINTERFACE_H
#define ABSINTERFACE_H

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <cstddef>
#include <cstdint>
#include <mutex>

#include <iostream>

#include "msp/messages/base.h"
#include "crc8_dvb_s2.h"

using namespace boost::asio;
class AbsIface {
  public:
    explicit AbsIface();
    virtual ~AbsIface() = 0;

    bool writeMSP(const MSP::Messages::MSPBaseV2& data);

    virtual size_t read(char *buffer, size_t max_length) = 0;
    virtual size_t available() = 0;

    virtual bool isConnected() const = 0;
    virtual void stop() = 0;
    virtual bool connect() = 0;

  protected:
    bool connectedState;

    virtual size_t write(char *data, size_t length) = 0;
    boost::asio::io_service io_context;
    std::mutex readWriteMutex;
};

#endif // ABSINTERFACE_H