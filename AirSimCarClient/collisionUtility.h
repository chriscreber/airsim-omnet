#ifndef COLLISION_UTILITY_H
#define COLLISION_UTILITY_H

#include <iostream>
#include <utility>
#include <cstdlib>
#include <math.h>

#include "rotationUtility.h"
#include "packet.h"

using namespace std;
using namespace cPkt;
using namespace msr::airlib;

#ifdef _cplusplus
extern "C" {
#endif

bool simpleCollisionDetect(carPacket otherCar, CarApiBase::CarState thisCar);
Vector3r getOtherCarEndPoint(carPacket otherCar);
Vector3r getThisCarEndPoint(CarApiBase::CarState thisCar);
int orientation(Vector3r p, Vector3r q, Vector3r r);
bool onSegment(Vector3r p, Vector3r q, Vector3r r);
bool doIntersect(Vector3r tCarBeg, Vector3r tCarEnd, Vector3r oCarBeg, Vector3r oCarEnd);

#ifdef _cplusplus
}
#endif

#endif
