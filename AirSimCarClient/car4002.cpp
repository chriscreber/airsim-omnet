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
#include <Eigen/Geometry>

using namespace std;
using namespace msr::airlib;

struct carNetPacket *serializeCarData(CarRpcLibClient &car) {
    CarApiBase::CarState state = car.getCarState();
    Vector3r p = state.kinematics_estimated.pose.position;
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

CarRpcLibClient thisCar;         // This car
struct carNetPacket otherCar;
int otherCarPktReady = 0;

void sigHandler(int signum) {
    readPacket((unsigned char *)&otherCar, sizeof(otherCar));
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

    struct carNetPacket *packet = serializeCarData(thisCar);
    sendPacket((unsigned char *)packet, sizeof(*packet));
    delete packet;
    cout << "Sending message" << endl;

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
          CarApiBase::CarState thisCarState = thisCar.getCarState();
          Vector3r thisCarPosition = thisCarState.kinematics_estimated.pose.position;
          Quaternionr thisCarQuat = thisCarState.kinematics_estimated.pose.orientation;
          Vector3r otherCarPosition (otherCar.px, otherCar.py, otherCar.pz);
          cout << "This Car Position: " << thisCarPosition << " Other Car Position: " << otherCarPosition << endl;
          Vector3r *collisionVecx = getCollisionVector(thisCarPosition, otherCarPosition);
          cout << " CollisionVector: " << *collisionVecx << endl;
          Vector3r collisionVecz (0, 0, 1);
          Quaternionr collisionQuat = Eigen::Quaternion<float>::FromTwoVectors (*collisionVecx, collisionVecz);
          Quaternionr transformQuat = collisionQuat * thisCarQuat.inverse();      // Check order
          Vector3r turnAngles = transformQuat.toRotationMatrix().eulerAngles(2, 1, 2);
          cout << turnAngles.z() << endl;
          delete collisionVecx;

          sleep(2);
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
