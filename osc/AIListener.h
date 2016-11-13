#pragma once

#include <iostream>
#include <cstring>
#include <oscpack/osc/OscReceivedElements.h>
#include <oscpack/osc/OscPacketListener.h>
#include <oscpack/ip/UdpSocket.h>

#include "../common/RobotData.h"
#include "../common/Const.h"

class AIListener : public osc::OscPacketListener {
	public:
		void setup(RobotData *_data, ETeam *_owner);
		virtual void ProcessMessage(const osc::ReceivedMessage& msg, const IpEndpointName& remoteEndPoint );
		bool checkMessageReceived(void);
	private:
		RobotData *data;
		ETeam *owner;
		bool m_message_received;

    void assignPosData(int _id, osc::ReceivedMessageArgumentStream& _argstr);
};
