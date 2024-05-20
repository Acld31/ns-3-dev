#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/on-off-helper.h"
#include "ns3/simple-net-device-helper.h"
#include "ns3/simple-channel.h"
#include "ns3/traffic-control-helper.h"
#include "ns3/traffic-control-layer.h"
#include "ns3/inet-socket-address.h"
#include "ns3/drop-tail-queue.h"

/*
*
* Exemple d'une application TCP avec un serveur qui écoute sur le port 5000 d'adresse IP 192.168.0.1 et un client d'IP 192.168.0.2 qui génère
* du trafic CBR de 500 bytes/s à destination de 192.168.0.1 sur le port 5000 
*/

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("exampleDavid");

int main(int argc, char* argv[])
{
    Time::SetResolution(Time::NS);
    LogComponentEnable("OnOffApplication", LOG_LEVEL_INFO);
    LogComponentEnable("PacketSink", LOG_LEVEL_INFO);
    //LogComponentEnableAll(LOG_LEVEL_INFO);

    NodeContainer sender;
    NodeContainer receiver;
    NodeContainer routers;
    sender.Create(1);
    receiver.Create(1);
    routers.Create(3);


    PointToPointHelper edgeLink;
    edgeLink.SetDeviceAttribute("DataRate", StringValue("1000Mbps"));
    edgeLink.SetChannelAttribute("Delay", StringValue("5ms"));

    // Create NetDevice containers
    NetDeviceContainer senderEdgeR1 = edgeLink.Install(sender.Get(0), routers.Get(0));
    NetDeviceContainer senderEdgeR3 = edgeLink.Install(sender.Get(0), routers.Get(2));
    NetDeviceContainer r1r2 = edgeLink.Install(routers.Get(0), routers.Get(1));
    NetDeviceContainer receiverEdgeR2 = edgeLink.Install(routers.Get(1), receiver.Get(0));
    NetDeviceContainer receiverEdgeR3 = edgeLink.Install(routers.Get(2), receiver.Get(0));

    // Install Stack
    InternetStackHelper internet;
    internet.Install(sender);
    internet.Install(receiver);
    internet.Install(routers);


    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.0.0.0", "255.255.255.0");

    Ipv4InterfaceContainer i1i2 = ipv4.Assign(r1r2);

    ipv4.NewNetwork();
    Ipv4InterfaceContainer is1 = ipv4.Assign(senderEdgeR1);

    ipv4.NewNetwork();
    Ipv4InterfaceContainer is3 = ipv4.Assign(senderEdgeR3);

    ipv4.NewNetwork();
    Ipv4InterfaceContainer ir3 = ipv4.Assign(receiverEdgeR3);

    ipv4.NewNetwork();
    Ipv4InterfaceContainer ir2 = ipv4.Assign(receiverEdgeR2);


    // Populate routing tables
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    /*
    * On gère l'application avec noeud0 comme serveur et noeud1 comme client
    *
    */
    OnOffHelper ooh("ns3::TcpSocketFactory", InetSocketAddress(InetSocketAddress(ir2.GetAddress(1), 5000)));
    ooh.SetConstantRate(DataRate("2KBps"), 1000);
    ooh.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=10.0]"));
    ooh.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=1.0]"));
    ooh.SetAttribute("StartTime", TimeValue(Seconds(1.0)));
    ooh.SetAttribute("StopTime", TimeValue(Seconds(20.0)));
    ApplicationContainer client = ooh.Install(sender.Get(0));
    client.Start(Seconds(1.0));
    client.Stop(Seconds(20.0));

    PacketSinkHelper psh("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(),5000));
    psh.SetAttribute("StartTime", TimeValue(Seconds(0.0)));
    psh.SetAttribute("StopTime", TimeValue(Seconds(20.0)));
    ApplicationContainer serveur = psh.Install(receiver.Get(0));
    serveur.Start(Seconds(0.0));
    serveur.Stop(Seconds(0.0));
    
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
