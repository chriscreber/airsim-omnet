#include "collisionUtility.h"
#include "rotationUtility.h"
#include "packet.h"

using namespace std;
using namespace cPkt;
using namespace msr::airlib;



// for slightly more advanced steering: (Steering Coefficient, Car Speed (Magnitude))
// (1, 0.0), (.6055, 16.233), (0.27773, 43.573), (0.1992, 117.026 >=)
// Found steeing angle to be 50 degrees for front SUV wheel.
// Estimating car to be 3 Unreal Units in width and 5 Unreal Units in length


// For creating improved collision prediction: (still not complex and not generating a quadratic function)
// take speed and steering value from car (need to check with Chris how to get that into packet)
// create linear regression for associating values from (-0.5 to 0.5) to degrees (-50 to 50) so multiply steering by 10 and then multiply by steering approximation
// this should generate new endpoints to run the same intersect methods from simpleCollisionPredict


// overarching function that calls the helpers on the bottom
bool simpleCollisionPredict(carPacket otherCar, CarApiBase::CarState thisCar, Json::Value obj, char *carName) {
    Vector3r _thisCarBegPt = thisCar.kinematics_estimated.pose.position;
    Vector3r thisCarBegPt (_thisCarBegPt.x() + obj["Vehicles"][carName]["X"].asInt(), _thisCarBegPt.y() + obj["Vehicles"][carName]["Y"].asInt(), _thisCarBegPt.z() + obj["Vehicles"][carName]["Z"].asInt());
    Vector3r otherCarBegPt (otherCar.PX, otherCar.PY, otherCar.PZ);
    Vector3r thisCarEndPt = getThisCarEndPoint(thisCar, obj, carName);
    Vector3r otherCarEndPt = getOtherCarEndPoint(otherCar);


    // std::cout << "this car begin point " << thisCarBegPt << endl;
    // std::cout << "this car end point " << thisCarEndPt << endl;
    // std::cout << "other car begin point " << otherCarBegPt << endl;
    // std::cout << "other car end point " << otherCarEndPt << endl;

    // working with thin line, give the line some thickness by adding more line segments
    // does not account for the back half of the car or forward half depending on speed.

    for (float increment = -1.5; increment < 2.0; increment += 0.5) {
      Vector3r thisChange = thisPerpendicularPtTranslation(thisCar, increment);
      Vector3r otherChange = otherPerpendicularPtTranslation(otherCar, increment);
      Vector3r newTCarBegPt (thisCarBegPt.x() + thisChange.x(), thisCarBegPt.y() + thisChange.y(), thisCarBegPt.z());
      Vector3r newOCarBegPt (otherCarBegPt.x() + otherChange.x(), otherCarBegPt.y() + otherChange.y(), otherCarBegPt.z());
      Vector3r newTCarEndPt (thisCarEndPt.x() + thisChange.x(), thisCarEndPt.y() + thisChange.y(), thisCarEndPt.z());
      Vector3r newOCarEndPt (otherCarEndPt.x() + otherChange.x(), otherCarEndPt.y() + otherChange.y(), otherCarEndPt.z());
      if (doIntersect(newTCarBegPt, newTCarEndPt, newOCarBegPt, newOCarEndPt)) {
        return true;
      }
    }

    return false;

}


// accepts an array containing spots for 6 points: first two points is line segment of left-most side of car, second two points
// is line segment of middle of the car, last two points is right-most side of car. (assuming car is facing forward when reference left and right)
void otherCarCoverage(Vector3r *vectArr, carPacket otherCar) {
    int counter = 0;
    Vector3r otherCarBegPt (otherCar.PX, otherCar.PY, otherCar.PZ);
    Vector3r translateFor = otherParallelPtTranslation(otherCar, 2.5);
    Vector3r translateBack = otherParallelPtTranslation(otherCar, -2.5);

    for (float width = -1.5; width < 2.0; width += 1.5) {
        Vector3r widthChange = otherPerpendicularPtTranslation(otherCar, width);
        vectArr[counter++] = otherCarBegPt + widthChange + translateFor;
        vectArr[counter++] = otherCarBegPt + widthChange + translateBack;
    }
}


// calculates the position of the other car in the next 1 second (adjustable)
// assumes the car is travelling linearly, does not take into account steering angle
// assume for now one UU = 1 meter, if things go wrong forums say 1 UU = 1cm.
// DOES NOT HANDLE Z change, cause quaternions
Vector3r getOtherCarEndPoint(carPacket otherCar) {

    float inputTime = 1;
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

    if (otherCar.Speed < 0) {
      yDelta = yDelta * -1;
      xDelta = xDelta * -1;
    }

    Vector3r retVector(otherCar.PX + xDelta, otherCar.PY + yDelta, otherCar.PZ);
    return retVector;
}


// calculates delta if a given vector were translated perpendicular from where the car is facing
// positive increments correspond to clockwise of where the car is facing
Vector3r otherPerpendicularPtTranslation(carPacket otherCar, float increment) {

    Quaternionr otherCarQuat(otherCar.OW, otherCar.OX, otherCar.OY, otherCar.OZ);
    float otherCarRad = getZAngleFromQuat(otherCarQuat);
    float newCarRad = rotate90(otherCarRad, increment);

    float xDelta = increment * cos(newCarRad);
    if (abs(newCarRad) > (M_PI / 2)) {
      xDelta = abs(xDelta) * -1;
    }
    else {
      xDelta = abs(xDelta);
    }

    float yDelta = increment * sin(newCarRad);
    if (newCarRad < 0) {
      yDelta = abs(yDelta) * -1;
    }
    else {
      yDelta = abs(yDelta);
    }

    Vector3r retVector(xDelta, yDelta, 0);
    return retVector;
}


// calculates delta if a given vector were translated parallel from where the car is facing
Vector3r otherParallelPtTranslation(carPacket otherCar, float increment) {

    Quaternionr otherCarQuat(otherCar.OW, otherCar.OX, otherCar.OY, otherCar.OZ);
    float otherCarRad = getZAngleFromQuat(otherCarQuat);
    float newCarRad = otherCarRad;

    if (increment < 0) {
      newCarRad = rotate180(otherCarRad, increment);
    }

    float xDelta = increment * cos(newCarRad);
    if (abs(newCarRad) > (M_PI / 2)) {
      xDelta = abs(xDelta) * -1;
    }
    else {
      xDelta = abs(xDelta);
    }

    float yDelta = increment * sin(newCarRad);
    if (newCarRad < 0) {
      yDelta = abs(yDelta) * -1;
    }
    else {
      yDelta = abs(yDelta);
    }

    Vector3r retVector(xDelta, yDelta, 0);
    return retVector;
}


// accepts an array containing spots for 6 points: first two points is line segment of left-most side of car, second two points
// is line segment of middle of the car, last two points is right-most side of car. (assuming car is facing forward when reference left and right)
void thisCarCoverage(Vector3r *vectArr, CarApiBase::CarState thisCar, Json::Value obj, char *carName) {

    int counter = 0;
    Vector3r _thisCarVector = thisCar.kinematics_estimated.pose.position;
    Vector3r thisCarBegPt (_thisCarVector.x() + obj["Vehicles"][carName]["X"].asInt(), _thisCarVector.y() + obj["Vehicles"][carName]["Y"].asInt(), _thisCarVector.z() + obj["Vehicles"][carName]["Z"].asInt());
    Vector3r translateFor = thisParallelPtTranslation(thisCar, 2.5);
    Vector3r translateBack = thisParallelPtTranslation(thisCar, -2.5);

    for (float width = -1.5; width < 2.0; width += 1.5) {
        Vector3r widthChange = thisPerpendicularPtTranslation(thisCar, width);
        vectArr[counter++] = thisCarBegPt + widthChange + translateFor;
        vectArr[counter++] = thisCarBegPt + widthChange + translateBack;
    }
}


// calculates the postion of this car will be in the next 1 second (adjustable)
// assumes the car is travelling linearly, does not take into account steering angle
// assume for now one UU = 1 meter, if things go wrong forums say 1 UU = 1cm.
// DOES NOT HANDLE Z change, cause quaternions
Vector3r getThisCarEndPoint(CarApiBase::CarState thisCar, Json::Value obj, char *carName) {

    float inputTime = 1;
    float hypotenuse = thisCar.speed * inputTime;

    // orientation of Z angle from rotation utils
    Quaternionr thisCarQuat = thisCar.kinematics_estimated.pose.orientation;
    float thisCarRadian = getZAngleFromQuat(thisCarQuat);
    Vector3r _thisCarVector = thisCar.kinematics_estimated.pose.position;
    Vector3r thisCarVector (_thisCarVector.x() + obj["Vehicles"][carName]["X"].asInt(), _thisCarVector.y() + obj["Vehicles"][carName]["Y"].asInt(), _thisCarVector.z() + obj["Vehicles"][carName]["Z"].asInt());


    float xDelta = hypotenuse * cos(thisCarRadian);
    if (abs(thisCarRadian) > (M_PI / 2)) {
      xDelta = abs(xDelta) * -1;
    }
    else {
      xDelta = abs(xDelta);
    }

    float yDelta = hypotenuse * sin(thisCarRadian);
    if (thisCarRadian < 0) {
      yDelta = abs(yDelta) * -1;
    }
    else {
      yDelta = abs(yDelta);
    }

    if (thisCar.speed < 0) {
      yDelta = yDelta * -1;
      xDelta = xDelta * -1;
    }

    Vector3r retVector(thisCarVector.x() +xDelta, thisCarVector.y() + yDelta, thisCarVector.z());
    return retVector;
}


// calculates the delta if a given vector were translated perpendicular from where the car is facing
// positive increments correspond to clockwise of where the car is facing
Vector3r thisPerpendicularPtTranslation(CarApiBase::CarState thisCar, float increment) {

    float carRad = getZAngleFromQuat(thisCar.kinematics_estimated.pose.orientation);
    float newCarRad = rotate90(carRad, increment);

    float xDelta = increment * cos(newCarRad);
    if (abs(newCarRad) > (M_PI / 2)) {
      xDelta = abs(xDelta) * -1;
    } else {
      xDelta = abs(xDelta);
    }

    float yDelta = increment * sin(newCarRad);
    if (newCarRad < 0) {
      yDelta = abs(yDelta) * -1;
    } else {
      yDelta = abs(yDelta);
    }

    Vector3r retVector(xDelta, yDelta, 0);
    return retVector;
}


// calculates the delta if a given vector were translated parallel from where the car is facing
Vector3r thisParallelPtTranslation(CarApiBase::CarState thisCar, float increment) {

    float carRad = getZAngleFromQuat(thisCar.kinematics_estimated.pose.orientation);
    float newCarRad = carRad;
    if (increment < 0) {
      newCarRad = rotate180(carRad, increment);
    }

    float xDelta = increment * cos(newCarRad);
    if (abs(newCarRad) > (M_PI / 2)) {
      xDelta = abs(xDelta) * -1;
    } else {
      xDelta = abs(xDelta);
    }

    float yDelta = increment * sin(newCarRad);
    if (newCarRad < 0) {
      yDelta = abs(yDelta) * -1;
    } else {
      yDelta = abs(yDelta);
    }

    Vector3r retVector(xDelta, yDelta, 0);
    return retVector;
}


// given a float of the speed, a steering coefficient is returned for the degree rotation
// this is an approximation based on the diagram in unreal of the steering curve
// approximation is made using rise over run between points in diagram
float approximateSteeringCoeff(float carSpeed) {
    if (abs(carSpeed) < 16.233) {
        return (1 + (-0.02430235 * carSpeed));
    }
    else if (abs(carSpeed) < 43.573) {
        return (0.6055 + (-0.1200439 * (carSpeed - 16.233)));
    }
    else if (abs(carSpeed) < 117.026) {
        return (0.27773 + (-0.00106326 * (carSpeed - 43.573)));
    } else {
        return (0.1992 + (-0.00001234 * (carSpeed - 117.026)));
    }
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
