#ifndef ROTATION_UTILITY_H_
#define ROTATION_UTILITY_H_

#include "common/common_utils/StrictMode.hpp"
STRICT_MODE_OFF
#ifndef RPCLIB_MSGPACK
#define RPCLIB_MSGPACK clmdep_msgpack
#endif // !RPCLIB_MSGPACK
#include "rpc/rpc_error.h"
STRICT_MODE_ON

#include "vehicles/car/api/CarRpcLibClient.hpp"
#include "common/common_utils/FileSystem.hpp"
#include "packet.h"

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <cmath>
#include <tgmath.h>
#include <Eigen/Geometry>
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <fstream>
#include <jsoncpp/json/json.h>

using namespace std;
using namespace cPkt;
using namespace msr::airlib;

#ifdef _cplusplus
extern "C" {
#endif

#define RCVBUFSIZE 200

Vector3r *getCollisionVector(Vector3r &thisCarPos, Vector3r &otherCarPos);
float getCollisionAngle(Vector3r &collisionVecx);
float getZAngleFromQuat(Quaternionr &thisCarQuat);
float calcTurnAngle(float collisionAngle, float thisCarAngle);
float calcSteering(float turnAngle);
float completeCalcSteering(CarApiBase::CarState thisCarState, carPacket otherCar, Json::Value obj, char *carName);

#ifdef _cplusplus
}
#endif

#endif
