#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <oscpack/osc/OscTypes.h>
#include <oscpack/osc/OscOutboundPacketStream.h>
#include <oscpack/ip/UdpSocket.h>

#define OUTPUT_BUFFER_SIZE 1024

class AISender{
  public:
    AISender();
    //void setup(std::string& host, int port);
    void setup(
        const std::string& robotHost, int robotPort,
        const std::string& mainHost, int mainPort);
    //void sendPos(int x, int y, int z);
    void sendShot(int _id, bool _shot);
    void sendOperation(int _operation);

  private:
    std::string robotHost, mainHost;
    int robotPort, mainPort;
    //int portname;
    std::unique_ptr<UdpTransmitSocket>
      robotSock, mainSock;
};
