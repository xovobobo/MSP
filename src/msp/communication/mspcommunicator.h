#ifndef MSPCOMMUNICATOR_H
#define MSPCOMMUNICATOR_H

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <variant>
#include <typeindex>
#include <unordered_map>

#include "iface/absinterface.h"
#include "msp/messages/base.h"


class MSPCommunicator {
  public:
    MSPCommunicator();
    ~MSPCommunicator();

    bool start();
    bool stop();

    using MessageCallback =
        std::function<void(const std::shared_ptr<MSP::Messages::MSPBaseV2> &)>;

    template <typename MSPMessage_t>
    void registerCallbackV2(MessageCallback callback) {
        static_assert(
            std::is_base_of<MSP::Messages::MSPBaseV2, MSPMessage_t>::value,
            "MSPMessage_t must derive from MSPBaseV2");
        callbackMapV2[std::make_shared<MSPMessage_t>()] = std::move(callback);
    }

    void setIface(AbsIface *new_iface);
    void pushMessage(const std::shared_ptr<MSP::Messages::MSPBaseV2> &message);

  private:
    enum ReadState {
        UNKNOWN,
        WAIT_FIRST_BYTE,
        WAIT_VERSION,
        WAIT_DIRECTION,
        WAIT_DIRECTION_V2,
        WAIT_FLAG_V2,
        WAIT_SIZE_V1,
        WAIT_COMMAND_V2,
        WAIT_COMMAND_V1,
        WAIT_SIZE_V2,
        WAIT_PAYLOAD,
        WAIT_PAYLOAD_V2,
        WAIT_CRC,
        WAIT_CRC_V2
    } readState;
    
    std::unordered_map<std::shared_ptr<MSP::Messages::MSPBaseV2>, MessageCallback> callbackMapV2;

    char readedData[2048];
    uint16_t readedIdx;

    void readLoop();
    void writeLoop();
    void processMspData(char data);

    std::thread writeThread;
    std::thread readThread;

    std::atomic<bool> running;
    std::mutex ifaceMutex;
    std::condition_variable reconnect_cv; //for unblock and exit


    std::mutex writeMutex;
    std::mutex callbackMutex;
    std::condition_variable writeCV;

    std::queue<std::shared_ptr<MSP::Messages::MSPBaseV2>> writeQueue;
    AbsIface *iface;
};

#endif // MSPCOMMUNICATOR_H