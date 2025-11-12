#include "tcpiface.h"

#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <cstddef>
#include <iostream>
#include <mutex>

TcpIface::TcpIface() : AbsIface(), socket(io_context), connectedState(false) {}

TcpIface::~TcpIface() { stop(); }

bool TcpIface::connect() {
    std::lock_guard<std::mutex> lock(readWriteMutex);

    if (isConnected()) {
        return true;
    }

    try {
        socket.connect(ip::tcp::endpoint(boost::asio::ip::address::from_string(msp_ip),msp_port));
        connectedState = true;
    } catch (boost::system::system_error &e) {
        // printf("Connection failed (Boost error) : %s\n", e.what());
        return false;
    }
    return true;
}

size_t TcpIface::write(char *data, size_t length) {
    std::lock_guard<std::mutex> lock(readWriteMutex);

    if (!isConnected())
        return -1;

    try {
        return boost::asio::write(socket, boost::asio::buffer(data, length));
    } catch (boost::system::system_error &e) {
        if (e.code() == boost::asio::error::broken_pipe) {
            socket.close();
            connectedState = false;
        }
        std::cout << "error happened while sending: " << e.what() << std::endl;
        return -1;
    }
}

size_t TcpIface::available() {
    std::lock_guard<std::mutex> lock(readWriteMutex);
    try {
        return socket.available();
    } catch (...) {

        return 0;
    }
}

size_t TcpIface::read(char *buffer, size_t max_length) {
    std::lock_guard<std::mutex> lock(readWriteMutex);
    if (!isConnected())
        return 0;

    try {
        size_t bytes_read = socket.read_some(boost::asio::buffer(buffer, max_length));
        return bytes_read;
    } catch (...) {

        return 0;
    }
    return 0;
}

void TcpIface::stop() {
    std::lock_guard<std::mutex> lock(readWriteMutex);

    if (socket.is_open()) {
        try {
            socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        } catch (boost::system::system_error &e) {
            if (e.code() != boost::asio::error::not_connected) // ignore error if socket was not connected
            {
                std::cerr << "Failed to shutdown socket:: " << e.what() << std::endl;
            }
        }

        // try
        // {
        socket.close();
        // }
        // catch(boost::system::system_error &e)
        // {
        // std::cerr << "Failed to close socket:: " << e.what() << std::endl;

        // }
    }

    connectedState = false;
}

bool TcpIface::isConnected() const { return connectedState; }

void TcpIface::setAddress(std::string ip, uint16_t port)
{
    msp_ip = ip;
    msp_port = port;
}