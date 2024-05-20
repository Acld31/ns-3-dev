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

    NodeContainer noeuds;
    noeuds.Create(2);

    // Crée un channel d'une longueur de 100km à 2.10⁵km/s
//    Ptr<SimpleChannel> sc = new SimpleChannel();
    //sc->SetAttribute("Delay", TimeValue(Seconds(0.0005)));
    

    
    /*
    * Attache le channel sc au noeuds créés et on le limite à seulement deux noeuds
    * avec une file d'attente de 1000packets maximum et un débit de 10Mbps
    * Attention on ne peut pas configurer la MTU avec SimpleNetDeviceHelper...
    */
    SimpleNetDeviceHelper sndh;
    sndh.SetNetDevicePointToPointMode(true);
    sndh.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue("1000p"));
    sndh.SetDeviceAttribute("DataRate", DataRateValue(DataRate("10Mb/s")));
    sndh.SetChannel("ns3::SimpleChannel", "Delay", TimeValue(MilliSeconds(5)));
    
    NetDeviceContainer ndc = sndh.Install(noeuds);

    
    /*
    * Ajout des informations IPv4 pour chacun des noeuds
    * noeud0 a adresse 192.168.0.1
    * noeud1 a adresse 192.168.0.2
    */   

    // Création de la pile IPv4 et TCP/UDP en utilisant le helper
    InternetStackHelper ipV4;
    ipV4.SetIpv6StackInstall(false);
    ipV4.Install(noeuds);

    /*
    * Affectation des adresses IP
    * J'ai récupéré le code de Ipv4AddressHelper et je l'ai modifié
    */
    int32_t interface;
    Ptr<Ipv4> ip;
    Ipv4InterfaceAddress ipadd;
    Ptr<TrafficControlLayer> tc;
    Ptr<NetDeviceQueueInterface> ndqi;
    TrafficControlHelper tcHelper;
    
    // Noeud0
    Ptr<Node> noeud0 = noeuds.Get(0);
    Ptr<NetDevice> nd0 = noeud0->GetDevice(0);
    ip = noeud0->GetObject<Ipv4>();
    interface = ip->GetInterfaceForDevice(nd0);
    if (interface == -1){
       interface = ip->AddInterface(nd0);
    }
    
    // Ajout de l'adresse IP à noeud0
    ipadd = Ipv4InterfaceAddress(Ipv4Address("192.168.0.1"), Ipv4Mask("255.255.255.0"));
    ip->AddAddress(interface, ipadd);
    ip->SetMetric(interface, 1);
    ip->SetUp(interface);
    
    // On gère la queue de noeud0 pour la lier à la queue de l'interface
    tc = noeud0->GetObject<TrafficControlLayer>();
    if (tc && !DynamicCast<LoopbackNetDevice>(nd0) && !tc->GetRootQueueDiscOnDevice(nd0)){
       ndqi = nd0->GetObject<NetDeviceQueueInterface>();
       if (ndqi){
          std::size_t nTxQueues = ndqi->GetNTxQueues();
          NS_LOG_LOGIC("Installing default traffic control configuration ("
                             << nTxQueues << " device queue(s))");
          TrafficControlHelper tcHelper = TrafficControlHelper::Default(nTxQueues);
          tcHelper.Install(nd0);
       }
    }

    // Noeud1
    Ptr<Node> noeud1 = noeuds.Get(1);
    Ptr<NetDevice> nd1 = noeud1->GetDevice(0);
    ip = noeud1->GetObject<Ipv4>();
    interface = ip->GetInterfaceForDevice(nd1);
    if (interface == -1){
       interface = ip->AddInterface(nd1);
    }
    
    // Ajout de l'adresse IP à noeud1
    ipadd = Ipv4InterfaceAddress(Ipv4Address("192.168.0.2"),Ipv4Mask("255.255.255.0"));
    ip->AddAddress(interface, ipadd);
    ip->SetMetric(interface, 1);
    ip->SetUp(interface);
    
    // On gère la queue de noeud1 pour la lier à la queue de l'interface
    tc = noeud1->GetObject<TrafficControlLayer>();
    if (tc && !DynamicCast<LoopbackNetDevice>(nd1) && !tc->GetRootQueueDiscOnDevice(nd1)){
       ndqi = nd1->GetObject<NetDeviceQueueInterface>();
       if (ndqi){
          std::size_t nTxQueues = ndqi->GetNTxQueues();
          NS_LOG_LOGIC("Installing default traffic control configuration ("
                             << nTxQueues << " device queue(s))");
          tcHelper = TrafficControlHelper::Default(nTxQueues);
          tcHelper.Install(nd1);
       }
    }

    /*
    * On gère l'application avec noeud0 comme serveur et noeud1 comme client
    *
    */
    OnOffHelper ooh("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address("192.168.0.1"),5000));
    ooh.SetConstantRate(DataRate("2KBps"), 1000);
    ooh.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=10.0]"));
    ooh.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=1.0]"));
    ooh.SetAttribute("StartTime", TimeValue(Seconds(1.0)));
    ooh.SetAttribute("StopTime", TimeValue(Seconds(20.0)));
    ApplicationContainer client = ooh.Install(noeud1);
    client.Start(Seconds(1.0));
    client.Stop(Seconds(20.0));

    PacketSinkHelper psh("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(),5000));
    psh.SetAttribute("StartTime", TimeValue(Seconds(0.0)));
    psh.SetAttribute("StopTime", TimeValue(Seconds(20.0)));
    ApplicationContainer serveur = psh.Install(noeud0);
    serveur.Start(Seconds(0.0));
    serveur.Stop(Seconds(20.0));
    
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}