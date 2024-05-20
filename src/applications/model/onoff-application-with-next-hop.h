

#ifndef ONOFF_APPLICATION_WITH_NEXT_HOP_H
#define ONOFF_APPLICATION_WITH_NEXT_HOP_H

#include "onoff-application.h"
#include "ns3/internet-module.h"
#include "ns3/address.h"

#include "seq-ts-size-header.h"

#include "ns3/application.h"
#include "ns3/data-rate.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/traced-callback.h"

namespace ns3
{


class OnOffApplicationWithNextHop : public OnOffApplication
{
public:

    static TypeId GetTypeId();
    OnOffApplicationWithNextHop();
    ~OnOffApplicationWithNextHop() override;

private:
    // inherited from Application base class.
    void StartApplication() override;
    //void StartApplication(); // Called at time specified by Start
    //void StopApplication() override;  // Called at time specified by Stop

    Address m_nextHop;                      //!< adresse du premier saut
    uint32_t m_metric;                      //!< metric de la route
};

} // namespace ns3

#endif /* ONOFF_APPLICATION_WITH_NEXT_HOP_H */
