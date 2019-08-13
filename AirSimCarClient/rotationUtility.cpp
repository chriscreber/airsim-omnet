#include "rotationUtility.h"


using namespace std;
using namespace msr::airlib;

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
// returns in radians
// facing "forward or x-axis in unreal" is 0, then going clockwise is to positive pi and counter-clockwise is to negative pi
float getZAngleFromQuat(Quaternionr &thisCarQuat) {
  float theta;
  theta = 2 * acos(thisCarQuat.w());
  if(thisCarQuat.z() < 0) {
    theta = -1 * theta;
  }

  return theta;
}

// calculate turn angle
float calcTurnAngle(float collisionAngle, float thisCarAngle) {
  float turnAngle = collisionAngle - thisCarAngle;
  if(turnAngle < -1 * M_PI) {
    turnAngle = turnAngle + 2 * M_PI;
  } else if(turnAngle > M_PI) {
    turnAngle = turnAngle - 2 * M_PI;
  }

  return turnAngle;
}

// get the steering value from the turn angle. Between -0.5 and 0.5
float calcSteering(float turnAngle) {
  if(turnAngle < M_PI / -2) {
    turnAngle = -0.5;
  } else if(turnAngle > M_PI) {
    turnAngle = 0.5;
  } else {
    turnAngle = turnAngle / (M_PI / 2);
  }

  return turnAngle;
}

float completeCalcSteering(CarApiBase::CarState thisCarState, carPacket otherCar, Json::Value obj, char *carName) {
  Vector3r _thisCarPosition = thisCarState.kinematics_estimated.pose.position;
  Vector3r thisCarPosition (_thisCarPosition.x() + obj["Vehicles"][carName]["X"].asInt(), _thisCarPosition.y() + obj["Vehicles"][carName]["Y"].asInt(), _thisCarPosition.z() + obj["Vehicles"][carName]["Z"].asInt());

  Quaternionr thisCarQuat = thisCarState.kinematics_estimated.pose.orientation;
  Vector3r otherCarPosition (otherCar.PX, otherCar.PY, otherCar.PZ);
  Vector3r *collisionVecx = getCollisionVector(thisCarPosition, otherCarPosition);

  float collisionAngle = getCollisionAngle(*collisionVecx);
  float thisCarAngle = getZAngleFromQuat(thisCarQuat);
  float turnAngle = calcTurnAngle(collisionAngle, thisCarAngle);

  return calcSteering(turnAngle);
}

// How to get the car to hit the other car.
// CarApiBase::CarState thisCarState = thisCar.getCarState(carName);
// Vector3r _thisCarPosition = thisCarState.kinematics_estimated.pose.position;
// Vector3r thisCarPosition (_thisCarPosition.x() + STARTX, _thisCarPosition.y() + STARTY, _thisCarPosition.z() + STARTZ);
// Quaternionr thisCarQuat = thisCarState.kinematics_estimated.pose.orientation;
// Vector3r otherCarPosition (otherCar.PX, otherCar.PY, otherCar.PZ);
// Vector3r *collisionVecx = getCollisionVector(thisCarPosition, otherCarPosition);
//
// float collisionAngle = getCollisionAngle(*collisionVecx);
// float thisCarAngle = getZAngleFromQuat(thisCarQuat);
// float turnAngle = calcTurnAngle(collisionAngle, thisCarAngle);
//
// controls.steering = calcSteering(turnAngle);
// thisCar.setCarControls(controls, carName);
