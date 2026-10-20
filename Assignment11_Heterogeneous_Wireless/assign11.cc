#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include <fstream>

using namespace ns3;

static std::ofstream signalFile;
static Ptr<FriisPropagationLossModel> signalModel;
static NodeContainer mobileNodes;
static Ptr<Node> gatewayNode;
static double txPowerDbm = 16.0;

static void
RecordSignalStrength(double timeSeconds)
{
    Ptr<MobilityModel> gatewayMobility = gatewayNode->GetObject<MobilityModel>();
    for (uint32_t i = 0; i < mobileNodes.GetN(); ++i)
    {
        Ptr<MobilityModel> mobileMobility = mobileNodes.Get(i)->GetObject<MobilityModel>();
        double distance = mobileMobility->GetDistanceFrom(gatewayMobility);
        double rxPower = signalModel->CalcRxPower(txPowerDbm, mobileMobility, gatewayMobility);
        signalFile << timeSeconds << "," << i << "," << distance << "," << rxPower << "\n";
    }
}

int main(int argc, char *argv[])
{
    double speed = 1.0;
    CommandLine cmd;
    cmd.AddValue("speed", "Random-walk speed in metres/second: 1, 5, or 10", speed);
    cmd.Parse(argc, argv);

    NodeContainer allNodes;
    allNodes.Create(4); // Nodes 0-2 are mobile users; Node 3 is the Wi-Fi gateway.
    mobileNodes.Add(allNodes.Get(0));
    mobileNodes.Add(allNodes.Get(1));
    mobileNodes.Add(allNodes.Get(2));
    gatewayNode = allNodes.Get(3);

    YansWifiChannelHelper channel;
    channel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    channel.AddPropagationLoss("ns3::FriisPropagationLossModel");
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());

    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211g);
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                 "DataMode", StringValue("OfdmRate6Mbps"),
                                 "ControlMode", StringValue("OfdmRate6Mbps"));
    WifiMacHelper mac;
    mac.SetType("ns3::AdhocWifiMac");
    NetDeviceContainer devices = wifi.Install(phy, mac, allNodes);

    // The gateway stays at the centre. Mobile nodes use the RandomWalk model from the cheat sheet.
    MobilityHelper gatewayMobility;
    Ptr<ListPositionAllocator> gatewayPosition = CreateObject<ListPositionAllocator>();
    gatewayPosition->Add(Vector(0.0, 0.0, 0.0));
    gatewayMobility.SetPositionAllocator(gatewayPosition);
    gatewayMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    NodeContainer onlyGateway(gatewayNode);
    gatewayMobility.Install(onlyGateway);

    MobilityHelper moving;
    Ptr<ListPositionAllocator> positions = CreateObject<ListPositionAllocator>();
    positions->Add(Vector(5.0, 0.0, 0.0));
    positions->Add(Vector(10.0, 5.0, 0.0));
    positions->Add(Vector(-8.0, 4.0, 0.0));
    moving.SetPositionAllocator(positions);
    moving.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                            "Bounds", RectangleValue(Rectangle(-50, 50, -50, 50)),
                            "Speed", StringValue("ns3::ConstantRandomVariable[Constant=" + std::to_string(speed) + "]"));
    moving.Install(mobileNodes);

    InternetStackHelper internet;
    internet.Install(allNodes);
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    const uint16_t port = 9000;
    UdpServerHelper server(port);
    ApplicationContainer serverApp = server.Install(gatewayNode);
    serverApp.Start(Seconds(0.5)); serverApp.Stop(Seconds(20.0));

    for (uint32_t i = 0; i < mobileNodes.GetN(); ++i)
    {
        UdpClientHelper client(interfaces.GetAddress(3), port);
        client.SetAttribute("MaxPackets", UintegerValue(2000));
        client.SetAttribute("Interval", TimeValue(MilliSeconds(20)));
        client.SetAttribute("PacketSize", UintegerValue(512));
        ApplicationContainer clientApp = client.Install(mobileNodes.Get(i));
        clientApp.Start(Seconds(1.0)); clientApp.Stop(Seconds(19.0));
    }

    phy.EnablePcapAll("assign11-wifi");
    signalModel = CreateObject<FriisPropagationLossModel>();
    signalModel->SetAttribute("Frequency", DoubleValue(2.4e9));
    signalFile.open("assign11-signal-strength.csv");
    signalFile << "time_seconds,mobile_node,distance_m,estimated_rx_power_dbm\n";
    for (double t = 1.0; t <= 19.0; t += 1.0)
    {
        Simulator::Schedule(Seconds(t), &RecordSignalStrength, t);
    }

    FlowMonitorHelper flowHelper;
    Ptr<FlowMonitor> monitor = flowHelper.InstallAll();
    Simulator::Stop(Seconds(20.0));
    Simulator::Run();
    monitor->CheckForLostPackets();

    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowHelper.GetClassifier());
    std::cout << "\nAssignment 11 Wi-Fi mobility results; speed=" << speed << " m/s\n";
    std::cout << "Flow | Tx | Rx | Loss (%) | Throughput (Mbps) | Mean delay (ms)\n";
    for (const auto &entry : monitor->GetFlowStats())
    {
        Ipv4FlowClassifier::FiveTuple tuple = classifier->FindFlow(entry.first);
        if (tuple.destinationPort != port) continue;
        const FlowMonitor::FlowStats &s = entry.second;
        double loss = s.txPackets == 0 ? 0.0 : 100.0 * (s.txPackets - s.rxPackets) / s.txPackets;
        double duration = s.timeLastRxPacket.GetSeconds() - s.timeFirstTxPacket.GetSeconds();
        double throughput = duration > 0 ? s.rxBytes * 8.0 / duration / 1000000.0 : 0.0;
        double delay = s.rxPackets > 0 ? s.delaySum.GetMilliSeconds() / s.rxPackets : 0.0;
        std::cout << tuple.sourceAddress << " -> gateway:" << tuple.destinationPort << " | "
                  << s.txPackets << " | " << s.rxPackets << " | " << loss << " | "
                  << throughput << " | " << delay << "\n";
    }
    std::cout << "Estimated signal power is written to assign11-signal-strength.csv.\n";
    std::cout << "Note: this program simulates Wi-Fi. Stock ns-3 does not provide a complete Bluetooth stack; see README for honest scope notes.\n";
    monitor->SerializeToXmlFile("assign11-flowmon.xml", true, true);
    signalFile.close();
    Simulator::Destroy();
    return 0;
}
