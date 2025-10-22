cmake_minimum_required(VERSION 3.10.0)
project(MSP VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS system)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

add_library(MSPMessages INTERFACE
    src/msp/messages/base.h
    src/msp/messages/sim.h
    src/msp/messages/raw_rc.h
    src/msp/messages/reboot.h
    src/msp/messages/motor.h
    src/msp/messages/servo.h
)
target_include_directories(MSPMessages INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

get_target_property(MSPMessages_INCLUDES MSPMessages INTERFACE_INCLUDE_DIRECTORIES)
message(STATUS "MSPMessages include directories: ${MSPMessages_INCLUDES}")


add_library(MSPCommunication 
    src/msp/communication/iface/crc8_dvb_s2.cpp
    src/msp/communication/iface/crc8_dvb_s2.h
    src/msp/communication/iface/absinterface.cpp
    src/msp/communication/iface/absinterface.h
    src/msp/communication/iface/tcpiface.cpp
    src/msp/communication/iface/tcpiface.h
    src/msp/communication/mspcommunicator.cpp
    src/msp/communication/mspcommunicator.h
    )

target_link_libraries(MSPCommunication
    PRIVATE
        ${Boost_SYSTEM_LIBRARY}
        MSPMessages
)


add_executable(test_tcp src/msp/test/test_tcp.cpp)
target_link_libraries(test_tcp
    MSPCommunication
    MSPMessages
    ${Boost_SYSTEM_LIBRARY}
)

add_executable(test_tcp_th src/msp/test/test_tcp_th.cpp)
target_link_libraries(test_tcp_th
    MSPCommunication
    MSPMessages
    ${Boost_SYSTEM_LIBRARY}
)