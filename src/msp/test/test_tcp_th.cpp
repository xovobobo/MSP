#include <functional>
#include <iostream>
#include <memory>
#include <termio.h>

#include <msp/communication/iface/tcpiface.h>
#include <msp/communication/mspcommunicator.h>
#include <msp/messages/motor.h>
#include <msp/messages/raw_rc.h>
#include <msp/messages/reboot.h>
#include <msp/messages/sim.h>

void onSimResponse(const std::shared_ptr<MSP::Messages::MSPBaseV2> &baseMsg) {
    auto msg = std::dynamic_pointer_cast<MSP::Messages::SIMULATOR_RESPONSE>(baseMsg);

    std::cout << "on sim cb" << std::endl;

    char *payload = const_cast<char *>(msg->payload());
    for (size_t i = 0; i < msg->payloadSize(); i++) {
        printf("%02X ", static_cast<unsigned char>(payload[i]));
    }
    printf("\n");

    // printf("version is: %d \n", msg->data.version);
}

void onMotors(const std::shared_ptr<MSP::Messages::MSPBaseV2> &baseMsg) {
    auto msg = std::dynamic_pointer_cast<MSP::Messages::MOTOR_RESPONSE>(baseMsg);

    std::cout << "on motors cb" << std::endl;

    char *payload = const_cast<char *>(msg->payload());
    for (size_t i = 0; i < msg->payloadSize(); i++) {
        printf("%02X ", static_cast<unsigned char>(payload[i]));
    }
    printf("\n");

    // printf("version is: %d \n", msg->data.version);
}

int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

int main() {

    MSP::Messages::SIMULATOR_REQUEST sim;
    sim.data.version = 2;
    sim.data.flags = MSP::Messages::simulatorFlags_t::HITL_ENABLE | MSP::Messages::simulatorFlags_t::HITL_SIMULATE_BATTERY | MSP::Messages::simulatorFlags_t::HITL_EXT_BATTERY_VOLTAGE;
    sim.data.vbat = 125;

    char *payload_orig = const_cast<char *>(sim.payload());

    std::cout << sim.payloadSize() << std::endl;
    for (size_t i = 0; i < sim.payloadSize(); i++) {
        printf("%02X ", static_cast<unsigned char>(payload_orig[i]));
    }
    std::cout << std::endl;

    MSP::Messages::SET_RAW_RC_Request raw_rc;
    raw_rc.data.channels[0] = 1000;
    raw_rc.data.channels[1] = 1000;
    raw_rc.data.channels[2] = 1500;
    raw_rc.data.channels[3] = 1500;

    MSP::Messages::REBOOT_REQUEST reboot;

    MSP::Messages::MOTOR_REQUEST motorRequest;

    MSPCommunicator *mspComm = new MSPCommunicator();
    TcpIface *tcp = new TcpIface();
    mspComm->setIface(tcp);
    mspComm->registerCallbackV2<MSP::Messages::SIMULATOR_RESPONSE>(onSimResponse);
    mspComm->registerCallbackV2<MSP::Messages::MOTOR_RESPONSE>(onMotors);

    auto test = std::make_shared<MSP::Messages::SIMULATOR_REQUEST>(sim);
    char *payload = const_cast<char *>(test->payload());
    payload[0] = 4;
    std::cout << test->data.version << std::endl;

    for (size_t i = 0; i < test->payloadSize(); i++) {
        printf("%02X ", static_cast<unsigned char>(payload[i]));
    }
    std::cout << std::endl;

    while (true) {
        if (kbhit()) {
            char key = getchar();

            if (key == ' ') {
                mspComm->pushMessage(std::make_shared<MSP::Messages::SIMULATOR_REQUEST>(sim));
            } else if (key == 'j' || key == 'J') {
                mspComm->pushMessage(std::make_shared<MSP::Messages::SET_RAW_RC_Request>(raw_rc));
            } else if (key == 'r' || key == 'R') {
                mspComm->pushMessage(std::make_shared<MSP::Messages::REBOOT_REQUEST>(reboot));
            } else if (key == 'm' || key == 'M') {
                mspComm->pushMessage(std::make_shared<MSP::Messages::MOTOR_REQUEST>());
            } else if (key == 'q' || key == 'Q') {
                std::cout << "Exiting program..." << std::endl;
                break;
            } else if (key == 'C' || key == 'c') {
                std::cout << "Closing..." << std::endl;
                if (mspComm->stop()) {
                    std::cout << "started" << std::endl;
                } else {
                    std::cout << "failed to start" << std::endl;
                }
            } else if (key == 'O' || key == 'O') {
                std::cout << "Opening..." << std::endl;
                if (mspComm->start()) {
                    std::cout << "started" << std::endl;
                } else {
                    std::cout << "failed to open";
                }
            } else if (key == 'c' || key == 'C') {
                mspComm->start();
            }
        }

        // Small delay to prevent CPU overuse
        usleep(10000); // 10ms delay
    }

    return 0;
}