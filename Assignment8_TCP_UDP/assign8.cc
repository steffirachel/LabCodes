#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;

int main(int argc, char *argv[])
{
    std::string bottleneckRate = "2Mbps";
    std::string bottleneckDelay = "10ms";
    uint32_t queuePackets = 50;
    double udpRateMbps = 1.0;
    CommandLine cmd;
    cmd.AddValue("bottleneckRate", "Bottleneck link rate: 2Mbps, 5Mbps, or 10Mbps", bottleneckRate);
    cmd.AddValue("bottleneckDelay", "Bottleneck propagation delay", bottleneckDelay);
    cmd.AddValue("queuePackets", "DropTail queue capacity in packets", queuePackets);
    cmd.AddValue("udpRateMbps", "UDP offered rate in Mbps", udpRateMbps);
    cmd.Parse(argc, argv);

    // Nodes 0 and 1 are senders; nodes 2 and 3 are receivers; 4 and 5 are routers.
    NodeContainer nodes;
    nodes.Create(6);
    NodeContainer access0(nodes.Get(0), nodes.Get(4));
    NodeContainer access1(nodes.Get(1), nodes.Get(4));
    NodeContainer bottleneckNodes(nodes.Get(4), nodes.Get(5));
    NodeContainer access2(nodes.Get(5), nodes.Get(2));
    NodeContainer access3(nodes.Get(5), nodes.Get(3));

    PointToPointHelper access;
    access.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    access.SetChannelAttribute("Delay", StringValue("2ms"));

    PointToPointHelper bottleneck;
    bottleneck.SetDeviceAttribute("DataRate", StringValue(bottleneckRate));
    bottleneck.SetChannelAttribute("Delay", StringValue(bottleneckDelay));
    bottleneck.SetQueue("ns3::DropTailQueue<Packet>", "MaxSize",
                        StringValue(std::to_string(queuePackets) + "p"));

    NetDeviceContainer d0 = access.Install(access0);
    NetDeviceContainer d1 = access.Install(access1);
    NetDeviceContainer db = bottleneck.Install(bottleneckNodes);
    NetDeviceContainer d2 = access.Install(access2);
    NetDeviceContainer d3 = access.Install(access3);

    InternetStackHelper internet;
    internet.Install(nodes);
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0"); address.Assign(d0);
    address.SetBase("10.1.2.0", "255.255.255.0"); address.Assign(d1);
    address.SetBase("10.1.3.0", "255.255.255.0"); address.Assign(db);
    address.SetBase("10.1.4.0", "255.255.255.0"); Ipv4InterfaceContainer i2 = address.Assign(d2);
    address.SetBase("10.1.5.0", "255.255.255.0"); address.Assign(d3);
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(1024));
    const uint16_t tcpPort = 5000;
    const uint16_t udpPort = 6000;

    PacketSinkHelper tcpSink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), tcpPort));
    ApplicationContainer tcpSinkApp = tcpSink.Install(nodes.Get(3));
    tcpSinkApp.Start(Seconds(0.5)); tcpSinkApp.Stop(Seconds(20.0));

    BulkSendHelper tcpSource("ns3::TcpSocketFactory", InetSocketAddress(nodes.Get(3)->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal(), tcpPort));
    // Use the receiver's address from its interface, not a hard-coded address.
    tcpSource.SetAttribute("SendSize", UintegerValue(1024));
    tcpSource.SetAttribute("MaxBytes", UintegerValue(0));
    ApplicationContainer tcpSourceApp = tcpSource.Install(nodes.Get(0));
    tcpSourceApp.Start(Seconds(1.0)); tcpSourceApp.Stop(Seconds(19.5));

    UdpServerHelper udpServer(udpPort);
    ApplicationContainer udpServerApp = udpServer.Install(nodes.Get(2));
    udpServerApp.Start(Seconds(0.5)); udpServerApp.Stop(Seconds(20.0));

    double udpInterval = (512.0 * 8.0) / (udpRateMbps * 1000000.0);
    UdpClientHelper udpClient(i2.GetAddress(1), udpPort);
    udpClient.SetAttribute("MaxPackets", UintegerValue(30000));
    udpClient.SetAttribute("Interval", TimeValue(Seconds(udpInterval)));
    udpClient.SetAttribute("PacketSize", UintegerValue(512));
    ApplicationContainer udpClientApp = udpClient.Install(nodes.Get(1));
    udpClientApp.Start(Seconds(1.0)); udpClientApp.Stop(Seconds(19.5));

    // Capture the middle link. The generated pcap can be opened in Wireshark.
    bottleneck.EnablePcapAll("assign8-bottleneck");

    FlowMonitorHelper flowHelper;
    Ptr<FlowMonitor> monitor = flowHelper.InstallAll();
    Simulator::Stop(Seconds(20.0));
    Simulator::Run();
    monitor->CheckForLostPackets();

    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowHelper.GetClassifier());
    std::cout << "\nAssignment 8 results | bottleneck=" << bottleneckRate
              << " | delay=" << bottleneckDelay << " | queue=" << queuePackets
              << " packets | UDP offered rate=" << udpRateMbps << " Mbps\n";
    std::cout << "Source port -> destination port | Tx | Rx | Loss (%) | Throughput (Mbps) | Mean delay (ms)\n";
    for (const auto &entry : monitor->GetFlowStats())
    {
        Ipv4FlowClassifier::FiveTuple tuple = classifier->FindFlow(entry.first);
        if (tuple.destinationPort != tcpPort && tuple.destinationPort != udpPort) continue;
        const FlowMonitor::FlowStats &s = entry.second;
        double loss = s.txPackets == 0 ? 0.0 : 100.0 * (s.txPackets - s.rxPackets) / s.txPackets;
        double duration = s.timeLastRxPacket.GetSeconds() - s.timeFirstTxPacket.GetSeconds();
        double throughput = duration > 0 ? s.rxBytes * 8.0 / duration / 1000000.0 : 0.0;
        double delay = s.rxPackets > 0 ? s.delaySum.GetMilliSeconds() / s.rxPackets : 0.0;
        std::cout << tuple.sourcePort << " -> " << tuple.destinationPort << " | "
                  << s.txPackets << " | " << s.rxPackets << " | " << loss << " | "
                  << throughput << " | " << delay << "\n";
    }
    monitor->SerializeToXmlFile("assign8-flowmon.xml", true, true);
    Simulator::Destroy();
    return 0;
}
