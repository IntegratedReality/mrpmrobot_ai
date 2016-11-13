#include "AIListener.h"
#include "../common/Const.h"

#include <mutex>
#include <iostream>
using namespace std;

extern std::mutex mutex_obj;
extern int ID;

void AIListener::setup(RobotData *_data, ETeam *_owner) {
	data = _data;
	owner = _owner;
}

void AIListener::assignPosData(int _id, osc::ReceivedMessageArgumentStream& _argstr){
  double x;
  double y;
  double theta;

  _argstr >> x >> y >> theta >> osc::EndMessage;
  auto& d = data[_id];
  d.id=_id;
  d.pos.x=x;
  d.pos.y=y;
  d.pos.theta=theta;
}

void AIListener::ProcessMessage(const osc::ReceivedMessage& m, __attribute__((unused)) const IpEndpointName& remoteEndPoint) {
  try {
    if(std::strcmp(m.AddressPattern(), "/main/toAI/yourpos") == 0) {
      auto argstr = m.ArgumentStream();
      assignPosData(ID, argstr);

      std::cerr<<"/main/toAI/yourpos"<<std::endl;

    } else if (std::strcmp(m.AddressPattern(), "/main/toAI/allpos") == 0) {
      auto argstr = m.ArgumentStream();
      for(int i=0; i<NUM_OF_ROBOT; ++i){
        assignPosData(i, argstr);
      }
      std::cerr<<"/main/toAI/allpos"<<std::endl;

    } else if (std::strcmp(m.AddressPattern(), "/main/toAI/POowner") == 0) {
      osc::ReceivedMessageArgumentStream args = m.ArgumentStream();

      for(int i=0; i<NUM_OF_POINT_OBJ; ++i){
        osc::int32 team;
        args >> team >> osc::EndMessage;
        owner[i] = (ETeam)team;
      }

      std::cerr<<"/main/toAI/POowner"<<std::endl;
    }
  } catch(osc::Exception& e) {
		std::cout << "error while parsing message :."
			<< m.AddressPattern() << ": " << e.what() << "\n";
	}
	mutex_obj.lock();
	m_message_received = true;
	mutex_obj.unlock();
}

bool AIListener::checkMessageReceived(void) {
	mutex_obj.lock();
	//cout << "Message:lock" << endl;
	if (m_message_received) {
		m_message_received = false;
		mutex_obj.unlock();
		//cout << "Message:unlock" << endl;
		return true;
	} else {
		mutex_obj.unlock();
		//cout << "Message:unlock" << endl;
		return false;
	}
}
