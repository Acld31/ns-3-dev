#include "onoff-application-with-next-hop.h"
#include "onoff-application.h"

#include "ns3/address.h"
#include "ns3/boolean.h"
#include "ns3/data-rate.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/nstime.h"
#include "ns3/packet-socket-address.h"
#include "ns3/packet.h"
#include "ns3/pointer.h"
#include "ns3/random-variable-stream.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/socket.h"
#include "ns3/string.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/uinteger.h"


namespace ns3 {

NS_LOG_COMPONENT_DEFINE("OnOffApplicationWithNextHop");

NS_OBJECT_ENSURE_REGISTERED(OnOffApplicationWithNextHop); 

TypeId
OnOffApplicationWithNextHop::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::OnOffApplicationWithNextHop")
            .SetParent<OnOffApplication>()
            .SetGroupName("OnOffApplication")
            .AddConstructor<OnOffApplicationWithNextHop>()
            .AddAttribute("Metric",
                          "Priority of the route",
                          UintegerValue(0),
                          MakeUintegerAccessor(&OnOffApplicationWithNextHop::m_metric),
                          MakeUintegerChecker<uint32_t>(1))
            .AddAttribute("FirstHop",
                          "The address of the first hop",
                          AddressValue(),
                          MakeAddressAccessor(&OnOffApplicationWithNextHop::m_nextHop),
                          MakeAddressChecker());
    return tid;
}

OnOffApplicationWithNextHop::OnOffApplicationWithNextHop() 
    : OnOffApplication()
{
  NS_LOG_FUNCTION(this);
}


OnOffApplicationWithNextHop::~OnOffApplicationWithNextHop() {
    NS_LOG_FUNCTION(this);
}

void
OnOffApplicationWithNextHop::StartApplication() // Called at time specified by Start
{
    NS_LOG_FUNCTION(this);

    // Create the socket if not already
    if (!m_socket)
    {
        m_socket = Socket::CreateSocket(GetNode(), m_tid);
        int ret = -1;

        if (!m_local.IsInvalid())
        {
            NS_ABORT_MSG_IF((Inet6SocketAddress::IsMatchingType(m_peer) &&
                             InetSocketAddress::IsMatchingType(m_local)) ||
                                (InetSocketAddress::IsMatchingType(m_peer) &&
                                 Inet6SocketAddress::IsMatchingType(m_local)),
                            "Incompatible peer and local address IP version");
            ret = m_socket->Bind(m_local);
        }
        else
        {
            if (Inet6SocketAddress::IsMatchingType(m_peer))
            {
                ret = m_socket->Bind6();
            }
            else if (InetSocketAddress::IsMatchingType(m_peer) ||
                     PacketSocketAddress::IsMatchingType(m_peer))
            {
                ret = m_socket->Bind();
            }
        }

        if (ret == -1)
        {
            NS_FATAL_ERROR("Failed to bind socket");
        }

        m_socket->SetConnectCallback(MakeCallback(&OnOffApplicationWithNextHop::ConnectionSucceeded, this),
                                     MakeCallback(&OnOffApplicationWithNextHop::ConnectionFailed, this));

        m_socket->Connect(m_peer);
        m_socket->SetAllowBroadcast(true);
        m_socket->ShutdownRecv();
    }
    m_cbrRateFailSafe = m_cbrRate;

    // Ensure no pending event
    CancelEvents();

    // If we are not yet connected, there is nothing to do here,
    // the ConnectionComplete upcall will start timers at that time.
    // If we are already connected, CancelEvents did remove the events,
    // so we have to start them again.
    Ptr<Node> sender = GetNode();
    Ptr<Ipv4> ipv4Sender = sender->GetObject<Ipv4>();
    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ptr<Ipv4StaticRouting> staticRoutingSnd = ipv4RoutingHelper.GetStaticRouting(ipv4Sender);
    
    Ipv4Address ipv4FirstHop = Ipv4Address::ConvertFrom(m_nextHop);

    staticRoutingSnd->AddNetworkRouteTo(Ipv4Address("O.O.O.O"), Ipv4Mask("0.0.0.0"), ipv4FirstHop, 1, m_metric);

    if (m_connected)
    {
        ScheduleStartEvent();
    }
}

} // namespace ns3
