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
#include <cmath>

#include "asyncSocketClient.h"
#include "packet.h"
#include <Eigen/Geometry>

using namespace std;
using namespace cPkt;
using namespace msr::airlib;

// these need to match the settings.json file
// TODO: Read settings.json so we don't have to hardcode values
#define STARTX -20
#define STARTY 0
#define STARTZ 0

void serializeCarData(CarRpcLibClient &car, char *carName, char *packetString) {
    // cout << carName << endl;
    CarApiBase::CarState state = car.getCarState(carName);
    Vector3r p = state.kinematics_estimated.pose.position;
    cout << p << endl;
    Quaternionr orient = state.kinematics_estimated.pose.orientation;
    sprintf(packetString, "Speed:%f,Gear:%d,PX:%f,PY:%f,PZ:%f,OW:%f,OX:%f,OY:%f,OZ:%f\r\n", state.speed, state.gear, p.x() + STARTX, p.y() + STARTY, p.z() + STARTZ, orient.w(), orient.x(), orient.y(), orient.z());
}

CarRpcLibClient thisCar;         // This car
// struct carNetPacket otherCar;
char buffer[RCVBUFSIZE] = {0};
carPacket otherCar;
int otherCarPktReady = 0;

void sigHandler(int signum) {
    readPacket(buffer);
    otherCar.setValues(buffer);

    otherCarPktReady = 1;
}

Vector3r *getCollisionVector(Vector3r &thisCarPos, Vector3r &otherCarPos) {
    Vector3r _collisionVector = otherCarPos - thisCarPos;
    Vector3r *collisionVector = new Vector3r (_collisionVector);
    return collisionVector;
}

int main(int argc, char *argv[]) {
    thisCar.confirmConnection();
    char carName[10] = {0};
    sprintf(carName, "Car%d", atoi(argv[1]));
    thisCar.enableApiControl(true, carName);  //this disables manual control
    CarApiBase::CarControls controls;

    setupSocket(atoi(argv[1]), sigHandler);
    cout << "Sleeping!" << endl;
    sleep(5);

    // struct carNetPacket *packet = serializeCarData(thisCar, carName);
    // sendPacket((unsigned char *)packet, sizeof(*packet));
    // delete packet;
    // cout << "Sending message" << endl;

    cout << "Go forward" << endl;
    controls.handbrake = false;
    controls.throttle = 0.3;
    thisCar.setCarControls(controls, carName);

    while (true) {
        /*
        struct carNetPacket *packet = serializeCarData(thisCar);
        sendPacket((unsigned char *)packet, sizeof(*packet));
        delete packet;
        cout << "Sending message" << endl;

        std::cout << "Reversing" << std::endl;;
        controls.throttle = 0.5;
        controls.is_manual_gear = true;
        controls.manual_gear = -1;
        thisCar.setCarControls(controls, carName);

        sleep(2);
        cout << "Breaking" << endl;
        controls.is_manual_gear = false;
        controls.manual_gear = 0;
        controls.handbrake = true;
        thisCar.setCarControls(controls, carName);
        */

        // Get angle to turn the carto face other car
        if(otherCarPktReady == 1){
          CarApiBase::CarState thisCarState = thisCar.getCarState(carName);
          Vector3r _thisCarPosition = thisCarState.kinematics_estimated.pose.position;
          Vector3r thisCarPosition (_thisCarPosition.x() + STARTX, _thisCarPosition.y() + STARTY, _thisCarPosition.z() + STARTZ);
          Quaternionr thisCarQuat = thisCarState.kinematics_estimated.pose.orientation;
          cout << "thisCarQuat: " << thisCarQuat << endl;
          Vector3r otherCarPosition (otherCar.PX, otherCar.PY, otherCar.PZ);
          cout << "This Car Position: " << thisCarPosition << " Other Car Position: " << otherCarPosition << endl;
          Vector3r *collisionVecx = getCollisionVector(thisCarPosition, otherCarPosition);
          cout << " CollisionVectorx: " << *collisionVecx << endl;
          Vector3r collisionVecz (0, 0, 1);
          Quaternionr collisionQuat = Eigen::Quaternion<float>::FromTwoVectors (*collisionVecx, collisionVecz);
          Quaternionr transformQuat = collisionQuat * thisCarQuat.inverse();      // Check order
          Vector3r turnAngles = transformQuat.toRotationMatrix().eulerAngles(2, 2, 2);
          cout << "turnAngle.z(): " << turnAngles.z() << endl;

          controls.steering = turnAngles.z() / (-2 * M_PI);
          thisCar.setCarControls(controls, carName);

          delete collisionVecx;

          // sleep(2);
          /*
          cout << "Go forward" << endl;
          controls.handbrake = false;
          controls.throttle = 1;
          thisCar.setCarControls(controls, carName);
          */


          otherCarPktReady = 0;
      }
      // cout << otherCarPktReady << endl;
    }

    thisCar.setCarControls(CarApiBase::CarControls());
    return 0;
}
