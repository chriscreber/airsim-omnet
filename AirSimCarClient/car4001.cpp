// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// #include "vehicles/car/api/CarRpcLibClient.hpp"
// #include <iostream>
#include "common/common_utils/StrictMode.hpp"
STRICT_MODE_OFF
#ifndef RPCLIB_MSGPACK
#define RPCLIB_MSGPACK clmdep_msgpack
#endif // !RPCLIB_MSGPACK
#include "rpc/rpc_error.h"
STRICT_MODE_ON

#include "vehicles/car/api/CarRpcLibClient.hpp"
#include "common/common_utils/FileSystem.hpp"
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <unistd.h>
#include <ctime>
#include <cmath>

#include "asyncSocketClient.h"
#include "packet.h"

// these need to match the settings.json file
// TODO: Read settings.json so we don't have to hardcode values
#define STARTX 10
#define STARTY 10
#define STARTZ 0

using namespace std;
using namespace cPkt;
using namespace msr::airlib;

// clock_t startTime;

// struct carNetPacket *serializeCarData(CarRpcLibClient &car, char *carName) {
//     // cout << carName << endl;
//     CarApiBase::CarState state = car.getCarState(carName);
//     Vector3r p = state.kinematics_estimated.pose.position;
//     cout << p << endl;
//     Quaternionr orient = state.kinematics_estimated.pose.orientation;
//     struct carNetPacket *packet = new carNetPacket ();
//     // packet->timeElapsed = double(clock() - startTime);
//     packet->speed = state.speed;
//     packet->gear  = state.gear;
//     packet->px    = p.x();
//     packet->py    = p.y();
//     packet->pz    = p.z();
//     packet->qx    = orient.x();
//     packet->qy    = orient.y();
//     packet->qz    = orient.z();
//     packet->qw    = orient.w();
//     packet->eof   = '\r';
//     return packet;
// }

void serializeCarData(CarRpcLibClient &car, char *carName, char *packetString) {
    // cout << carName << endl;
    CarApiBase::CarState state = car.getCarState(carName);
    Vector3r p = state.kinematics_estimated.pose.position;
    cout << p << endl;
    Quaternionr orient = state.kinematics_estimated.pose.orientation;
    sprintf(packetString, "Speed:%f,Gear:%d,PX:%f,PY:%f,PZ:%f,OW:%f,OX:%f,OY:%f,OZ:%f\r\n", state.speed, state.gear, p.x() + STARTX, p.y() + STARTY, p.z() + STARTZ, orient.w(), orient.x(), orient.y(), orient.z());
    // sprintf(packetString, "Speed:%f,Gear:%d,PX:%f,PY:%f,PZ:%f,OW:%f,OX:%f,OY:%f,OZ:%f\r\n", state.speed, state.gear, p.y() + STARTY, p.x() + STARTX, p.z() + STARTZ, orient.w(), orient.x(), orient.y(), orient.z());

}

CarRpcLibClient client;         // This car
// struct carNetPacket otherCar;
char buffer[RCVBUFSIZE] = {0};
carPacket otherCar;
int pktReady = 0;

void sigHandler(int signum) {
    // readPacket((unsigned char *)&otherCar, sizeof(otherCar));
    readPacket(buffer);
    otherCar.setValues(buffer);
    cout << otherCar.Speed << endl;
    pktReady = 1;
}

int main(int argc, char *argv[]) {
    // startTime = clock();
    client.confirmConnection();
    char carName[10] = {0};
    sprintf(carName, "Car%d", atoi(argv[1]));
    // client.enableApiControl(true, carName);  //this disables manual control
    CarApiBase::CarControls controls;

    setupSocket(atoi(argv[1]), sigHandler);
    cout << "Sleeping!" << endl;
    sleep(5);

    /*
    cout << "Go forward" << endl;
    controls.handbrake = false;
    controls.throttle = 1;
    client.setCarControls(controls, carName);
    */

    while (true) {
        // struct carNetPacket *packet = serializeCarData(client, carName);
        char packetString[200] = {0};
        serializeCarData(client, carName, packetString);
        cout << packetString << endl;
        sendPacket(packetString);
        // sendPacket((unsigned char *)packet, sizeof(*packet));
        // delete packet;
        cout << "Sending message" << endl;
        if(pktReady) {
          cout << "hi" << endl;
          pktReady = 0;
        }
        // controls.handbrake = false;
        // controls.throttle = 0.1;
        // client.setCarControls(controls, carName);
        // sleep(1);
        // controls.is_manual_gear = false;
        // controls.manual_gear = 0;
        // controls.handbrake = true;
        // client.setCarControls(controls, carName);
        // sleep(1);
        // controls.handbrake = false;
        // controls.throttle = 0.1;
        // controls.is_manual_gear = true;
        // controls.manual_gear = -1;
        // client.setCarControls(controls, carName);
        // sleep(1);
        // controls.is_manual_gear = false;
        // controls.manual_gear = 0;
        // controls.handbrake = true;
        // client.setCarControls(controls, carName);
        sleep(2);
    }

    client.setCarControls(CarApiBase::CarControls());
    return 0;
}
