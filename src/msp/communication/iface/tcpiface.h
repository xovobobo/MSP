#ifndef TCPIFACE_H
#define TCPIFACE_H

#include "absinterface.h"
#include <cstdint>

using namespace boost::asio;
class TcpIface : public AbsIface {
  public:
    explicit TcpIface();
    ~TcpIface() override;

    size_t read(char *buffer, size_t max_length) override;
    size_t available() override;

    void setAddress(std::string ip, uint16_t port);
    void stop() override;
    bool connect() override;

    bool isConnected() const override;

  private:
    ip::tcp::socket socket;
    std::string msp_ip = "127.0.0.1";
    uint16_t msp_port = 9001;

  protected:
    size_t write(char *data, size_t length) override;
};

#endif // TCPIFACE_H