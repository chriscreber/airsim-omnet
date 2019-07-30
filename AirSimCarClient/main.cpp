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

int main(int argc, char *argv[]) {
    using namespace msr::airlib;
    using namespace std;

    setupSocket(atoi(argv[1]));
    sendPacket("Packet 1!\r");

    CarRpcLibClient client;
    client.confirmConnection();
    client.enableApiControl(true, "Car1"); //this disables manual control
    CarApiBase::CarControls controls;

    std::cout << "Reversing" << std::endl;;
    controls.throttle = 0.5;
    controls.is_manual_gear = true;
    controls.manual_gear = -1;
    client.setCarControls(controls, "Car1");


    sleep(6);

    cout << "Breaking" << endl;
    controls.is_manual_gear = false;
    controls.manual_gear = 0;
    controls.handbrake = true;
    client.setCarControls(controls, "Car1");

    sleep(1);

    cout << "Go forward" << endl;
    controls.handbrake = false;
    controls.throttle = 1;
    client.setCarControls(controls, "Car1");

    sleep(10);

    // std::cout << "Press Enter to take turn and drive backward" << std::endl; std::cin.get();
    // controls.handbrake = false;
    // controls.throttle = -1;
    // controls.steering = 1;
    // client.setCarControls(controls, "Car1");
    //
    // std::cout << "Press Enter to stop" << std::endl; std::cin.get();
    client.setCarControls(CarApiBase::CarControls());

    return 0;
}
