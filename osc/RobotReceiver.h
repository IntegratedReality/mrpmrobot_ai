#pragma once 

#include <iostream>
#include <string>
#include <osc/OscReceiveElements>
#include <osc/OscPacketListener>
#include <ip/UdpSocket>

#include "../common/RobotData.h"
#include "../common/Const.h"

class RobotReceiver {
public:
	void init()
private:
	RobotData data[NUM_OF_ROBOT];
};
