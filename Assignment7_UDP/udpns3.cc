#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;

int main(int argc, char *argv[])
{
    std::string delay = "2ms";
    double rateMbps = 1.0;
    CommandLine cmd;
    cmd.AddValue("delay", "Point-to-point link delay (for example 2ms, 5ms, 10ms, 20ms)", delay);
    cmd.AddValue("rateMbps", "Offered rate of each UDP flow in Mbps", rateMbps);
    cmd.Parse(argc, argv);

    NodeContainer nodes;
    nodes.Create(2);

    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue(delay));
    NetDeviceContainer devices = p2p.Install(nodes);

    InternetStackHelper internet;
    internet.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    const uint16_t port1 = 5000;
    const uint16_t port2 = 6000;
    const uint32_t packetSize = 512;
    double intervalSeconds = (packetSize * 8.0) / (rateMbps * 1000000.0);

    UdpServerHelper server1(port1);
    ApplicationContainer serverApp1 = server1.Install(nodes.Get(1));
    serverApp1.Start(Seconds(0.5));
    serverApp1.Stop(Seconds(11.0));

    UdpServerHelper server2(port2);
    ApplicationContainer serverApp2 = server2.Install(nodes.Get(1));
    serverApp2.Start(Seconds(0.5));
    serverApp2.Stop(Seconds(11.0));

    UdpClientHelper client1(interfaces.GetAddress(1), port1);
    client1.SetAttribute("MaxPackets", UintegerValue(20000));
    client1.SetAttribute("Interval", TimeValue(Seconds(intervalSeconds)));
    client1.SetAttribute("PacketSize", UintegerValue(packetSize));
    ApplicationContainer clientApp1 = client1.Install(nodes.Get(0));
    clientApp1.Start(Seconds(1.0));
    clientApp1.Stop(Seconds(10.0));

    UdpClientHelper client2(interfaces.GetAddress(1), port2);
    client2.SetAttribute("MaxPackets", UintegerValue(20000));
    client2.SetAttribute("Interval", TimeValue(Seconds(intervalSeconds)));
    client2.SetAttribute("PacketSize", UintegerValue(packetSize));
    ApplicationContainer clientApp2 = client2.Install(nodes.Get(0));
    clientApp2.Start(Seconds(1.0));
    clientApp2.Stop(Seconds(10.0));

    p2p.EnablePcapAll("udp-port-trace");

    FlowMonitorHelper flowHelper;
    Ptr<FlowMonitor> monitor = flowHelper.InstallAll();

    Simulator::Stop(Seconds(11.0));
    Simulator::Run();

    monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowHelper.GetClassifier());
    std::cout << "\nUDP results: link delay = " << delay << ", offered rate per flow = "
              << rateMbps << " Mbps\n";
    std::cout << "Destination port | Tx packets | Rx packets | Loss (%) | Throughput (Mbps) | Mean delay (ms)\n";

    for (const auto &entry : monitor->GetFlowStats())
    {
        Ipv4FlowClassifier::FiveTuple tuple = classifier->FindFlow(entry.first);
        if (tuple.destinationPort != port1 && tuple.destinationPort != port2)
            continue;
        const FlowMonitor::FlowStats &stats = entry.second;
        double lossPercent = stats.txPackets == 0 ? 0.0 :
            100.0 * (stats.txPackets - stats.rxPackets) / stats.txPackets;
        double throughput = stats.timeLastRxPacket.GetSeconds() > stats.timeFirstTxPacket.GetSeconds()
            ? stats.rxBytes * 8.0 / (stats.timeLastRxPacket.GetSeconds() - stats.timeFirstTxPacket.GetSeconds()) / 1000000.0
            : 0.0;
        double meanDelay = stats.rxPackets == 0 ? 0.0 : stats.delaySum.GetMilliSeconds() / (double)stats.rxPackets;
        std::cout << tuple.destinationPort << " | " << stats.txPackets << " | " << stats.rxPackets
                  << " | " << lossPercent << " | " << throughput << " | " << meanDelay << "\n";
    }

    monitor->SerializeToXmlFile("udp-port-flowmon.xml", true, true);
    Simulator::Destroy();
    return 0;
}
