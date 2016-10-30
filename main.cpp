
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <array>
#include <thread>
#include <mutex>
#include "unistd.h"


#include "osc/AIReceiver.h"
#include "osc/AISender.h"

#include "AI/AI.h"

const std::string robotHostname = "127.0.0.1"; //localhost
const int PORT_ROBOT = 8000;

using namespace std;
std::mutex mutex_obj;

extern int ID;

AI ai;

int main(int argc, char **argv)
{
	if (argc == 2) ID = atoi(argv[1]);
	else {
		cout << "コマンドライン引数でロボットのIDを指定してください。m(__)m" << endl;
		return 0;
	}

	ai.init(ID);

	AIReceiver receiver;
	receiver.init();
	AISender sender;
	sender.setup(robotHostname, PORT_ROBOT);

	while (!receiver.checkMessageReceived());

	long count = 0;
	std::thread ai_thread([&](){
		sender.sendShot(ID, false);
		while (ID >= 3 || receiver.getData(ID).isAI) {
			static bool last_shot_state = ai.getOperation().shot;;
			if (!(receiver.getData(ID).state == DEAD || receiver.getData(ID).state == STANDBY)) {
				if (last_shot_state != ai.getOperation().shot) {
					sender.sendShot(ID, ai.getOperation().shot);
				}
			}
			last_shot_state = ai.getOperation().shot;
			for (int i = 0; i < 6; i++) {
			ai.setRobotData(i, receiver.getData(i));
			}
			for (int i = 0; i < 3; i++) {
			ai.setPOOwner(i, receiver.getPOOwner(i));
			}
			ai.update();
		}
	});
	ai_thread.detach();

	while (1) {
		RobotData data;
		data = receiver.getData(ID);

		double max_v = 0.1, max_omega = 0.001;
		double v = 0, omega = 0;

		// オーバードライブ防止処理
		bool F, B;
		checkMovable(data.pos, F, B);

		bool MoveSURUNO = true;

        double ofsetRotate = 2.7;
        double ofsetMoving = 1.0;
		switch (ID < 3 && !data.isAI ? data.operation.direction : ai.getOperation().direction) {
			case NO_INPUT:
				break;
			case TOP:
				v = (double)F * max_v;
				MoveSURUNO = F;
				break;
			case TOP_RIGHT:
				v = (double)F * max_v;
				omega = -ofsetMoving * max_omega;
				MoveSURUNO = F;
				break;
			case RIGHT:
				omega = - ofsetRotate * max_omega;
				break;
			case BOTTOM_RIGHT:
				v = -((double)B * max_v);
				omega = ofsetMoving * max_omega;
				MoveSURUNO = B;
				break;
			case BOTTOM:
				v = -((double)B * max_v);
				MoveSURUNO = B;
				break;
			case BOTTOM_LEFT:
				v = -((double)B * max_v);
				omega = -ofsetMoving * max_omega;
				MoveSURUNO = B;
				break;
			case LEFT:
				omega = ofsetRotate * max_omega;
				break;
			case TOP_LEFT:
				v = (double)F * max_v;
				omega = ofsetMoving * max_omega;
				MoveSURUNO = F;
				break;
		}

		bool active = true;
		if (data.state == DEAD || data.state == STANDBY) active = false;

		mutex_obj.lock();
		drive.setTarget((int)active * v, (int)active * omega, (MoveSURUNO ? MotorMode::Move : MotorMode::Brake));
		mutex_obj.unlock();
		// ここまでを弄る

		if (count != 3000) {
			count++;
			continue;
		}
		//	cout << F << B << endl;
		cout << "v: " << v << " ";
		cout << "omega: " << omega << " ";
		cout << "x: " << data.pos.x << " ";
		cout << "y: " << data.pos.y << " ";
		cout << "theta: " << data.pos.theta << " ";
		cout << "time: " << data.time << " ";
		cout << "isAI: " << data.isAI << " ";
		cout << "state: " << data.state << " ";
		cout << "drc: " << ((ID < 3 && !data.isAI) ? data.operation.direction : ai.getOperation().direction) << " ";
		cout << "shot: " << ((ID < 3 && !data.isAI) ? data.operation.shot : ai.getOperation().shot) << " ";
		cout << endl;
		count = 0;

		usleep(10000);
	}
	return 0;
}
