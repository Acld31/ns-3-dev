
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/nix-vector-helper.h"
#include "ns3/node.h"

using namespace ns3;

std::vector<int> 
ParseStringToVector(std::string& input) {
    std::vector<int> result;
    std::stringstream ss(input);
    std::string token;
    while (std::getline(ss, token, ',')) {
        result.push_back(std::stoi(token));
    }
    return result;
}

std::vector<Ptr<Node>> 
CreateCustomParentVector(std::string& input) {
    std::vector<int> intVector = ParseStringToVector(input);
    std::vector<Ptr<Node>> CustomParentVector;

    for(long unsigned int i =0; i < intVector.size(); i++){
        Ptr<Node> node = NodeList::GetNode(intVector[i]);
        CustomParentVector.push_back(node);
    }

    for(long unsigned int i =0; i < CustomParentVector.size(); i++){
         std::cout << "CustomParentVector : " << CustomParentVector[i]->GetId() << "\n";
    
    }
    std::cout << "CustomParentVectorsize : " << CustomParentVector.size() << "\n";
    return CustomParentVector;
}

int main(int argc, char *argv[]) {
    LogComponentEnable("OnOffApplicationNixVector", LOG_LEVEL_INFO);
    LogComponentEnable("PacketSink", LOG_LEVEL_INFO);
    LogComponentEnable("NixVectorRouting", LOG_LEVEL_INFO);
    //LogComponentEnable("Packet", LOG_LEVEL_INFO);
    //LogComponentEnable("Socket", LOG_LEVEL_INFO);
    //LogComponentEnable("NixVector", LOG_LEVEL_DEBUG);


    NodeContainer nodes;
    nodes.Create(6);


    //uint32_t node0 = nodes.Get(1)->GetId();

    //std::cout << node0;

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("0.1Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer devices01, devices12, devices23, devices34, devices45;

    NodeContainer net1, net2, net3, net4, net5;

    net1.Add(nodes.Get(0));
    net1.Add(nodes.Get(1));
    devices01 = pointToPoint.Install(net1);

    net2.Add(nodes.Get(1));
    net2.Add(nodes.Get(2));
    devices12 = pointToPoint.Install(net2);

    net3.Add(nodes.Get(2));
    net3.Add(nodes.Get(3));
    devices23 = pointToPoint.Install(net3);

    net4.Add(nodes.Get(3));
    net4.Add(nodes.Get(4));
    devices34 = pointToPoint.Install(net4);

    net5.Add(nodes.Get(4));
    net5.Add(nodes.Get(5));
    devices45 = pointToPoint.Install(net5);

    Ipv4NixVectorHelper nixRouting;
    InternetStackHelper stack;
    stack.SetRoutingHelper(nixRouting); 
    stack.Install(nodes);

    Ipv4AddressHelper address;

    address.SetBase(Ipv4Address("10.1.1.0"), Ipv4Mask("255.255.255.0"));
    Ipv4InterfaceContainer interfaces01 = address.Assign(devices01);

    address.SetBase(Ipv4Address("10.1.2.0"), Ipv4Mask("255.255.255.0"));
    Ipv4InterfaceContainer interfaces12 = address.Assign(devices12);

    address.SetBase(Ipv4Address("10.1.3.0"), Ipv4Mask("255.255.255.0"));
    Ipv4InterfaceContainer interfaces23 = address.Assign(devices23);

    address.SetBase(Ipv4Address("10.1.4.0"), Ipv4Mask("255.255.255.0"));
    Ipv4InterfaceContainer interfaces34 = address.Assign(devices34);

    address.SetBase(Ipv4Address("10.1.5.0"), Ipv4Mask("255.255.255.0"));
    Ipv4InterfaceContainer interfaces45 = address.Assign(devices45);


    uint16_t port = 9; // Discard port (RFC 863)
    uint32_t packetSize = 1024;

    std::string DataVector = "0,0,1,2,7,4,0,6";
    //std::vector<Ptr<Node>> CustomParentVector = CreateCustomParentVector(DataVector);

    OnOffHelperNixVector ooh("ns3::TcpSocketFactory", InetSocketAddress(InetSocketAddress(interfaces45.GetAddress(1), port)), DataVector, DataVector);
    ooh.SetConstantRate(DataRate("5kb/s"), 1024);
    ooh.SetAttribute("PacketSize", UintegerValue(packetSize));
    ooh.SetAttribute("CustomParentVector1", StringValue(DataVector));
    ApplicationContainer client = ooh.Install(nodes.Get(0));
    client.Start(Seconds(0.0));
    client.Stop(Seconds(3.0));

    PacketSinkHelper psh("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(),port));
    ApplicationContainer serveur = psh.Install(nodes.Get(5));
    serveur.Start(Seconds(0.0));
    serveur.Stop(Seconds(3.0));


    Simulator::Run();
    Simulator::Destroy();

    return 0;
}