#include <iostream>
#include <termio.h>

#include "msp/messages/motor.h"
#include "msp/messages/sim.h"
#include "msp/messages/raw_rc.h"
#include "msp/messages/reboot.h"
#include "msp/communication/iface/tcpiface.h"

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

    TcpIface *tcp = new TcpIface();

    MSP::Messages::SIMULATOR_REQUEST sim;
    sim.data.version = 2;
    sim.data.flags = MSP::Messages::simulatorFlags_t::HITL_ENABLE |
                MSP::Messages::simulatorFlags_t::HITL_SIMULATE_BATTERY |
                MSP::Messages::simulatorFlags_t::HITL_EXT_BATTERY_VOLTAGE;
    sim.data.vbat = 125;

    MSP::Messages::SET_RAW_RC_Request raw_rc;
    raw_rc.data.channels[0] = 1000;
    raw_rc.data.channels[1] = 1000;
    raw_rc.data.channels[2] = 1500;
    raw_rc.data.channels[3] = 1500;

    MSP::Messages::REBOOT_REQUEST reboot;

    MSP::Messages::MOTOR_REQUEST motorRequest;

    while (true) {
        if (kbhit()) { 
            char key = getchar();
            
            if (key == ' ') {
                if (tcp->writeMSP(sim))
                    std::cout << "Sent SIM packet" << std::endl;
                else
                    std::cerr << "Failed to send SIM packet" << std::endl;
            }
            // else if (key == 'j' || key == 'J'){ 
            //     if (tcp->writeMSP(raw_rc ))
            //         std::cout << "Sent RAW_RC packet" << std::endl;
            //     else
            //         std::cerr << "Failed to send RAW_RC packet" << std::endl;
            // }
            // else if (key == 'r' || key == 'R'){ 
            //     if (tcp->writeMSP(reboot))
            //         std::cout << "Sent Reboot packet" << std::endl;
            //     else
            //         std::cerr << "Failed to send Reboot packet" << std::endl;
            // }
            else if (key == 'q' || key == 'Q') {
                std::cout << "Exiting program..." << std::endl;
                break;
            }
            else if (key == 'c' || key == 'C')
            {
                if (tcp->connect())
                {
                    std::cout << "Connected successfully" << std::endl;
                }
                else {
                    std::cout << "Failed to connect" << std::endl;
                }
            }
        }
        
        // Small delay to prevent CPU overuse
        usleep(10000);  // 10ms delay
    }

    return 0;
}