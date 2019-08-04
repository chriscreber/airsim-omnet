#ifndef PACKET_H
#define PACKET_H

#include <string>
using namespace std;

// TODO: Add car ID for multicar simulations!

struct carNetPacket {
    // double timeElapsed;
    float speed;
    float gear;
    float px;
    float py;
    float pz;
    float qw;
    float qx;
    float qy;
    float qz;
    char  eof;
} __attribute__ ((packed));

// TODO: Namespace this please!
namespace cPkt {
  class carPacket {
    public:
      float Speed, Gear, PX, PY, PZ, OW, OX, OY, OZ;
    	carPacket(float Speed = 0.0, float Gear = 0.0, float PX = 0.0, float PY = 0.0, float PZ = 0.0, float OW = 0.0, float OX = 0.0, float OY = 0.0, float OZ = 0.0);
    	void setValues(string);
  };
}

#endif
