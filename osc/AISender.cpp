#include "AISender.h"

#include <oscpack/osc/OscOutboundPacketStream.h>
#include <oscpack/ip/UdpSocket.h>

AISender::AISender()
{
}

void AISender::setup(
    const std::string& robotHost, int robotPort,
    const std::string& mainHost, int mainPort)
{
  this->robotHost = robotHost;
  this->mainHost = mainHost;
  this->robotPort = robotPort;
  this->mainPort = mainPort;

  robotSock
    =std::unique_ptr<UdpTransmitSocket>
    (new UdpTransmitSocket(IpEndpointName(
          robotHost.c_str(), robotPort)));
  mainSock
    =std::unique_ptr<UdpTransmitSocket>
    (new UdpTransmitSocket(IpEndpointName(
          mainHost.c_str(), mainPort)));
}

void AISender::sendShot(int _id, bool _shot) {
  char buffer[OUTPUT_BUFFER_SIZE];
  osc::OutboundPacketStream p(buffer, OUTPUT_BUFFER_SIZE);

  p << osc::BeginBundleImmediate
    << osc::BeginMessage("/operator/shot")
    << _id
    << _shot
    << osc::EndMessage
    << osc::EndBundle;

  mainSock->Send(p.Data(), p.Size());
}

void AISender::sendOperation(int _operation){
  char buffer[OUTPUT_BUFFER_SIZE];
  osc::OutboundPacketStream p(buffer, OUTPUT_BUFFER_SIZE);

  p << osc::BeginBundleImmediate
    << osc::BeginMessage("/operator/operation")
    << _operation
    << osc::EndMessage
    << osc::EndBundle;

  robotSock->Send(p.Data(), p.Size());
}

