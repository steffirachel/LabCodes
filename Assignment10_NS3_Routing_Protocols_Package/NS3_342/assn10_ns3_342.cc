#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/aodv-module.h"
#include "ns3/rip-helper.h"
#include "ns3/output-stream-wrapper.h"
#include <iostream>
#include <sstream>
#include <string>
using namespace ns3;
using namespace std;

string protocolName, previousTable;
uint32_t tableChanges = 0;

void CheckRoutingTables()
{
    ostringstream output;
    Ptr<OutputStreamWrapper> stream = Create<OutputStreamWrapper>(&output);
    for (uint32_t i = 0; i < NodeList::GetNNodes(); i++)
    {
        Ptr<Ipv4> ipv4 = NodeList::GetNode(i)->GetObject<Ipv4>();
        if (ipv4 && ipv4->GetRoutingProtocol())
            ipv4->GetRoutingProtocol()->PrintRoutingTable(stream);
    }
    istringstream lines(output.str());
    string line, currentTable;
    while (getline(lines, line))
        if (line.find("Time:") == string::npos)
            currentTable += line + "\n";
    if (currentTable != previousTable)
    {
        tableChanges++;
        previousTable = currentTable;
    }
    if (Simulator::Now() < Seconds(24))
        Simulator::Schedule(Seconds(1), &CheckRoutingTables);
}

void DisableLink(Ptr<NetDevice> d1, Ptr<NetDevice> d2)
{
    Ptr<Ipv4> ip1 = d1->GetNode()->GetObject<Ipv4>();
    Ptr<Ipv4> ip2 = d2->GetNode()->GetObject<Ipv4>();
    int32_t i1 = ip1->GetInterfaceForDevice(d1);
    int32_t i2 = ip2->GetInterfaceForDevice(d2);
    if (i1 >= 0) ip1->SetDown(i1);
    if (i2 >= 0) ip2->SetDown(i2);
    cout << "\n*** Link failure at " << Simulator::Now().GetSeconds()
         << " seconds ***\n";
    if (protocolName == "OSPF-approx")
        Ipv4GlobalRoutingHelper::RecomputeRoutingTables();
}

void RunScenario(string protocol, string condition)
{
    protocolName = protocol;
    previousTable = "";
    tableChanges = 0;
    cout << "\n====================================\nProtocol: " << protocol
         << "\nCondition: " << condition
         << "\n====================================\n";

    NodeContainer nodes;
    nodes.Create(6);
    Ptr<Node> s1 = nodes.Get(0), r1 = nodes.Get(1), r2 = nodes.Get(2);
    Ptr<Node> r3 = nodes.Get(3), dst = nodes.Get(4), s2 = nodes.Get(5);

    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("5Mbps"));
    csma.SetChannelAttribute("Delay", StringValue("2ms"));
    NetDeviceContainer l1 = csma.Install(NodeContainer(s1, r1));
    NetDeviceContainer l2 = csma.Install(NodeContainer(r1, r2));
    NetDeviceContainer l3 = csma.Install(NodeContainer(r2, r3));
    NetDeviceContainer l4 = csma.Install(NodeContainer(r3, dst));
    NetDeviceContainer l5 = csma.Install(NodeContainer(r1, r3));
    NetDeviceContainer l6 = csma.Install(NodeContainer(s2, r2));

    InternetStackHelper internet;
    if (protocol == "RIP")
    {
        RipHelper rip;
        Ipv4ListRoutingHelper list;
        list.Add(rip, 0);
        internet.SetRoutingHelper(list);
    }
    else if (protocol == "AODV")
    {
        AodvHelper aodv;
        internet.SetRoutingHelper(aodv);
    }
    else
    {
        Ipv4GlobalRoutingHelper global;
        internet.SetRoutingHelper(global);
    }
    internet.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0"); address.Assign(l1);
    address.SetBase("10.1.2.0", "255.255.255.0"); address.Assign(l2);
    address.SetBase("10.1.3.0", "255.255.255.0"); address.Assign(l3);
    address.SetBase("10.1.4.0", "255.255.255.0");
    Ipv4InterfaceContainer dstIf = address.Assign(l4);
    address.SetBase("10.1.5.0", "255.255.255.0"); address.Assign(l5);
    address.SetBase("10.1.6.0", "255.255.255.0"); address.Assign(l6);
    Ipv4Address destination = dstIf.GetAddress(1);

    if (protocol == "OSPF-approx")
        Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    UdpServerHelper server1(9);
    ApplicationContainer sink1 = server1.Install(dst);
    sink1.Start(Seconds(0)); sink1.Stop(Seconds(24));
    UdpServerHelper server2(10);
    ApplicationContainer sink2 = server2.Install(dst);
    sink2.Start(Seconds(0)); sink2.Stop(Seconds(24));

    OnOffHelper src1("ns3::UdpSocketFactory", InetSocketAddress(destination, 9));
    src1.SetConstantRate(DataRate("2Mbps"), 512);
    ApplicationContainer app1 = src1.Install(s1);
    app1.Start(Seconds(3)); app1.Stop(Seconds(24));

    if (condition == "congestion")
    {
        OnOffHelper src2("ns3::UdpSocketFactory", InetSocketAddress(destination, 10));
        src2.SetConstantRate(DataRate("8Mbps"), 1024);
        ApplicationContainer app2 = src2.Install(s2);
        app2.Start(Seconds(3)); app2.Stop(Seconds(24));
    }
    if (condition == "failure")
        Simulator::Schedule(Seconds(12), &DisableLink, l3.Get(0), l3.Get(1));

    FlowMonitorHelper fm;
    Ptr<FlowMonitor> monitor = fm.InstallAll();
    Simulator::Schedule(Seconds(1), &CheckRoutingTables);
    Simulator::Stop(Seconds(25));
    Simulator::Run();
    monitor->CheckForLostPackets();

    Ptr<Ipv4FlowClassifier> classifier =
        DynamicCast<Ipv4FlowClassifier>(fm.GetClassifier());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();
    uint64_t sent = 0, received = 0;
    double delayTotal = 0.0;
    for (auto it = stats.begin(); it != stats.end(); ++it)
    {
        Ipv4FlowClassifier::FiveTuple tuple = classifier->FindFlow(it->first);
        if (tuple.protocol == 17 &&
            (tuple.destinationPort == 9 || tuple.destinationPort == 10))
        {
            sent += it->second.txPackets;
            received += it->second.rxPackets;
            delayTotal += it->second.delaySum.GetSeconds();
        }
    }
    double pdr = sent ? 100.0 * received / sent : 0.0;
    double delayMs = received ? 1000.0 * delayTotal / received : 0.0;
    cout << "\nPackets sent: " << sent
         << "\nPackets received: " << received
         << "\nPDR: " << pdr << "%"
         << "\nAverage end-to-end delay: " << delayMs << " ms"
         << "\nRouting-table changes detected: " << tableChanges << "\n";
    Simulator::Destroy();
}

int main()
{
    string protocols[] = {"RIP", "AODV", "OSPF-approx"};
    string conditions[] = {"normal", "congestion", "failure"};
    cout << "NS-3 Routing Protocol Comparison\n";
    cout << "OSPF-approx means Global Routing, not real OSPF.\n";
    for (int p = 0; p < 3; p++)
        for (int c = 0; c < 3; c++)
            RunScenario(protocols[p], conditions[c]);
    cout << "\nAll simulations completed.\n";
    return 0;
}
