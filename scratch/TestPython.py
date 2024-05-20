from ns import ns


ns.core.LogComponentEnable("OnOffApplicationNixVector", ns.core.LOG_LEVEL_INFO)
ns.core.LogComponentEnable("PacketSink", ns.core.LOG_LEVEL_INFO)
ns.core.LogComponentEnable("NixVectorRouting", ns.core.LOG_LEVEL_INFO)
ns.core.LogComponentEnable("Packet", ns.core.LOG_LEVEL_INFO)


#ns.core.LogComponentEnableAll(ns.core.LOG_LEVEL_INFO)

nodes = ns.network.NodeContainer()
nodes.Create(8)

pointToPoint = ns.point_to_point.PointToPointHelper()
pointToPoint.SetDeviceAttribute("DataRate", ns.core.StringValue("0.1Mbps"))
pointToPoint.SetChannelAttribute("Delay", ns.core.StringValue("2ms"))

devices01 = ns.network.NetDeviceContainer()
devices12 = ns.network.NetDeviceContainer()
devices23 = ns.network.NetDeviceContainer()
devices34 = ns.network.NetDeviceContainer()
devices45 = ns.network.NetDeviceContainer()
devices06 = ns.network.NetDeviceContainer()
devices67 = ns.network.NetDeviceContainer()
devices74 = ns.network.NetDeviceContainer()

net1 = ns.NodeContainer()
net1.Add(nodes.Get(0))
net1.Add(nodes.Get(1))
devices01 = pointToPoint.Install(net1)

net2 = ns.NodeContainer()
net2.Add(nodes.Get(1))
net2.Add(nodes.Get(2))
devices12 = pointToPoint.Install(net2)

net3 = ns.NodeContainer()
net3.Add(nodes.Get(2))
net3.Add(nodes.Get(3))
devices23 = pointToPoint.Install(net3)

net4 = ns.NodeContainer()
net4.Add(nodes.Get(3))
net4.Add(nodes.Get(4))
devices34 = pointToPoint.Install(net4)

net5 = ns.NodeContainer()
net5.Add(nodes.Get(4))
net5.Add(nodes.Get(5))
devices45 = pointToPoint.Install(net5)

net6 = ns.NodeContainer()
net6.Add(nodes.Get(0))
net6.Add(nodes.Get(6))
devices06 = pointToPoint.Install(net6)

net7 = ns.NodeContainer()
net7.Add(nodes.Get(6))
net7.Add(nodes.Get(7))
devices67 = pointToPoint.Install(net7)

net8 = ns.NodeContainer()
net8.Add(nodes.Get(7))
net8.Add(nodes.Get(4))
devices74 = pointToPoint.Install(net8)

#print(help(ns.internet.Ipv4NixVector))
nixRouting =ns.internet.Ipv4NixVectorHelper()
stack = ns.internet.InternetStackHelper()
print(nixRouting)
stack.SetRoutingHelper(nixRouting)
stack.Install(nodes)

address01 = ns.internet.Ipv4AddressHelper()
address01.SetBase(ns.network.Ipv4Address("10.1.1.0"),
                ns.network.Ipv4Mask("255.255.255.0"))
interfaces01 = address01.Assign(devices01)

address12 = ns.internet.Ipv4AddressHelper()
address12.SetBase(ns.network.Ipv4Address("10.1.2.0"),
                ns.network.Ipv4Mask("255.255.255.0"))
interfaces12 = address12.Assign(devices12)

address23 = ns.internet.Ipv4AddressHelper()
address23.SetBase(ns.network.Ipv4Address("10.1.3.0"),
                ns.network.Ipv4Mask("255.255.255.0"))
interfaces23 = address23.Assign(devices23)

address34 = ns.internet.Ipv4AddressHelper()
address34.SetBase(ns.network.Ipv4Address("10.1.4.0"),
                ns.network.Ipv4Mask("255.255.255.0"))
interfaces34 = address34.Assign(devices34)

address45 = ns.internet.Ipv4AddressHelper()
address45.SetBase(ns.network.Ipv4Address("10.1.5.0"),
                ns.network.Ipv4Mask("255.255.255.0"))
interfaces45 = address45.Assign(devices45)

address06 = ns.internet.Ipv4AddressHelper()
address06.SetBase(ns.network.Ipv4Address("10.1.6.0"),
                ns.network.Ipv4Mask("255.255.255.0"))
interfaces06 = address06.Assign(devices06)

address67 = ns.internet.Ipv4AddressHelper()
address67.SetBase(ns.network.Ipv4Address("10.1.7.0"),
                ns.network.Ipv4Mask("255.255.255.0"))
interfaces67 = address67.Assign(devices67)

address74 = ns.internet.Ipv4AddressHelper()
address74.SetBase(ns.network.Ipv4Address("10.1.8.0"),
                ns.network.Ipv4Mask("255.255.255.0"))
interfaces74 = address74.Assign(devices74)


#App
#print(dir(ns.OnOffApplication))
port = 9   #  Discard port(RFC 863)
PacketSize = 1024

address = interfaces45.GetAddress(1)
socketAddr = ns.network.InetSocketAddress(address, port)

parentVector1 = '0,0,1,2,7,4,0,6'
parentVector2 = '6,2,3,4,5,5,7,4'


packetSize = 1024
onoff = ns.applications.OnOffHelperNixVector("ns3::TcpSocketFactory", socketAddr.ConvertTo(), parentVector1, parentVector2)
onoff.SetConstantRate(ns.network.DataRate("5kb/s"))
onoff.SetAttribute("PacketSize", ns.core.UintegerValue(packetSize))

appOnOff = onoff.Install(nodes.Get(0))
appOnOff.Start(ns.core.Seconds(1))
appOnOff.Stop(ns.core.Seconds(3))

sink = ns.applications.PacketSinkHelper("ns3::TcpSocketFactory",
                            ns.network.InetSocketAddress(ns.network.InetSocketAddress(ns.network.Ipv4Address.GetAny(), port)).ConvertTo())
appSink = sink.Install(nodes.Get(5))
appSink.Start(ns.core.Seconds(1))
appSink.Stop(ns.core.Seconds(3))


ns.core.Simulator.Run()
ns.core.Simulator.Destroy()
