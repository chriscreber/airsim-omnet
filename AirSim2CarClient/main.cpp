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
#include <unistd.h>



int main()
{
    using namespace msr::airlib;
    using namespace std;

    CarRpcLibClient client;
    client.confirmConnection();
    client.enableApiControl(true, "Car2"); //this disables manual control
    CarApiBase::CarControls controls;


    cout << "Go reverse and steer right" << endl;
    controls.throttle = -0.5;
    controls.is_manual_gear = true;
    controls.manual_gear = -1;
    controls.steering = -0.5;
    client.setCarControls(controls, "Car2");

    sleep(5);

    controls.steering = 0;
    client.setCarControls(controls, "Car2");
    sleep(1);

    cout << "Breaking" << endl;
    controls.is_manual_gear = false;
    controls.manual_gear = 0;
    controls.handbrake = 1;
    client.setCarControls(controls, "Car2");

    sleep(1);

    cout << "Go forward" << endl;
    controls.handbrake = 0;
    controls.throttle = 1;
    controls.steering = 0;
    client.setCarControls(controls, "Car2");

    sleep(10);

    // std::cout << "Press Enter to activate handbrake" << std::endl; std::cin.get();
    // controls.handbrake = true;
    // client.setCarControls(controls, "Car2");
    //
    // std::cout << "Press Enter to take turn and drive backward" << std::endl; std::cin.get();
    // controls.handbrake = false;
    // controls.throttle = -1;
    // controls.steering = 1;
    // client.setCarControls(controls, "Car2");
    //
    // std::cout << "Press Enter to stop" << std::endl; std::cin.get();
    client.setCarControls(CarApiBase::CarControls());

    return 0;
}
