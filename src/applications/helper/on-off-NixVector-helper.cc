#include "on-off-NixVector-helper.h"
#include "ns3/boolean.h"
#include "ns3/data-rate.h"
#include "ns3/inet-socket-address.h"
#include "ns3/names.h"
#include "ns3/onoff-application.h"
#include "ns3/onoff-application-NixVector.h"
#include "ns3/packet-socket-address.h"
#include "ns3/random-variable-stream.h"
#include "ns3/string.h"
#include "ns3/uinteger.h"

namespace ns3
{

OnOffHelperNixVector::OnOffHelperNixVector(std::string protocol, Address address)
{
    m_factory.SetTypeId("ns3::OnOffApplicationNixVector");
    m_factory.Set("Protocol", StringValue(protocol));
    m_factory.Set("Remote", AddressValue(address));

}

void
OnOffHelperNixVector::SetAttribute(std::string name, const AttributeValue& value)
{
    m_factory.Set(name, value);
}

ApplicationContainer
OnOffHelperNixVector::Install(Ptr<Node> node) const
{
    return ApplicationContainer(InstallPriv(node));
}

ApplicationContainer
OnOffHelperNixVector::Install(std::string nodeName) const
{
    Ptr<Node> node = Names::Find<Node>(nodeName);
    return ApplicationContainer(InstallPriv(node));
}

ApplicationContainer
OnOffHelperNixVector::Install(NodeContainer c) const
{
    ApplicationContainer apps;
    for (auto i = c.Begin(); i != c.End(); ++i)
    {
        apps.Add(InstallPriv(*i));
    }

    return apps;
}

Ptr<Application>
OnOffHelperNixVector::InstallPriv(Ptr<Node> node) const
{
    Ptr<Application> app = m_factory.Create<Application>();
    node->AddApplication(app);

    return app;
}

int64_t
OnOffHelperNixVector::AssignStreams(NodeContainer c, int64_t stream)
{
    int64_t currentStream = stream;
    Ptr<Node> node;
    for (auto i = c.Begin(); i != c.End(); ++i)
    {
        node = (*i);
        for (uint32_t j = 0; j < node->GetNApplications(); j++)
        {
            Ptr<OnOffApplicationNixVector> onoff = DynamicCast<OnOffApplicationNixVector>(node->GetApplication(j));
            if (onoff)
            {
                currentStream += onoff->AssignStreams(currentStream);
            }
        }
    }
    return (currentStream - stream);
}

void
OnOffHelperNixVector::SetConstantRate(DataRate dataRate, uint32_t packetSize)
{
    m_factory.Set("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1000]"));
    m_factory.Set("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    m_factory.Set("DataRate", DataRateValue(dataRate));
    m_factory.Set("PacketSize", UintegerValue(packetSize));
}

} // namespace ns3
