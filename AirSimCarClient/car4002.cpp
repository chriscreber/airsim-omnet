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
#include <tgmath.h>


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

// return angle of otherCar from x axis in radians
float getCollisionAngle(Vector3r &collisionVecx) {
    float theta;
    theta = asin(collisionVecx.y() / hypot(collisionVecx.x(), collisionVecx.y()));
    if(collisionVecx.x() < 0) {
      if(collisionVecx.y() < 0) {
        theta = M_PI - theta;
      } else if(collisionVecx.y() > 0) {
        theta = -1 * M_PI - theta;
      } else if(theta < 0) {
        theta = M_PI;
      }
    }

    return theta;
}

// return angle of this car around z axis from x axis in radians (CW is positive, CCW is negative)
float getZAngleFromQuat(Quaternionr &thisCarQuat) {
  float theta;
  theta = 2 * acos(thisCarQuat.w());
  if(thisCarQuat.z() < 0) {
    theta = -1 * theta;
  }

  return theta;
}



Quaternionr RotationBetweenVectors(Vector3r start, Vector3r dest){
  dest.normalize();
  start.normalize();

	float cosTheta = start.dot(dest);
	Vector3r rotationAxis;

	if (cosTheta < -1 + 0.001f){
		// special case when vectors in opposite directions:
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		rotationAxis = Vector3r(0.0f, 0.0f, 1.0f).cross(start);
		if (rotationAxis.squaredNorm() < 0.01 ) // bad luck, they were parallel, try again!
			rotationAxis = Vector3r(1.0f, 0.0f, 0.0f).cross(start);

    rotationAxis.normalize();
    Eigen::Quaternion<float> retAxis;
    retAxis = Eigen::AngleAxis<float>(180.0, rotationAxis);
		return retAxis;
	}

  rotationAxis = start.cross(dest);

	float s = sqrt( (1+cosTheta)*2 );
	float invs = 1 / s;

	Quaternionr retQuat(
		(s * 0.5f),
		(rotationAxis.x() * invs),
		(rotationAxis.y() * invs),
		(rotationAxis.z() * invs)
	);

  return retQuat;

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

        // Get angle to turn the car to face other car
        if(otherCarPktReady == 1){
          CarApiBase::CarState thisCarState = thisCar.getCarState(carName);
          Vector3r _thisCarPosition = thisCarState.kinematics_estimated.pose.position;
          Vector3r thisCarPosition (_thisCarPosition.x() + STARTX, _thisCarPosition.y() + STARTY, _thisCarPosition.z() + STARTZ);
          // Vector3r thisCarPosition (_thisCarPosition.y() + STARTY, _thisCarPosition.x() + STARTX, _thisCarPosition.z() + STARTZ);
          Quaternionr thisCarQuat = thisCarState.kinematics_estimated.pose.orientation;
          // Quaternionr thisCarQuat (-1 * _thisCarQuat.w(), _thisCarQuat.x(), _thisCarQuat.y(), _thisCarQuat.z());
          cout << "thisCarQuat: " << thisCarQuat << endl;
          Vector3r otherCarPosition (otherCar.PX, otherCar.PY, otherCar.PZ);
          cout << "This Car Position: " << thisCarPosition << " Other Car Position: " << otherCarPosition << endl;
          Vector3r *collisionVecx = getCollisionVector(thisCarPosition, otherCarPosition);
          cout << " CollisionVectorx: " << *collisionVecx << endl;

          //START
          float collisionAngle = getCollisionAngle(*collisionVecx);
          float thisCarAngle = getZAngleFromQuat(thisCarQuat);
          float turnAngle = collisionAngle - thisCarAngle;

          controls.steering = turnAngle / (2 * M_PI);
          thisCar.setCarControls(controls, carName);


          //END
          // Vector3r collisionVecz (0, 0, 1);
          // Quaternionr collisionQuat = Eigen::Quaternion<float>::FromTwoVectors (*collisionVecx, collisionVecz);
          // Quaternionr transformQuat = collisionQuat * thisCarQuat.inverse();      // Check order
          // Vector3r turnAngles = transformQuat.toRotationMatrix().eulerAngles(2, 1, 2);
          // cout << "turnAngle.z(): " << turnAngles.z() << endl;
          //
          // controls.steering = turnAngles.z() / (-2 * M_PI);
          // thisCar.setCarControls(controls, carName);

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
