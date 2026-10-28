#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

using namespace ns3;
using namespace std;

ofstream cwndFile;

// Record congestion-window changes
static void CwndChange(uint32_t oldCwnd, uint32_t newCwnd)
{
    if (cwndFile.is_open())
    {
        cwndFile << Simulator::Now().GetSeconds() << ","
                 << oldCwnd << ","
                 << newCwnd << "\n";
    }
}

// Connect the trace safely after TCP sockets are created
static void ConnectCwndTrace()
{
    bool connected = Config::ConnectWithoutContextFailSafe(
        "/NodeList/*/$ns3::TcpL4Protocol/SocketList/*/CongestionWindow",
        MakeCallback(&CwndChange));

    if (!connected)
        cout << "Warning: Congestion window trace unavailable.\n";
}

struct Result
{
    string tcp;
    bool ran;
    double throughput;
    double delay;
    uint64_t lost;
    double fairness;
};

// Run one TCP algorithm
static Result RunOne(string tcp)
{
    Result result = {tcp, false, 0.0, 0.0, 0, 0.0};

    string typeName = "ns3::Tcp" + tcp;
    TypeId tcpType;

    // Check whether this TCP algorithm exists
    if (!TypeId::LookupByNameFailSafe(typeName, &tcpType))
    {
        cout << "\nSkipping " << tcp
             << ": not available in this NS-3 installation.\n";
        return result;
    }

    Config::SetDefault(
        "ns3::TcpL4Protocol::SocketType",
        TypeIdValue(tcpType));

    // Create five vehicles, one router and one server
    NodeContainer vehicles;
    vehicles.Create(5);

    Ptr<Node> router = CreateObject<Node>();
    Ptr<Node> server = CreateObject<Node>();

    NodeContainer allNodes;
    allNodes.Add(vehicles);
    allNodes.Add(router);
    allNodes.Add(server);

    InternetStackHelper internet;
    internet.Install(allNodes);

    // Connect each vehicle to the router
    PointToPointHelper access;
    access.SetDeviceAttribute(
        "DataRate", StringValue("10Mbps"));
    access.SetChannelAttribute(
        "Delay", StringValue("2ms"));

    Ipv4AddressHelper address;

    for (uint32_t i = 0; i < 5; i++)
    {
        NodeContainer pair(vehicles.Get(i), router);
        NetDeviceContainer devices = access.Install(pair);

        string network = "10.1." + to_string(i + 1) + ".0";

        address.SetBase(
            network.c_str(),
            "255.255.255.0");

        address.Assign(devices);
    }

    // Shared bottleneck between router and server
    PointToPointHelper bottleneck;

    bottleneck.SetDeviceAttribute(
        "DataRate", StringValue("2Mbps"));
    bottleneck.SetChannelAttribute(
        "Delay", StringValue("20ms"));

    bottleneck.SetQueue(
        "ns3::DropTailQueue",
        "MaxSize", StringValue("10p"));

    NetDeviceContainer devices =
        bottleneck.Install(router, server);

    address.SetBase(
        "10.2.0.0",
        "255.255.255.0");

    Ipv4InterfaceContainer interfaces =
        address.Assign(devices);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    Ipv4Address serverIP = interfaces.GetAddress(1);

    // Create one TCP sender for each vehicle
    for (uint32_t i = 0; i < 5; i++)
    {
        uint16_t port = 5000 + i;

        PacketSinkHelper sink(
            "ns3::TcpSocketFactory",
            InetSocketAddress(
                Ipv4Address::GetAny(), port));

        ApplicationContainer receiver =
            sink.Install(server);

        receiver.Start(Seconds(0.0));
        receiver.Stop(Seconds(20.0));

        BulkSendHelper source(
            "ns3::TcpSocketFactory",
            InetSocketAddress(serverIP, port));

        source.SetAttribute(
            "SendSize", UintegerValue(1024));

        ApplicationContainer sender =
            source.Install(vehicles.Get(i));

        sender.Start(Seconds(1.0));
        sender.Stop(Seconds(19.0));
    }

    // Save congestion-window data
    string filename = "cwnd_" + tcp + ".csv";

    cwndFile.open(filename.c_str());
    cwndFile << "Time,OldCwnd,NewCwnd\n";

    // TCP sockets are created when senders start
    Simulator::Schedule(
        Seconds(1.1), &ConnectCwndTrace);

    // Monitor network performance
    FlowMonitorHelper flowHelper;
    Ptr<FlowMonitor> monitor = flowHelper.InstallAll();

    Simulator::Stop(Seconds(20.0));
    Simulator::Run();

    monitor->CheckForLostPackets();

    Ptr<Ipv4FlowClassifier> classifier =
        DynamicCast<Ipv4FlowClassifier>(
            flowHelper.GetClassifier());

    FlowMonitor::FlowStatsContainer stats =
        monitor->GetFlowStats();

    double flowThroughput[5] = {0, 0, 0, 0, 0};
    double flowDelay[5] = {0, 0, 0, 0, 0};
    uint64_t flowReceived[5] = {0, 0, 0, 0, 0};
    uint64_t flowLost[5] = {0, 0, 0, 0, 0};

    // Read statistics for the five data flows
    for (auto it = stats.begin(); it != stats.end(); ++it)
    {
        Ipv4FlowClassifier::FiveTuple tuple =
            classifier->FindFlow(it->first);

        if (tuple.destinationAddress != serverIP ||
            tuple.protocol != 6)
            continue;

        if (tuple.destinationPort < 5000 ||
            tuple.destinationPort > 5004)
            continue;

        uint32_t index = tuple.destinationPort - 5000;

        flowLost[index] += it->second.lostPackets;

        if (it->second.rxPackets > 0)
        {
            flowThroughput[index] +=
                it->second.rxBytes * 8.0 /
                18.0 / 1000000.0;

            flowDelay[index] +=
                it->second.delaySum.GetSeconds();

            flowReceived[index] +=
                it->second.rxPackets;
        }
    }

    // Print individual flow results
    cout << "\n===== TCP " << tcp << " =====\n";
    cout << "Vehicle\tThroughput(Mbps)\tDelay(ms)\tLost packets\n";

    double totalThroughput = 0.0;
    double totalDelaySeconds = 0.0;
    uint64_t totalReceived = 0;
    uint64_t totalLost = 0;
    double sumSquares = 0.0;

    for (uint32_t i = 0; i < 5; i++)
    {
        double delayMs = 0.0;

        if (flowReceived[i] > 0)
        {
            delayMs = flowDelay[i] /
                      flowReceived[i] * 1000.0;

            totalDelaySeconds += flowDelay[i];
            totalReceived += flowReceived[i];
        }

        cout << i + 1 << "\t"
             << flowThroughput[i] << "\t\t\t"
             << delayMs << "\t\t"
             << flowLost[i] << "\n";

        totalThroughput += flowThroughput[i];
        sumSquares += flowThroughput[i] *
                      flowThroughput[i];

        totalLost += flowLost[i];
    }

    // Jain's fairness index
    double fairness = 0.0;

    if (sumSquares > 0)
    {
        fairness = totalThroughput * totalThroughput /
                   (5.0 * sumSquares);
    }

    double averageDelay = 0.0;

    if (totalReceived > 0)
    {
        averageDelay =
            totalDelaySeconds / totalReceived * 1000.0;
    }

    cout << "\nTotal throughput: "
         << totalThroughput << " Mbps\n";

    cout << "Average delay: "
         << averageDelay << " ms\n";

    cout << "Total lost packets: "
         << totalLost << "\n";

    cout << "Jain fairness index: "
         << fairness << "\n";

    cout << "Congestion window file: "
         << filename << "\n";

    cwndFile.close();

    result.ran = true;
    result.throughput = totalThroughput;
    result.delay = averageDelay;
    result.lost = totalLost;
    result.fairness = fairness;

    Simulator::Destroy();

    return result;
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    vector<string> variants = {
        "NewReno", "Cubic", "Bbr"
    };

    vector<Result> results;

    cout << "VANET TCP Congestion Control Comparison\n";
    cout << "Five vehicles -> router -> traffic server\n";
    cout << "Bottleneck: 2 Mbps, delay: 20 ms\n";
    cout << "Simulation time: 20 seconds\n";

    // Run each available TCP variant automatically
    for (const string &tcp : variants)
    {
        results.push_back(RunOne(tcp));
    }

    // Final comparison table
    cout << "\n\n===== FINAL COMPARISON =====\n";

    cout << "TCP\t\tThroughput(Mbps)\tDelay(ms)"
         << "\tLost\tFairness\n";

    for (const Result &r : results)
    {
        if (!r.ran)
        {
            cout << r.tcp
                 << "\t\tNot available in this NS-3 build\n";
            continue;
        }

        cout << r.tcp << "\t\t"
             << r.throughput << "\t\t"
             << r.delay << "\t\t"
             << r.lost << "\t"
             << r.fairness << "\n";
    }

    cout << "\nBBR must be supported by your NS-3 installation "
         << "to produce BBR results.\n";

    return 0;
}
