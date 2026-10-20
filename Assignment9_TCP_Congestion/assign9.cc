#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include <fstream>

using namespace ns3;

static std::ofstream cwndFile;

static void
CwndChange(uint32_t oldCwnd, uint32_t newCwnd)
{
    cwndFile << Simulator::Now().GetSeconds() << "," << oldCwnd << "," << newCwnd << "\n";
}

static void
ConnectCwndTraces()
{
    // Wildcard trace records congestion-window changes from TCP sockets that exist now.
    Config::ConnectWithoutContextFailSafe(
        "/NodeList/*/$ns3::TcpL4Protocol/SocketList/*/CongestionWindow",
        MakeCallback(&CwndChange));
}

int main(int argc, char *argv[])
{
    std::string tcpType = "ns3::TcpNewReno";
    std::string bottleneckRate = "2Mbps";
    CommandLine cmd;
    cmd.AddValue("tcpType", "TCP variant: ns3::TcpNewReno, ns3::TcpCubic, or ns3::TcpBbr (if available)", tcpType);
    cmd.AddValue("bottleneckRate", "Shared road-to-server link rate", bottleneckRate);
    cmd.Parse(argc, argv);

    TypeId tcpTid;
    if (!TypeId::LookupByNameFailSafe(tcpType, &tcpTid))
    {
        std::cerr << "ERROR: This ns-3 installation does not provide '" << tcpType << "'.\n"
                  << "Try ns3::TcpNewReno or ns3::TcpCubic. BBR is version-dependent.\n";
        return 1;
    }
    Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(tcpTid));
    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(1024));

    const uint32_t vehicleCount = 5;
    NodeContainer vehicles; vehicles.Create(vehicleCount);
    Ptr<Node> roadsideRouter = CreateObject<Node>();
    Ptr<Node> server = CreateObject<Node>();
    NodeContainer all;
    all.Add(vehicles); all.Add(roadsideRouter); all.Add(server);
    InternetStackHelper internet; internet.Install(all);

    PointToPointHelper vehicleLink;
    vehicleLink.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    vehicleLink.SetChannelAttribute("Delay", StringValue("2ms"));
    PointToPointHelper roadLink;
    roadLink.SetDeviceAttribute("DataRate", StringValue(bottleneckRate));
    roadLink.SetChannelAttribute("Delay", StringValue("20ms"));

    Ipv4AddressHelper address;
    std::vector<Ipv4Address> serverAddresses;
    for (uint32_t i = 0; i < vehicleCount; ++i)
    {
        NodeContainer pair(vehicles.Get(i), roadsideRouter);
        NetDeviceContainer devices = vehicleLink.Install(pair);
        std::ostringstream subnet; subnet << "10.1." << (i + 1) << ".0";
        address.SetBase(subnet.str().c_str(), "255.255.255.0");
        address.Assign(devices);
    }
    NodeContainer finalPair(roadsideRouter, server);
    NetDeviceContainer finalDevices = roadLink.Install(finalPair);
    address.SetBase("10.1.20.0", "255.255.255.0");
    Ipv4InterfaceContainer finalInterfaces = address.Assign(finalDevices);
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    for (uint32_t i = 0; i < vehicleCount; ++i)
    {
        uint16_t port = 5000 + i;
        PacketSinkHelper sink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
        ApplicationContainer sinkApp = sink.Install(server);
        sinkApp.Start(Seconds(0.5)); sinkApp.Stop(Seconds(30.0));

        BulkSendHelper source("ns3::TcpSocketFactory", InetSocketAddress(finalInterfaces.GetAddress(1), port));
        source.SetAttribute("SendSize", UintegerValue(1024));
        source.SetAttribute("MaxBytes", UintegerValue(0));
        ApplicationContainer sourceApp = source.Install(vehicles.Get(i));
        sourceApp.Start(Seconds(1.0 + 0.1 * i)); sourceApp.Stop(Seconds(29.0));
    }

    roadLink.EnablePcapAll("assign9-road-link");
    cwndFile.open("assign9-cwnd.csv");
    cwndFile << "time_seconds,old_cwnd_bytes,new_cwnd_bytes\n";
    Simulator::Schedule(Seconds(1.5), &ConnectCwndTraces);

    FlowMonitorHelper flowHelper;
    Ptr<FlowMonitor> monitor = flowHelper.InstallAll();
    Simulator::Stop(Seconds(30.0));
    Simulator::Run();
    monitor->CheckForLostPackets();

    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowHelper.GetClassifier());
    std::vector<double> throughputs;
    std::cout << "\nAssignment 9: TCP=" << tcpType << ", shared link=" << bottleneckRate << "\n";
    std::cout << "Flow port | Tx | Rx | Loss (%) | Throughput (Mbps) | Mean delay (ms)\n";
    for (const auto &entry : monitor->GetFlowStats())
    {
        Ipv4FlowClassifier::FiveTuple tuple = classifier->FindFlow(entry.first);
        if (tuple.destinationPort < 5000 || tuple.destinationPort >= 5000 + vehicleCount) continue;
        const FlowMonitor::FlowStats &s = entry.second;
        double loss = s.txPackets == 0 ? 0.0 : 100.0 * (s.txPackets - s.rxPackets) / s.txPackets;
        double duration = s.timeLastRxPacket.GetSeconds() - s.timeFirstTxPacket.GetSeconds();
        double throughput = duration > 0 ? s.rxBytes * 8.0 / duration / 1000000.0 : 0.0;
        double delay = s.rxPackets > 0 ? s.delaySum.GetMilliSeconds() / s.rxPackets : 0.0;
        throughputs.push_back(throughput);
        std::cout << tuple.destinationPort << " | " << s.txPackets << " | " << s.rxPackets
                  << " | " << loss << " | " << throughput << " | " << delay << "\n";
    }

    double sum = 0.0, sumSquares = 0.0;
    for (double t : throughputs) { sum += t; sumSquares += t * t; }
    double fairness = sumSquares > 0 ? (sum * sum) / (throughputs.size() * sumSquares) : 0.0;
    std::cout << "Jain fairness index (1 means equal throughput): " << fairness << "\n";
    std::cout << "Congestion-window changes: assign9-cwnd.csv; packet capture: assign9-road-link-*.pcap\n";
    monitor->SerializeToXmlFile("assign9-flowmon.xml", true, true);
    cwndFile.close();
    Simulator::Destroy();
    return 0;
}
