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

bool simpleCollisionPredict(carPacket otherCar, CarApiBase::CarState thisCar, Json::Value obj, char *carName);
// function below is not tested and not used (yet)
void otherCarCoverage(Vector3r *vectArr, carPacket otherCar);
Vector3r getOtherCarEndPoint(carPacket otherCar);
Vector3r otherPerpendicularPtTranslation(carPacket otherCar, float increment);
// function below is not tested and not used (yet)
Vector3r otherParallelPtTranslation(carPacket otherCar, float increment);
// function below is not tested and not used (yet)
void thisCarCoverage(Vector3r *vectArr, CarApiBase::CarState thisCar, Json::Value obj, char *carName);
Vector3r getThisCarEndPoint(CarApiBase::CarState thisCar, Json::Value obj, char *carName);
Vector3r thisPerpendicularPtTranslation(CarApiBase::CarState thisCar, float increment);
// function below is not testedand not used (yet)
Vector3r thisParallelPtTranslation(CarApiBase::CarState thisCar, float increment);
// not reviewed
float approximateSteeringCoeff(float carSpeed);
bool onSegment(Vector3r p, Vector3r q, Vector3r r);
int orientation(Vector3r p, Vector3r q, Vector3r r);
bool doIntersect(Vector3r tCarBeg, Vector3r tCarEnd, Vector3r oCarBeg, Vector3r oCarEnd);

#ifdef _cplusplus
}
#endif

#endif
