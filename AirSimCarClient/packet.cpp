#include <iostream>
#include <string>
#include "packet.h"
using namespace std;
using namespace cPkt;
carPacket::carPacket(float speed, float gear, float px, float py, float pz, float ow, float ox, float oy, float oz)
	: Speed(speed), Gear(gear), PX(px), PY(py), PZ(pz), OW(ow), OX(ox), OY(oy), OZ(oz)
{}

// There is no error check for the format of the string, will segfault with improper formatting
void carPacket::setValues(string test) {
	string delim = ",";
	string strArr[9];
	float attributes[9];
	int counter = 0;
	string token;

	size_t pos = 0;
	while ((pos = test.find(delim)) != std::string::npos)
	{
		token = test.substr(0, pos);
		strArr[counter++] = token;
		test.erase(0, pos + delim.length());
	}
	strArr[counter] = test.substr(0, test.length());

	for (int index = 0; index < 9; index++)
	{
		string element = strArr[index];
		attributes[index] = stod(element.substr(element.find(":") + 1, element.length()));
	}

	// Packet(attributes[0], attributes[1], attributes[2], attributes[3], attributes[4], attributes[5], attributes[6], attributes[7], attributes[8]);
	this->Speed = attributes[0];
	this->Gear = attributes[1];
	this->PX = attributes[2];
	this->PY = attributes[3];
	this->PZ = attributes[4];
	this->OW = attributes[5];
	this->OX = attributes[6];
	this->OY = attributes[7];
	this->OZ = attributes[8];

};
