#include "mspcommunicator.h"
#include <msp/messages/base.h>

MSPCommunicator::MSPCommunicator() : readState(ReadState::WAIT_FIRST_BYTE), readedIdx(0), running(false), iface(nullptr) {}

MSPCommunicator::~MSPCommunicator() { stop(); }

void MSPCommunicator::setIface(AbsIface *new_iface) {
    if (running) {
        std::cerr << "MSPCommunicator is running, stop it first" << std::endl;
        return;
    }

    if (iface)
        delete iface;
    iface = new_iface;
}

bool MSPCommunicator::start() {
    if (running)
        return true;

    running = true;

    readThread = std::thread(&MSPCommunicator::readLoop, this);
    writeThread = std::thread(&MSPCommunicator::writeLoop, this);

    return true;
}

bool MSPCommunicator::stop() {
    if (!running)
        return true;

    running = false;
    writeCV.notify_all();

    printf("[MSP] joining readThread..\n");
    reconnect_cv.notify_all();
    if (readThread.joinable())
        readThread.join();
    printf("[MSP] readThread joined\n");

    printf("[MSP] joining writeThread..\n");
    if (writeThread.joinable())
        writeThread.join();

    iface->stop();
    printf("[MSP] writThread joined\n");

    // clear write queue
    std::queue<std::shared_ptr<MSP::Messages::MSPBaseV2>> empty;
    std::swap(writeQueue, empty);

    return true;
}

void MSPCommunicator::readLoop() {
    char tcp_buffer[65535] = {0};
    bool connection_lost = false;
    while (running) {

        {
            std::unique_lock<std::mutex> lock(ifaceMutex);
            if (!iface->isConnected()) {
                if (!iface->connect()) {
                    if (!connection_lost) {
                        connection_lost = true;
                        printf("[MSP] Iface connection lost during communication. Trying to reconnect...\n");
                    }
                    reconnect_cv.wait_for(lock, std::chrono::seconds(1), [this]() {
                        if (!running) {
                            printf("[MSP] readLoop. Shutdown requested, exiting...\n");
                        }
                        return !running;
                    });

                    continue;
                } else {
                    printf("[MSP] Iface connection recovered\n");
                }
            }
        }

        connection_lost = false;

        {
            std::unique_lock<std::mutex> lock(ifaceMutex);
            size_t available = iface->available();
            if (available > 0) {
                if (iface->read(&tcp_buffer[0], available) == available) {
                    for (size_t i = 0; i < available; i++) {
                        processMspData(tcp_buffer[i]);
                    }
                }
            } else {
                lock.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    }
}

void MSPCommunicator::writeLoop() {
    while (running) {
        std::shared_ptr<MSP::Messages::MSPBaseV2> packet;

        {
            std::unique_lock<std::mutex> lock(writeMutex);
            writeCV.wait(lock, [this]() { return !writeQueue.empty() || !running; });

            if (!running)
                break;

            packet = writeQueue.front();
            writeQueue.pop();
        }

        {
            std::lock_guard<std::mutex> ifaceLock(ifaceMutex);

            try {
                if (!iface->isConnected()) {
                    iface->connect();
                }
                iface->writeMSP(*packet);
            } catch (const std::exception &e) {
                std::cerr << "Write failed: " << e.what() << std::endl;
            }
        }
    }
}

void MSPCommunicator::pushMessage(const std::shared_ptr<MSP::Messages::MSPBaseV2> &message) {
    if (!running) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(writeMutex);
        writeQueue.push(message);
    }
    writeCV.notify_one();
}

void MSPCommunicator::processMspData(char data) {
    if (readState == ReadState::WAIT_FIRST_BYTE) {
        readedIdx = 0;
    }

    readedData[readedIdx] = data;

    uint8_t payload_size_v1;
    uint16_t payload_size_v2;
    uint8_t calced_crc;

    switch (readState) {
    case ReadState::WAIT_FIRST_BYTE:
        if (data == '$')
            readState = ReadState::WAIT_VERSION; // 0
        break;
    case ReadState::WAIT_VERSION:
        if (data == 'X')
            readState = ReadState::WAIT_DIRECTION_V2; // 1
        else if (data == 'M')
            readState = ReadState::WAIT_DIRECTION;
        else {
            readedIdx = 0;
            readState = ReadState::WAIT_FIRST_BYTE;
        }
        break;
    case ReadState::WAIT_DIRECTION:
        if (data == '>')
            readState = ReadState::WAIT_SIZE_V1;
        else
            readState = ReadState::WAIT_FIRST_BYTE;
        break;
    case ReadState::WAIT_DIRECTION_V2:
        if (data == '>')
            readState = ReadState::WAIT_FLAG_V2; // 2
        else
            readState = ReadState::WAIT_FIRST_BYTE;
        break;
    case ReadState::WAIT_SIZE_V1:
        readState = ReadState::WAIT_COMMAND_V1;
        break;
    case ReadState::WAIT_COMMAND_V1:
        payload_size_v1 = readedData[3];
        if (payload_size_v1 == 0)
            readState = ReadState::WAIT_CRC;
        else
            readState = ReadState::WAIT_PAYLOAD;
        break;
    case ReadState::WAIT_PAYLOAD:
        payload_size_v1 = readedData[3];
        if (readedIdx == 5 + payload_size_v1) {
            readState = ReadState::WAIT_CRC;
        }
        break;
    case ReadState::WAIT_FLAG_V2:
        readState = ReadState::WAIT_COMMAND_V2; // 3
        break;
    case ReadState::WAIT_COMMAND_V2:
        if (readedIdx == 5) {
            readState = ReadState::WAIT_SIZE_V2; // 4 //5
        }
        break;
    case ReadState::WAIT_SIZE_V2:
        if (readedIdx == 7) {
            payload_size_v2 = static_cast<uint8_t>(readedData[6]) | (static_cast<uint8_t>(readedData[7]) << 8);
            if (payload_size_v2 == 0) {
                readState = ReadState::WAIT_CRC_V2;
            } else {
                readState = ReadState::WAIT_PAYLOAD_V2; // 6 //7
            }
        }
        break;
    case ReadState::WAIT_PAYLOAD_V2:
        payload_size_v2 = static_cast<uint8_t>(readedData[6]) | (static_cast<uint8_t>(readedData[7]) << 8);
        if (readedIdx == 7 + payload_size_v2) {
            readState = ReadState::WAIT_CRC_V2;
        }
        break;

    case ReadState::WAIT_CRC:
        readState = ReadState::WAIT_FIRST_BYTE;
        payload_size_v1 = readedData[3];
        calced_crc = compute_crc8_dvb_s2(readedData, 3, readedIdx);

        if (calced_crc != (uint8_t)data) {
            std::cout << "wrong crc (MSP V1)" << std::endl;
        }
        break;
    case ReadState::WAIT_CRC_V2: {
        readState = ReadState::WAIT_FIRST_BYTE;
        payload_size_v2 = static_cast<uint8_t>(readedData[6]) | (static_cast<uint8_t>(readedData[7]) << 8);

        calced_crc = compute_crc8_dvb_s2(readedData, 3, readedIdx);

        if (calced_crc != (uint8_t)data) {
            fprintf(stderr, "wrong crc (MSP V2)");
            break;
        }
        uint16_t msg_id = static_cast<uint8_t>(readedData[4]) | (static_cast<uint8_t>(readedData[5]) << 8);

        for (const auto &pair : callbackMapV2) {
            if (pair.first->id == msg_id) {
                size_t payload_size = pair.first->payloadSize();
                if (payload_size < payload_size_v2) {
                    fprintf(stderr, "payload size of message(%04X)=%lu != received size=%hu", msg_id, payload_size, payload_size_v2);
                } else {
                    char *payload = const_cast<char *>(pair.first->payload());

                    memcpy(payload, &readedData[8], payload_size_v2);
                    pair.second(pair.first);
                    // printf("\n");
                }
            }
        }

        break;
    }
    default:
        break;
    }

    readedIdx++;

    if (readedIdx >= sizeof(readedData)) {
        readState = ReadState::WAIT_FIRST_BYTE;
        std::cerr << "MSP Buffer overflow" << std::endl;
        readedIdx = 0;
    }
}