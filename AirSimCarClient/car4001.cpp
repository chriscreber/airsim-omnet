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

#include "asyncSocketClient.h"
#include "packet.h"

using namespace std;
using namespace msr::airlib;

struct carNetPacket *serializeCarData(CarRpcLibClient &car) {
    CarApiBase::CarState state = car.getCarState();
    Vector3r p = state.kinematics_estimated.pose.position;
    cout << p << endl;
    Quaternionr orient = state.kinematics_estimated.pose.orientation;
    struct carNetPacket *packet = new carNetPacket ();
    packet->speed = state.speed;
    packet->gear  = state.gear;
    packet->px    = p.x();
    packet->py    = p.y();
    packet->pz    = p.z();
    packet->qx    = orient.x();
    packet->qy    = orient.y();
    packet->qz    = orient.z();
    packet->qw    = orient.w();
    packet->eof   = '\r';
    return packet;
}

CarRpcLibClient client;         // This car
struct carNetPacket otherCar;

void sigHandler(int signum) {
    readPacket((unsigned char *)&otherCar, sizeof(otherCar));
    cout << otherCar.speed << endl;
}

int main(int argc, char *argv[]) {
    client.confirmConnection();
    char carName[10] = {0};
    sprintf(carName, "Car%d", atoi(argv[1]));
    client.enableApiControl(true, carName);  //this disables manual control
    CarApiBase::CarControls controls;

    setupSocket(atoi(argv[1]), sigHandler);
    cout << "Sleeping!" << endl;
    sleep(5);

    /*
    cout << "Go forward" << endl;
    controls.handbrake = false;
    controls.throttle = 10;
    client.setCarControls(controls, carName);
    */

    while (true) {
        struct carNetPacket *packet = serializeCarData(client);
        sendPacket((unsigned char *)packet, sizeof(*packet));
        delete packet;
        cout << "Sending message" << endl;

        sleep(1);
    }

    client.setCarControls(CarApiBase::CarControls());
    return 0;
}
