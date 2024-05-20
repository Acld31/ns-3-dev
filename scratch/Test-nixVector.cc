#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv6-list-routing-helper.h"
#include "ns3/ipv6-static-routing-helper.h"
#include "ns3/network-module.h"
#include "ns3/nix-vector-helper.h"
#include "ns3/point-to-point-module.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE("NixSimpleExample");

int
main(int argc, char* argv[])
{
    LogComponentEnable("OnOffApplication", LOG_LEVEL_INFO);
    LogComponentEnable("PacketSink", LOG_LEVEL_INFO);
    //LogComponentEnableAll(LOG_LEVEL_INFO);

    NodeContainer nodes;
    nodes.Create(4);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer devices01;
    NetDeviceContainer devices12;
    NetDeviceContainer devices23;
    NetDeviceContainer devices02;
    devices01 = pointToPoint.Install(NodeContainer(nodes.Get(0), nodes.Get(1)));
    devices12 = pointToPoint.Install(NodeContainer(nodes.Get(1), nodes.Get(2)));
    devices23 = pointToPoint.Install(NodeContainer(nodes.Get(2), nodes.Get(3)));
    devices02 = pointToPoint.Install(NodeContainer(nodes.Get(0), nodes.Get(2)));


    Ipv4NixVectorHelper nixRouting;
    InternetStackHelper stack;
    stack.SetRoutingHelper(nixRouting);
    stack.Install(nodes);

    Ipv4AddressHelper address1;
    address1.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4AddressHelper address2;
    address2.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4AddressHelper address3;
    address3.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4AddressHelper address4;
    address4.SetBase("10.1.4.0", "255.255.255.0");

    Ipv4InterfaceContainer interfaces01 = address1.Assign(devices01);
    Ipv4InterfaceContainer interfaces12 = address2.Assign(devices12);
    Ipv4InterfaceContainer interfaces23 = address3.Assign(devices23);
    Ipv4InterfaceContainer interfaces02 = address4.Assign(devices02);

    // Trace routing paths for different source and destinations.
    Ptr<OutputStreamWrapper> routingStream =
        Create<OutputStreamWrapper>("nix-simple-ipv4.routes", std::ios::out);
    nixRouting.PrintRoutingPathAt(Seconds(3),
                                  nodes.Get(0),
                                  interfaces23.GetAddress(1),
                                  routingStream);
    nixRouting.PrintRoutingPathAt(Seconds(5),
                                  nodes.Get(1),
                                  interfaces23.GetAddress(1),
                                  routingStream);
    nixRouting.PrintRoutingPathAt(Seconds(6),
                                  nodes.Get(2),
                                  interfaces01.GetAddress(0),
                                  routingStream);
    nixRouting.PrintRoutingPathAt(Seconds(7),
                                  nodes.Get(1),
                                  interfaces01.GetAddress(1),
                                  routingStream);
    
    // Trace routing tables
    Ptr<OutputStreamWrapper> routingStream2 = Create<OutputStreamWrapper>(&std::cout);
    Ipv4GlobalRoutingHelper::PrintRoutingTableAllAt(Seconds(10.0), routingStream2);
   


    NS_LOG_INFO("Create Applications.");
    uint16_t port = 9; // Discard port (RFC 863)

    OnOffHelper onoff("ns3::TcpSocketFactory",
                      Address(InetSocketAddress(Ipv4Address("10.1.3.2"), port)));
    onoff.SetConstantRate(DataRate("50kb/s"));

    ApplicationContainer app = onoff.Install(nodes.Get(0));
    // Start the application
    app.Start(Seconds(1.0));
    app.Stop(Seconds(10.0));

    // Create an optional packet sink to receive these packets
    PacketSinkHelper sink("ns3::TcpSocketFactory",
                          Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    ApplicationContainer sink1 = sink.Install(nodes.Get(3));
    sink1.Start(Seconds(1.0));
    sink1.Stop(Seconds(10.0));

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
