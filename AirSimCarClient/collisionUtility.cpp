#include "collisionUtility.h"
#include "rotationUtility.h"
#include "packet.h"

using namespace std;
using namespace cPkt;
using namespace msr::airlib;

// overarching function that calls the helpers on the bottom
// need to add a for loop because the line segment created is really thin otherwise and misses thickness of the car
bool simpleCollisionDetect(carPacket otherCar, CarApiBase::CarState thisCar) {
    Vector3r thisCarBegPt = thisCar.kinematics_estimated.pose.position;
    Vector3r otherCarBegPt (otherCar.PX, otherCar.PY, otherCar.PZ);
    Vector3r thisCarEndPt = getThisCarEndPoint(thisCar);
    Vector3r otherCarEndPt = getOtherCarEndPoint(otherCar);
    return doIntersect(thisCarBegPt, thisCarEndPt, otherCarBegPt, otherCarEndPt);
}

// calculates the position of the other car in the next 0.5 seconds (adjustable)
// assumes the car is travelling linearly, does not take into account steering angle
// assume for now one UU = 1 meter, if things go wrong forums say 1 UU = 1cm.
// DOES NOT HANDLE Z change, cause quaternions
Vector3r getOtherCarEndPoint(carPacket otherCar) {

    float inputTime = 3;
    float hypotenuse = otherCar.Speed * inputTime;

    // orientation of Z angle from rotation utils
    Quaternionr otherCarQuat(otherCar.OW, otherCar.OX, otherCar.OY, otherCar.OZ);
    float otherCarRadian = getZAngleFromQuat(otherCarQuat);

    float xDelta = hypotenuse * cos(otherCarRadian);
    if (abs(otherCarRadian) > (M_PI / 2)) {
      xDelta = abs(xDelta) * -1;
    } else {
      xDelta = abs(xDelta);
    }

    float yDelta = hypotenuse * sin(otherCarRadian);
    if (otherCarRadian < 0) {
      yDelta = abs(yDelta) * -1;
    } else {
      yDelta = abs(yDelta);
    }

    // if (otherCar.Speed < 0) {
    //   yDelta = yDelta * -1;
    //   xDelta = xDelta * -1;
    // }

    Vector3r retVector(otherCar.PX + xDelta, otherCar.PY + yDelta, otherCar.PZ);
    std::cout << "other car endpoint is " << retVector << '\n';
    return retVector;
}

// calculates the postion of this car will be in the next 0.5 seconds (adjustable)
// assumes the car is travelling linearly, does not take into account steering angle
// assume for now one UU = 1 meter, if things go wrong forums say 1 UU = 1cm.
// DOES NOT HANDLE Z change, cause quaternions
Vector3r getThisCarEndPoint(CarApiBase::CarState thisCar) {

    float inputTime = 3;
    float hypotenuse = thisCar.speed * inputTime;

    // orientation of Z angle from rotation utils
    Quaternionr thisCarQuat = thisCar.kinematics_estimated.pose.orientation;
    float thisCarRadian = getZAngleFromQuat(thisCarQuat);
    Vector3r thisCarVector = thisCar.kinematics_estimated.pose.position;

    float xDelta = hypotenuse * cos(thisCarRadian);
    if (abs(thisCarRadian) > (M_PI / 2)) {
      xDelta = abs(xDelta) * -1;
    } else {
      xDelta = abs(xDelta);
    }

    float yDelta = hypotenuse * sin(thisCarRadian);
    if (thisCarRadian < 0) {
      yDelta = abs(yDelta) * -1;
    } else {
      yDelta = abs(yDelta);
    }

    // if (thisCar.speed < 0) {
    //   yDelta = yDelta * -1;
    //   xDelta = xDelta * -1;
    // }

    Vector3r retVector(thisCarVector.x() + xDelta, thisCarVector.y() + yDelta, thisCarVector.z());
    std::cout << "this car endpoint is " << retVector << '\n';

    return retVector;
}

// Given three points p, q, r, the function checks if point q lies on line segment 'pr'
// Stolen from GeeksforGeeks
bool onSegment(Vector3r p, Vector3r q, Vector3r r)
{
    if (q.x() <= max(p.x(), r.x()) && q.x() >= min(p.x(), r.x()) &&
        q.y() <= max(p.y(), r.y()) && q.y() >= min(p.y(), r.y())) {
          return true;
        }

    return false;
}


// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are colinear
// 1 --> Clockwise
// 2 --> Counterclockwise
// Stolent from GeeksforGeeks
int orientation(Vector3r p, Vector3r q, Vector3r r)
{
  // for details of below formula.
  int val = (q.y() - p.y()) * (r.x() - q.x()) -
            (q.x() - p.x()) * (r.y() - q.y());

  if (val == 0) {
    return 0;
  } // colinear

  return (val > 0)? 1: 2; // clock or counterclock wise
}

// stolen from GeeksforGeeks, calculate intersection between two line segments
bool doIntersect(Vector3r tCarBeg, Vector3r tCarEnd, Vector3r oCarBeg, Vector3r oCarEnd)
{

  // Find the four orientations needed for general and
  // special cases
  int o1 = orientation(tCarBeg, tCarEnd, oCarBeg);
  int o2 = orientation(tCarBeg, tCarEnd, oCarEnd);
  int o3 = orientation(oCarBeg, oCarEnd, tCarBeg);
  int o4 = orientation(oCarBeg, oCarEnd, tCarEnd);

  // General case
  if (o1 != o2 && o3 != o4) {
    return true;
  }

  // Special Cases
  if (o1 == 0 && onSegment(tCarBeg, oCarBeg, tCarEnd)) {
    return true;
  }

  if (o2 == 0 && onSegment(tCarBeg, oCarEnd, tCarEnd)) {
    return true;
  }

  if (o3 == 0 && onSegment(oCarBeg, tCarBeg, oCarEnd)) {
    return true;
  }

  if (o4 == 0 && onSegment(oCarBeg, tCarEnd, oCarEnd)) {
    return true;
  }

  return false; // Doesn't fall in any of the above cases
}
