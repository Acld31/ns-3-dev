from ns import ns


ns.core.LogComponentEnable("OnOffApplicationNixVector", ns.core.LOG_LEVEL_INFO)
ns.core.LogComponentEnable("PacketSink", ns.core.LOG_LEVEL_INFO)
ns.core.LogComponentEnable("NixVectorRouting", ns.core.LOG_LEVEL_INFO)
#ns.core.LogComponentEnable("Packet", ns.core.LOG_LEVEL_INFO)


#ns.core.LogComponentEnableAll(ns.core.LOG_LEVEL_INFO)

nodes = ns.network.NodeContainer()
nodes.Create(6)

pointToPoint = ns.point_to_point.PointToPointHelper()
pointToPoint.SetDeviceAttribute("DataRate", ns.core.StringValue("0.1Mbps"))
pointToPoint.SetChannelAttribute("Delay", ns.core.StringValue("2ms"))

devices01 = ns.network.NetDeviceContainer()
devices12 = ns.network.NetDeviceContainer()
devices23 = ns.network.NetDeviceContainer()
devices14 = ns.network.NetDeviceContainer()
devices43 = ns.network.NetDeviceContainer()
devices35 = ns.network.NetDeviceContainer()

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
net4.Add(nodes.Get(1))
net4.Add(nodes.Get(4))
devices14 = pointToPoint.Install(net4)

net5 = ns.NodeContainer()
net5.Add(nodes.Get(4))
net5.Add(nodes.Get(3))
devices43 = pointToPoint.Install(net5)

net6 = ns.NodeContainer()
net6.Add(nodes.Get(3))
net6.Add(nodes.Get(5))
devices35 = pointToPoint.Install(net6)

#print(help(ns.internet.Ipv4NixVector))
nixRouting =ns.internet.Ipv4NixVectorHelper()
stack = ns.internet.InternetStackHelper()
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

address14 = ns.internet.Ipv4AddressHelper()
address14.SetBase(ns.network.Ipv4Address("10.1.4.0"),
                ns.network.Ipv4Mask("255.255.255.0"))
interfaces14 = address14.Assign(devices14)

address43 = ns.internet.Ipv4AddressHelper()
address43.SetBase(ns.network.Ipv4Address("10.1.5.0"),
                ns.network.Ipv4Mask("255.255.255.0"))
interfaces43 = address43.Assign(devices43)

address35 = ns.internet.Ipv4AddressHelper()
address35.SetBase(ns.network.Ipv4Address("10.1.6.0"),
                ns.network.Ipv4Mask("255.255.255.0"))
interfaces35 = address35.Assign(devices35)


#App
#print(dir(ns.OnOffApplication))
port = 9   #  Discard port(RFC 863)
PacketSize = 1024

address = interfaces35.GetAddress(1)
socketAddr = ns.network.InetSocketAddress(address, port)

parentVector1 = '0,0,1,4,1,3'
parentVector2 = '1,4,3,5,3,5'

#sourceAdress = ns.network.Ipv4Address(interfaces01.GetAddress(0))

sourceAdress = interfaces01.GetAddress(0)

print(sourceAdress.ConvertTo())

onoff = ns.applications.OnOffHelperNixVector("ns3::TcpSocketFactory", socketAddr.ConvertTo())
onoff.SetConstantRate(ns.network.DataRate("5kb/s"))
onoff.SetAttribute("PacketSize", ns.core.UintegerValue(PacketSize))
onoff.SetAttribute("CustomParentVectorStD", ns.core.StringValue(parentVector1))
appOnOff = onoff.Install(nodes.Get(0))
appOnOff.Start(ns.core.Seconds(1))
appOnOff.Stop(ns.core.Seconds(10))

sink = ns.applications.PacketSinkHelper("ns3::TcpSocketFactory",
                            ns.network.InetSocketAddress(ns.network.InetSocketAddress(ns.network.Ipv4Address.GetAny(), port)).ConvertTo())
sink.SetAttribute("CustomParentVectorDtS", ns.core.StringValue(parentVector2))
sink.SetAttribute("SenderIpAdd", ns.network.AddressValue(sourceAdress.ConvertTo()))

appSink = sink.Install(nodes.Get(5))
appSink.Start(ns.core.Seconds(1))
appSink.Stop(ns.core.Seconds(10))


ns.core.Simulator.Run()
ns.core.Simulator.Destroy()
