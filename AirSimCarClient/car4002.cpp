// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// #include "vehicles/car/api/CarRpcLibClient.hpp"
// #include <iostream>

#include <thread>
#include <fstream>
#include <jsoncpp/json/json.h>
#include "rotationUtility.h"
#include "asyncSocketClient.h"
#include "packet.h"
#include "collisionUtility.h"

using namespace std;
using namespace cPkt;
using namespace msr::airlib;


CarRpcLibClient thisCar;         // This car
char buffer[RCVBUFSIZE] = {0};
carPacket otherCar;
int otherCarPktReady = 0;
ifstream ifs("/home/cyber/Documents/AirSim/settings.json");
Json::Reader reader;
Json::Value obj;


void serializeCarData(CarRpcLibClient &car, char *carName, char *packetString) {
    CarApiBase::CarState state = car.getCarState(carName);
    Vector3r p = state.kinematics_estimated.pose.position;
    Quaternionr orient = state.kinematics_estimated.pose.orientation;
    sprintf(packetString, "Speed:%f,Gear:%d,PX:%f,PY:%f,PZ:%f,OW:%f,OX:%f,OY:%f,OZ:%f\r\n", state.speed, state.gear, p.x() + obj["Vehicles"][carName]["X"].asInt(), p.y() + obj["Vehicles"][carName]["Y"].asInt(), p.z() + obj["Vehicles"][carName]["Z"].asInt(), orient.w(), orient.x(), orient.y(), orient.z());
}

// Switching to threads, therefore this will be unused
void sigHandler(int signum) {
    readPacket(buffer);
    otherCar.setValues(buffer);
    otherCarPktReady = 1;
}

void packetReceiver() {
    while(true) {
      readPacket(buffer);
      otherCar.setValues(buffer);
      otherCarPktReady = 1;
    }
}

int main(int argc, char *argv[]) {
    thisCar.confirmConnection();
    char carName[10] = {0};
    sprintf(carName, "Car%d", atoi(argv[1]));
    thisCar.enableApiControl(true, carName);  //this disables manual control
    CarApiBase::CarControls controls;

    reader.parse(ifs, obj);

    setupSocket(atoi(argv[1]), sigHandler);
    std::thread first(packetReceiver);
    cout << "Sleeping!" << endl;

    std::this_thread::sleep_for(std::chrono::seconds(5));

    cout << "Go forward" << endl;
    controls.handbrake = false;
    controls.throttle = 0.3;
    thisCar.setCarControls(controls, carName);
    int collisionCounter = 0;

    while (true) {
        char packetString[200] = {0};
        serializeCarData(thisCar, carName, packetString);
        // cout << packetString << endl;
        sendPacket(packetString);
        // Get angle to turn the car to face other car
        if(otherCarPktReady == 1){
          CarApiBase::CarState thisCarState = thisCar.getCarState(carName);

          controls.steering = completeCalcSteering(thisCarState, otherCar, obj, carName);
          thisCar.setCarControls(controls, carName);

          // delete collisionVecx;

          otherCarPktReady = 0;

           // Need to take another look at the endpoint math, something is not working out
           // line segment math checks tested, seems to work
           // if (simpleCollisionDetect(otherCar, thisCarState)) {
           //   std::cout << "collision iminent" << collisionCounter++ << '\n';
           // }

           std::cout << "this car end" << getThisCarEndPoint(thisCarState) << endl;
           std::cout << "other car end" << getOtherCarEndPoint(otherCar) << endl;


         }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // sleep(1);
        // sleep(5);
        // cout << otherCarPktReady << endl;
    }

    thisCar.setCarControls(CarApiBase::CarControls());
    return 0;
}
