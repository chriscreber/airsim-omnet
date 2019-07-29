// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "vehicles/car/api/CarRpcLibClient.hpp"
#include <iostream>



int main() 
{
    using namespace msr::airlib;
    using namespace std;

    CarRpcLibClient client;
    client.enableApiControl(true); //this disables manual control
    CarApiBase::CarControls controls;

    std::cout << "Press enter to drive forward" << std::endl; std::cin.get();
    controls.throttle = 1;
    client.setCarControls(controls);

    std::cout << "Press Enter to activate handbrake" << std::endl; std::cin.get();
    controls.handbrake = true;
    client.setCarControls(controls);

    std::cout << "Press Enter to take turn and drive backward" << std::endl; std::cin.get();
    controls.handbrake = false;
    controls.throttle = -1;
    controls.steering = 1;
    client.setCarControls(controls);

    std::cout << "Press Enter to stop" << std::endl; std::cin.get();
    client.setCarControls(CarApiBase::CarControls());

    return 0;
}
