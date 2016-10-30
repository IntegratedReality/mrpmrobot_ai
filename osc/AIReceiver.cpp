#include "AIReceiver.h"

void AIReceiver::init() {
	for	(int i = 0; i < NUM_OF_ROBOT; i++) {
		data[i].init();
	}
	listener.setup(data, owner);
	std::thread th([this](){s.RunUntilSigInt();});
	th.detach();
}

RobotData AIReceiver::getData(int _id) {
	return data[_id];
}

ETeam AIReceiver::getPOOwner(int _id) {
	return owner[_id];
}

bool AIReceiver::checkMessageReceived(void)
{
	return listener.checkMessageReceived();
}
