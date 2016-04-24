/*
   Robot's Main ProgramA of MRPM Project
   Dept. of Precision Engneering, U-tokyo
   Seimitsu Lab, Mayfes
 */

#include <iostream>
#include <array>
#include <thread>
#include <mutex>

using namespace std;

#include "ledunit/ledunit.h"

#include "motor/robotcontrol.h"
#include "motor/drive.h"
#include "motor/params.h"

#include "osc/RobotReceiver.h"
#include "osc/RobotSender.h"

#include "wiringPi.h"

std::mutex mutex_obj;

int main(void)
{
    wiringPiSetupGpio();
    LedUnit led_unit(24);
    led_unit.on();

    RobotReceiver receiver;
    receiver.init();
    RobotSender sender;
    sender.setup("Coconuts.local", 8000);

    while (!receiver.checkMessageReceived());

    DriveClass drive(MotorClass(22, 27, false), MotorClass(17, 18, true), receiver.getData(ID).pos.x, receiver.getData(ID).pos.y, receiver.getData(ID).pos.theta, receiver.getData(ID).time);

    std::thread robot_control_thread([&](){
            while (true) {
            while (!receiver.checkMessageReceived());
            mutex_obj.lock();
            drive.updateData(receiver.getData(ID).pos.x, receiver.getData(ID).pos.y, receiver.getData(ID).pos.theta, receiver.getData(ID).time);
            drive.updateDrive();
            sender.sendShot(ID, receiver.getData(ID).operation.shot);
            mutex_obj.unlock();
            }
            });

    long count = 0;
    while (1) {
        // ここを弄るといいらしい
        RobotData data;
        data = receiver.getData(ID);

        double max_v = 0.2, max_omega = 0.005;
        double v = 0, omega = 0;
        switch (data.operation.direction) {
            case NO_INPUT:
                break;
            case TOP:
                v = max_v;
                break;
            case TOP_RIGHT:
                v = max_v;
                omega = -max_omega;
                break;
            case RIGHT:
                omega = -max_omega;
                break;
            case BOTTOM_RIGHT:
                v = -max_v;
                omega = max_omega;
                break;
            case BOTTOM:
                v = -max_v;
                break;
            case BOTTOM_LEFT:
                v = -max_v;
                omega = -max_omega;
                break;
            case LEFT:
                omega = max_omega;
                break;
            case TOP_LEFT:
                v = max_v;
                omega = max_omega;
                break;
        }
        mutex_obj.lock();
        drive.setTarget(v, omega);
        mutex_obj.unlock();
        // ここまでを弄る

        if (count != 30000000) {
            count++;
            continue;
        }
        cout << "v: " << v << " ";
        cout << "omega: " << omega << " ";
        cout << "x: " << data.pos.x << " ";
        cout << "y: " << data.pos.y << " ";
        cout << "theta: " << data.pos.theta << " ";
        cout << "time:" << data.time << " ";
        cout << "drc: " << data.operation.direction << " ";
        cout << "shot: " << data.operation.shot << " ";
        cout << endl;
        count = 0;
    }
    return 0;
}
