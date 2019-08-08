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
#include <ctime>
// #include "Quat.h"

#include "asyncSocketClient.h"
#include "packet.h"
#include <Eigen/Geometry>

// these need to match the settings.json file
// TODO: Read settings.json so we don't have to hardcode values
#define STARTX1 0
#define STARTY1 0
#define STARTZ1 0

#define STARTX2 4
#define STARTY2 4
#define STARTZ2 0

using namespace std;
using namespace cPkt;
using namespace msr::airlib;

// So I found code online to help determine the angle between two vectors, we may have some dependency issues but I think it is worth checking out.
// I changed a bit of the code to see if I could adapt it to what we have, this is untested but it compiles so idk.
// It requires us to download OpenGL.
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

Vector3r *getCollisionVector(Vector3r &thisCarPos, Vector3r &otherCarPos) {
    Vector3r _collisionVector = (otherCarPos - thisCarPos);
    Vector3r *collisionVector = new Vector3r (_collisionVector);
    return collisionVector;
}
// 1.32579632679
// 1.81579632679

int main(int argc, char const *argv[]) {

  Vector3r car1Pos (STARTX1, STARTY1, STARTZ1);
  Quaternionr car1Quat (1, 0, 0, 0);
  cout << car1Quat.w() << endl;
  Vector3r car2Pos (atoi(argv[1]), atoi(argv[2]), STARTZ2);
  // Vector3r car2Pos (STARTX2, STARTY2, STARTZ2);

  // Vector3r car1Pos (STARTY1, STARTX1, STARTZ1);
  // Quaternionr car1Quat (1, 0, 0, 0);
  // Vector3r car2Pos (STARTY2, STARTX2, STARTZ2);

  Vector3r *collisionVecx = getCollisionVector(car1Pos, car2Pos);
  cout << "CollisionVector: " << *collisionVecx << endl;

  Vector3r collisionVecz (0, 0, 1);

  Quaternionr collisionQuat = Eigen::Quaternion<float>::FromTwoVectors (*collisionVecx, collisionVecz);
  Quaternionr transformQuat = collisionQuat * car1Quat.inverse();
  // struct FQuat transformFQuat (transformQuat.w(), transformQuat.x(), transformQuat.y(), transformQuat.z());

  cout << "turnAngles: " << endl;

  // there seems to be the option of 4+ for eulerAngles. So it allows for input larger than 3,but I am unsure of what that does.
  //Vector3r turntest = transformQuat.toRotationMatrix().eulerAngles(6, 6, 6);
  //cout << "turntest: " << turntest << endl;

  Vector3r turnAngle1 = transformQuat.toRotationMatrix().eulerAngles(0, 0, 0);
  cout << "turnAngle1: " << turnAngle1 << endl;
  Vector3r turnAngle2 = transformQuat.toRotationMatrix().eulerAngles(0, 0, 1);
  cout << "turnAngle2: " << turnAngle2 << endl;
  Vector3r turnAngle3 = transformQuat.toRotationMatrix().eulerAngles(0, 0, 2);
  cout << "turnAngle3: " << turnAngle3 << endl;
  Vector3r turnAngle4 = transformQuat.toRotationMatrix().eulerAngles(0, 1, 0);
  cout << "turnAngle4: " << turnAngle4 << endl;
  Vector3r turnAngle5 = transformQuat.toRotationMatrix().eulerAngles(0, 1, 1);
  cout << "turnAngle5: " << turnAngle5 << endl;
  Vector3r turnAngle6 = transformQuat.toRotationMatrix().eulerAngles(0, 1, 2);
  cout << "turnAngle6: " << turnAngle6 << endl;
  Vector3r turnAngle7 = transformQuat.toRotationMatrix().eulerAngles(0, 2, 0);
  cout << "turnAngle7: " << turnAngle7 << endl;
  Vector3r turnAngle8 = transformQuat.toRotationMatrix().eulerAngles(0, 2, 1);
  cout << "turnAngle8: " << turnAngle8 << endl;
  Vector3r turnAngle9 = transformQuat.toRotationMatrix().eulerAngles(0, 2, 2);
  cout << "turnAngle9: " << turnAngle9 << endl;

  Vector3r turnAngle21 = transformQuat.toRotationMatrix().eulerAngles(1, 0, 0);
  cout << "turnAngle21: " << turnAngle21 << endl;
  Vector3r turnAngle22 = transformQuat.toRotationMatrix().eulerAngles(1, 0, 1);
  cout << "turnAngle22: " << turnAngle22 << endl;
  Vector3r turnAngle23 = transformQuat.toRotationMatrix().eulerAngles(1, 0, 2);
  cout << "turnAngle23: " << turnAngle23 << endl;
  Vector3r turnAngle24 = transformQuat.toRotationMatrix().eulerAngles(1, 1, 0);
  cout << "turnAngle24: " << turnAngle24 << endl;
  Vector3r turnAngle25 = transformQuat.toRotationMatrix().eulerAngles(1, 1, 1);
  cout << "turnAngle25: " << turnAngle25 << endl;
  Vector3r turnAngle26 = transformQuat.toRotationMatrix().eulerAngles(1, 1, 2);
  cout << "turnAngle26: " << turnAngle26 << endl;
  Vector3r turnAngle27 = transformQuat.toRotationMatrix().eulerAngles(1, 2, 0);
  cout << "turnAngle27: " << turnAngle27 << endl;
  Vector3r turnAngle28 = transformQuat.toRotationMatrix().eulerAngles(1, 2, 1);
  cout << "turnAngle28: " << turnAngle28 << endl;
  Vector3r turnAngle29 = transformQuat.toRotationMatrix().eulerAngles(1, 2, 2);
  cout << "turnAngle29: " << turnAngle29 << endl;

  Vector3r turnAngle31 = transformQuat.toRotationMatrix().eulerAngles(2, 0, 0);
  cout << "turnAngle31: " << turnAngle31 << endl;
  Vector3r turnAngle32 = transformQuat.toRotationMatrix().eulerAngles(2, 0, 1);
  cout << "turnAngle32: " << turnAngle32 << endl;
  Vector3r turnAngle33 = transformQuat.toRotationMatrix().eulerAngles(2, 0, 2);
  cout << "turnAngle33: " << turnAngle33 << endl;
  Vector3r turnAngle34 = transformQuat.toRotationMatrix().eulerAngles(2, 1, 0);
  cout << "turnAngle34: " << turnAngle34 << endl;
  Vector3r turnAngle35 = transformQuat.toRotationMatrix().eulerAngles(2, 1, 1);
  cout << "turnAngle35: " << turnAngle35 << endl;
  Vector3r turnAngle36 = transformQuat.toRotationMatrix().eulerAngles(2, 1, 2);
  cout << "turnAngle36: " << turnAngle36 << endl;
  Vector3r turnAngle37 = transformQuat.toRotationMatrix().eulerAngles(2, 2, 0);
  cout << "turnAngle37: " << turnAngle37 << endl;
  Vector3r turnAngle38 = transformQuat.toRotationMatrix().eulerAngles(2, 2, 1);
  cout << "turnAngle38: " << turnAngle38 << endl;
  Vector3r turnAngle39 = transformQuat.toRotationMatrix().eulerAngles(2, 2, 2);
  cout << "turnAngle39: " << turnAngle39 << endl;


  cout << "turnAngles.z: " << endl;

  // Vector3r turnAngles = transformQuat.toRotationMatrix();
  // cout << "turnAngle.z(): " << turnAngles.z() << endl;
  Vector3r turnAngles1 = transformQuat.toRotationMatrix().eulerAngles(0, 0, 0);
  cout << "turnAngle1.z(): " << turnAngles1.z() << endl;
  Vector3r turnAngles2 = transformQuat.toRotationMatrix().eulerAngles(0, 0, 1);
  cout << "turnAngle2.z(): " << turnAngles2.z() << endl;
  Vector3r turnAngles3 = transformQuat.toRotationMatrix().eulerAngles(0, 0, 2);
  cout << "turnAngle3.z(): " << turnAngles3.z() << endl;
  Vector3r turnAngles4 = transformQuat.toRotationMatrix().eulerAngles(0, 1, 0);
  cout << "turnAngle4.z(): " << turnAngles4.z() << endl;
  Vector3r turnAngles5 = transformQuat.toRotationMatrix().eulerAngles(0, 1, 1);
  cout << "turnAngle5.z(): " << turnAngles5.z() << endl;
  Vector3r turnAngles6 = transformQuat.toRotationMatrix().eulerAngles(0, 1, 2);
  cout << "turnAngle6.z(): " << turnAngles6.z() << endl;
  Vector3r turnAngles7 = transformQuat.toRotationMatrix().eulerAngles(0, 2, 0);
  cout << "turnAngle7.z(): " << turnAngles7.z() << endl;
  Vector3r turnAngles8 = transformQuat.toRotationMatrix().eulerAngles(0, 2, 1);
  cout << "turnAngle8.z(): " << turnAngles8.z() << endl;
  Vector3r turnAngles9 = transformQuat.toRotationMatrix().eulerAngles(0, 2, 2);
  cout << "turnAngle9.z(): " << turnAngles9.z() << endl;

  Vector3r turnAngles21 = transformQuat.toRotationMatrix().eulerAngles(1, 0, 0);
  cout << "turnAngle21.z(): " << turnAngles21.z() << endl;
  Vector3r turnAngles22 = transformQuat.toRotationMatrix().eulerAngles(1, 0, 1);
  cout << "turnAngle22.z(): " << turnAngles22.z() << endl;
  Vector3r turnAngles23 = transformQuat.toRotationMatrix().eulerAngles(1, 0, 2);
  cout << "turnAngle23.z(): " << turnAngles23.z() << endl;
  Vector3r turnAngles24 = transformQuat.toRotationMatrix().eulerAngles(1, 1, 0);
  cout << "turnAngle24.z(): " << turnAngles24.z() << endl;
  Vector3r turnAngles25 = transformQuat.toRotationMatrix().eulerAngles(1, 1, 1);
  cout << "turnAngle25.z(): " << turnAngles25.z() << endl;
  Vector3r turnAngles26 = transformQuat.toRotationMatrix().eulerAngles(1, 1, 2);
  cout << "turnAngle26.z(): " << turnAngles26.z() << endl;
  Vector3r turnAngles27 = transformQuat.toRotationMatrix().eulerAngles(1, 2, 0);
  cout << "turnAngle27.z(): " << turnAngles27.z() << endl;
  Vector3r turnAngles28 = transformQuat.toRotationMatrix().eulerAngles(1, 2, 1);
  cout << "turnAngle28.z(): " << turnAngles28.z() << endl;
  Vector3r turnAngles29 = transformQuat.toRotationMatrix().eulerAngles(1, 2, 2);
  cout << "turnAngle29.z(): " << turnAngles29.z() << endl;

  Vector3r turnAngles31 = transformQuat.toRotationMatrix().eulerAngles(2, 0, 0);
  cout << "turnAngle31.z(): " << turnAngles31.z() << endl;
  Vector3r turnAngles32 = transformQuat.toRotationMatrix().eulerAngles(2, 0, 1);
  cout << "turnAngle32.z(): " << turnAngles32.z() << endl;
  Vector3r turnAngles33 = transformQuat.toRotationMatrix().eulerAngles(2, 0, 2);
  cout << "turnAngle33.z(): " << turnAngles33.z() << endl;
  Vector3r turnAngles34 = transformQuat.toRotationMatrix().eulerAngles(2, 1, 0);
  cout << "turnAngle34.z(): " << turnAngles34.z() << endl;
  Vector3r turnAngles35 = transformQuat.toRotationMatrix().eulerAngles(2, 1, 1);
  cout << "turnAngle35.z(): " << turnAngles35.z() << endl;
  Vector3r turnAngles36 = transformQuat.toRotationMatrix().eulerAngles(2, 1, 2);
  cout << "turnAngle36.z(): " << turnAngles36.z() << endl;
  Vector3r turnAngles37 = transformQuat.toRotationMatrix().eulerAngles(2, 2, 0);
  cout << "turnAngle37.z(): " << turnAngles37.z() << endl;
  Vector3r turnAngles38 = transformQuat.toRotationMatrix().eulerAngles(2, 2, 1);
  cout << "turnAngle38.z(): " << turnAngles38.z() << endl;
  Vector3r turnAngles39 = transformQuat.toRotationMatrix().eulerAngles(2, 2, 2);
  cout << "turnAngle39.z(): " << turnAngles39.z() << endl;

  cout << transformQuat.toRotationMatrix() << endl;

  // cout << myFQuat.Euler() << endl;
  // cout << transformFQuat.Euler() << endl;

  return 0;
}
