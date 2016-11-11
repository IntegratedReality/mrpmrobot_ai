
#include <iostream>
#include <cstdlib> //for atoi
#include <thread>
#include <mutex>
#include "unistd.h"


#include "osc/AIReceiver.h"
#include "osc/AISender.h"

#include "AI/AI.h"

const std::string robotHostname = "127.0.0.1"; //localhost
const int PORT_ROBOT = 8000;
const std::string mainHostname = "mainserver.local";
const int PORT_MAINRCV = 8000;

using namespace std;
std::mutex mutex_obj;

extern int ID;

AI ai;

int main(int argc, char **argv)
{
  if (argc == 2) ID = atoi(argv[1]);
  else {
    cout << "Execute with RobotID!\n" << endl;
    return 0;
  }

  ai.init(ID);

  AIReceiver receiver;
  receiver.init();

  AISender sender;
  sender.setup(
      robotHostname, PORT_ROBOT,
      mainHostname, PORT_MAINRCV
      );

  while (!receiver.checkMessageReceived());

  std::thread ai_thread([&](){
      sender.sendShot(ID, false);
      //while (ID >= 3 || receiver.getData(ID).isAI) {
      while (true) {  //これはAI用プログラムなので
      static bool last_shot_state = ai.getOperation().shot;
      //if (!(receiver.getData(ID).state == DEAD || receiver.getData(ID).state == STANDBY)) {
      if (last_shot_state != ai.getOperation().shot) {
      //ONかOFFの瞬間だけ送る
      sender.sendShot(ID, ai.getOperation().shot);
      }
      //}
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


  //AIプロセス メインループ
  while (1) {
    sender.sendOperation(static_cast<int>(ai.getOperation().direction));

    //Operation送信間隔を決める
    usleep(10000);
  }
  return 0;
  }
