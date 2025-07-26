#ifndef TCPIFACE_H
#define TCPIFACE_H

#include "absinterface.h"

using namespace boost::asio;
class TcpIface : public AbsIface {
  public:
    explicit TcpIface();
    ~TcpIface() override;

    size_t read(char *buffer, size_t max_length) override;
    size_t available() override;

    void stop() override;
    bool connect() override;

    bool isConnected() const override;

  private:
    ip::tcp::socket socket;

  protected:
    size_t write(char *data, size_t length) override;
};

#endif // TCPIFACE_H