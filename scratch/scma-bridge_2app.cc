//
//                   sender         192.168.1.1
//                     |                      
//                 -----------                
//                 | bridge1 | 
//                 -----------
// 192.168.1.2        /   \          192.168.1.3
//             router1     routeur2              
// 10.1.2.1           \   /         10.1.2.2
//                 -----------
//                 | bridge2 |
//                 -----------               
//                      |                   
//                  receiver         10.1.2.3
//

#include "ns3/applications-module.h"
#include "ns3/bridge-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE("CsmaBridgeOneHopExample");

int
main(int argc, char* argv[])
{
    Time::SetResolution(Time::NS);
    LogComponentEnable("OnOffApplication", LOG_LEVEL_INFO);
    LogComponentEnable("PacketSink", LOG_LEVEL_INFO);
    //LogComponentEnableAll(LOG_LEVEL_INFO);
    NS_LOG_INFO("Create nodes.");

    Ptr<Node> sender = CreateObject<Node>();
    Ptr<Node> routeur1 = CreateObject<Node>();
    Ptr<Node> routeur2 = CreateObject<Node>();
    Ptr<Node> receiver = CreateObject<Node>();

    Ptr<Node> bridge1 = CreateObject<Node>();
    Ptr<Node> bridge2 = CreateObject<Node>();

    NS_LOG_INFO("Build Topology");
    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", DataRateValue(5000000));
    csma.SetChannelAttribute("Delay", TimeValue(MilliSeconds(2)));

    // Create the csma links, from each terminal to the bridge
    // This will create six network devices; we'll keep track separately
    // of the devices on and off the bridge respectively, for later configuration
    NetDeviceContainer topLanDevices;
    NetDeviceContainer topBridgeDevices;

    // It is easier to iterate the nodes in C++ if we put them into a container
    NodeContainer topLan(sender, routeur1, routeur2);

    for (int i = 0; i < 3; i++)
    {
        // install a csma channel between the ith toplan node and the bridge node
        NetDeviceContainer link = csma.Install(NodeContainer(topLan.Get(i), bridge1));
        topLanDevices.Add(link.Get(0));
        topBridgeDevices.Add(link.Get(1));
    }

    //
    // Now, Create the bridge netdevice, which will do the packet switching.  The
    // bridge lives on the node bridge1 and bridges together the topBridgeDevices
    // which are the three CSMA net devices on the node in the diagram above.
    //
    BridgeHelper bridge;
    bridge.Install(bridge1, topBridgeDevices);

    // Add internet stack to the router nodes
    NodeContainer routerNodes(sender, receiver, routeur1, routeur2);
    InternetStackHelper internet;
    internet.Install(routerNodes);

    // Repeat for bottom bridged LAN
    NetDeviceContainer bottomLanDevices;
    NetDeviceContainer bottomBridgeDevices;
    NodeContainer bottomLan(routeur1, routeur2, receiver);
    for (int i = 0; i < 3; i++)
    {
        NetDeviceContainer link = csma.Install(NodeContainer(bottomLan.Get(i), bridge2));
        bottomLanDevices.Add(link.Get(0));
        bottomBridgeDevices.Add(link.Get(1));
    }
    bridge.Install(bridge2, bottomBridgeDevices);

    // We've got the "hardware" in place.  Now we need to add IP addresses.
    NS_LOG_INFO("Assign IP Addresses.");
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("192.168.1.0", "255.255.255.0");
    ipv4.Assign(topLanDevices);
    ipv4.SetBase("10.1.2.0", "255.255.255.0");
    ipv4.Assign(bottomLanDevices);

    //
    // Create router nodes, initialize routing database and set up the routing
    // tables in the nodes.  We excuse the bridge nodes from having to serve as
    // routers, since they don't even have internet stacks on them.
    //
    Ptr<Ipv4> ipv4Sender = sender->GetObject<Ipv4>();
    Ptr<Ipv4> ipv4Rtr1 = routeur1->GetObject<Ipv4>();
    Ptr<Ipv4> ipv4Rtr2 = routeur2->GetObject<Ipv4>();
    Ptr<Ipv4> ipv4Receiver = receiver->GetObject<Ipv4>();

    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ptr<Ipv4StaticRouting> staticRoutingSnd = ipv4RoutingHelper.GetStaticRouting(ipv4Sender);
    Ptr<Ipv4StaticRouting> staticRoutingRtr1 = ipv4RoutingHelper.GetStaticRouting(ipv4Rtr1);
    Ptr<Ipv4StaticRouting> staticRoutingRtr2 = ipv4RoutingHelper.GetStaticRouting(ipv4Rtr2);
    Ptr<Ipv4StaticRouting> staticRoutingRcv= ipv4RoutingHelper.GetStaticRouting(ipv4Receiver);

    
	//staticRoutingSnd->AddNetworkRouteTo(Ipv4Address("O.O.O.O"), Ipv4Mask("0.0.0.0"), Ipv4Address("192.168.1.2"), 1);
	//staticRoutingSnd->AddNetworkRouteTo(Ipv4Address("O.O.O.O"), Ipv4Mask("0.0.0.0"), Ipv4Address("192.168.1.3"), 1);

 	staticRoutingRtr1->AddNetworkRouteTo(Ipv4Address("10.1.2.0"), Ipv4Mask("255.255.255.0"),Ipv4Address("10.1.2.3"),2);
 	staticRoutingRtr2->AddNetworkRouteTo(Ipv4Address("10.1.2.0"), Ipv4Mask("255.255.255.0"),Ipv4Address("10.1.2.3"),2);
 	 
	staticRoutingRcv->AddNetworkRouteTo(Ipv4Address("O.O.O.O"), Ipv4Mask("0.0.0.0"), Ipv4Address("10.1.2.1"), 1);
	//staticRoutingRcv->AddNetworkRouteTo(Ipv4Address("O.O.O.O"), Ipv4Mask("0.0.0.0"), Ipv4Address("10.1.2.2"), 1);


    
    //Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    
    Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper>(&std::cout);
    Ipv4GlobalRoutingHelper::PrintRoutingTableAllAt(Seconds(1.0), routingStream);

    Ptr<OutputStreamWrapper> routingStream2 = Create<OutputStreamWrapper>(&std::cout);
    Ipv4GlobalRoutingHelper::PrintRoutingTableAllAt(Seconds(10.0), routingStream2);
    
    //
    NS_LOG_INFO("Create Applications.");
    //First route
    Address addr1 = Ipv4Address("192.168.1.2");
    Address addr2 = Ipv4Address("192.168.1.3");

    

    Ptr<OnOffApplicationWithNextHop> onoffApp1 = CreateObject<OnOffApplicationWithNextHop> ();
    onoffApp1->SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
    onoffApp1->SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
    onoffApp1->SetAttribute ("PacketSize", UintegerValue (1024));
    onoffApp1->SetAttribute ("DataRate", DataRateValue (DataRate ("50kb/s")));
    onoffApp1->SetAttribute ("Remote", AddressValue (InetSocketAddress (Ipv4Address("10.1.2.3"), 9)));
    onoffApp1->SetAttribute ("FirstHop", AddressValue(addr1));
    onoffApp1->SetAttribute ("Metric", UintegerValue (1));
    sender->AddApplication (onoffApp1);
    onoffApp1->SetStartTime (Seconds (1.0));
    onoffApp1->SetStopTime (Seconds (5.0));

    Ptr<OnOffApplicationWithNextHop> onoffApp2 = CreateObject<OnOffApplicationWithNextHop> ();
    onoffApp2->SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
    onoffApp2->SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
    onoffApp2->SetAttribute ("PacketSize", UintegerValue (1024));
    onoffApp2->SetAttribute ("DataRate", DataRateValue (DataRate ("50kb/s")));
    onoffApp2->SetAttribute ("Remote", AddressValue (InetSocketAddress (Ipv4Address("10.1.2.3"), 9)));
    onoffApp2->SetAttribute ("FirstHop", AddressValue(addr2));
    onoffApp2->SetAttribute ("Metric", UintegerValue (3));
    sender->AddApplication (onoffApp2);
    onoffApp2->SetStartTime (Seconds (5.1));
    onoffApp2->SetStopTime (Seconds (10.0));

    


    // Create an optional packet sink to receive these packets
    PacketSinkHelper sink("ns3::TcpSocketFactory",
                          Address(InetSocketAddress(Ipv4Address::GetAny(), 9)));
    ApplicationContainer sink1 = sink.Install(receiver);
    sink1.Start(Seconds(1.0));
    sink1.Stop(Seconds(5.0));


    // Now, do the actual simulation.


    NS_LOG_INFO("Run Simulation.");
    Simulator::Run();
    Simulator::Destroy();
    NS_LOG_INFO("Done.");

    return 0;
}
