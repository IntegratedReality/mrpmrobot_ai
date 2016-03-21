#include "RobotSender.h"

#include "osc/OscOutboundPacketStream.h"
#include "ip/UdpSocket.h"

OscSender::OscSender():portname(0),socket(NULL)
{
}

void OscSender::setup(std::string host, int port)
{
    socket = new UdpTransmitSocket(IpEndpointName(host.c_str(), port));
    
    this->hostname = host;
    this->portname = port;
}

void OscSender::sendPos(int x, int y, int z)
{
    char buffer[OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream p(buffer, OUTPUT_BUFFER_SIZE);

    p << osc::BeginBundleImmediate
        << osc::BeginMessage("/robot/pos")
        << x << y << z
        << osc::EndMessage
        << osc::EndBundle;

    socket->Send(p.Data(), p.Size());
}

