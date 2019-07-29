#ifndef PACKET_H
#define PACKET_H

#include <string>
using namespace std;

class Packet {
private:
	float Speed, float Gear, float PX, float PY, float PZ, float OW, float OX, float OY, float OZ;
public:
	Packet(float Speed = 0.0, float Gear = 0.0, float PX = 0.0, float PY = 0.0, float PZ = 0.0, float OW = 0.0, float OX = 0.0, float OY = 0.0, float OZ = 0.0);
	float getSpeed();
	float getGear();
	float getPX();
	float getPY();
	float getPZ();
	float getOW();
	float getOX();
	float getOY();
	float getOZ();
	void setValues(string);
};

#endif