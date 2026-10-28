#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/aodv-helper.h"
#include <fstream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("RoutingComparison");

int main(int argc,char*argv[]) {
    std::string pr="rip",cd="normal",out="routing_results.csv";
    uint16_t port=5000;
    double tp=0,ds=0,tx=0,rx=0,off,pdr,dly,tr,loss;
    int nf=0;
    bool hdr;
    NodeContainer src,r,dst;
    InternetStackHelper net;
    CsmaHelper csma;
    Ipv4AddressHelper addr;
    Ipv4InterfaceContainer s0r0,s1r0,r0r1,r0r2,r1r2,r1d0,r2d1;
    FlowMonitorHelper fm;
    Ptr<FlowMonitor> mon;
    Ptr<Ipv4FlowClassifier> cl;
    std::map<FlowId,FlowMonitor::FlowStats> st;
    Ipv4FlowClassifier::FiveTuple t5;

    CommandLine cmd(__FILE__);
    cmd.AddValue("protocol","Routing protocol (rip/ospf/aodv)",pr);
    cmd.AddValue("condition","Network condition (normal/congestion/failure)",cd);
    cmd.AddValue("out","Output CSV file",out);
    cmd.Parse(argc,argv);

    if(pr!="rip"&&pr!="ospf"&&pr!="aodv") {
        std::cout<<"Unknown protocol: "<<pr<<"\n";
        return 1;
    }

    src.Create(2);
    r.Create(3);
    dst.Create(2);

    if(pr=="rip") {
        RipHelper rip;
        net.SetRoutingHelper(rip);
    } else if(pr=="aodv") {
        AodvHelper aodv;
        net.SetRoutingHelper(aodv);
    } else {
        Config::SetDefault("ns3::Ipv4GlobalRouting::RespondToInterfaceEvents",BooleanValue(true));
    }
    net.Install(src);
    net.Install(r);
    net.Install(dst);

    csma.SetChannelAttribute("DataRate",StringValue("10Mbps"));
    csma.SetChannelAttribute("Delay",StringValue("2ms"));

    auto link=[&](Ptr<Node> a,Ptr<Node> b,const char*sub) {
        NodeContainer nc(a,b);
        NetDeviceContainer d=csma.Install(nc);
        addr.SetBase(sub,"255.255.255.0");
        return addr.Assign(d);
    };

    s0r0=link(src.Get(0),r.Get(0),"10.1.1.0");
    s1r0=link(src.Get(1),r.Get(0),"10.1.2.0");
    r0r1=link(r.Get(0),r.Get(1),"10.1.3.0");
    r0r2=link(r.Get(0),r.Get(2),"10.1.4.0");
    r1d0=link(r.Get(1),dst.Get(0),"10.1.5.0");
    r2d1=link(r.Get(2),dst.Get(1),"10.1.6.0");
    r1r2=link(r.Get(1),r.Get(2),"10.1.7.0");

    if(pr=="ospf") Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    for(uint16_t p=port;p<=port+2;p++) {
        PacketSinkHelper sk("ns3::UdpSocketFactory",InetSocketAddress(Ipv4Address::GetAny(),p));
        ApplicationContainer c=sk.Install(dst);
        c.Start(Seconds(1.0));
        c.Stop(Seconds(31.0));
    }

    auto cbr=[&](Ptr<Node> n,Address a,const char*rt) {
        OnOffHelper h("ns3::UdpSocketFactory",a);
        h.SetAttribute("DataRate",StringValue(rt));
        h.SetAttribute("PacketSize",UintegerValue(512));
        h.SetAttribute("OnTime",StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        h.SetAttribute("OffTime",StringValue("ns3::ConstantRandomVariable[Constant=0]"));
        ApplicationContainer c=h.Install(n);
        c.Start(Seconds(5.0));
        c.Stop(Seconds(30.0));
    };

    cbr(src.Get(0),InetSocketAddress(r1d0.GetAddress(1),port),"1Mbps");
    cbr(src.Get(1),InetSocketAddress(r2d1.GetAddress(1),port),"1Mbps");

    if(cd=="congestion") {
        cbr(src.Get(0),InetSocketAddress(r1d0.GetAddress(1),port+1),"9Mbps");
        cbr(src.Get(1),InetSocketAddress(r2d1.GetAddress(1),port+2),"9Mbps");
    }

    if(cd=="failure") {
        Simulator::Schedule(Seconds(10.0),&Ipv4::SetDown,r0r1.Get(0).first,r0r1.Get(0).second);
        Simulator::Schedule(Seconds(10.0),&Ipv4::SetDown,r0r1.Get(1).first,r0r1.Get(1).second);
    }

    mon=fm.InstallAll();
    Simulator::Stop(Seconds(31.0));
    Simulator::Run();

    mon->CheckForLostPackets();
    cl=DynamicCast<Ipv4FlowClassifier>(fm.GetClassifier());
    st=mon->GetFlowStats();

    std::cout<<"\nProtocol: "<<pr<<" | Condition: "<<cd<<"\n";
    std::cout<<"==========================================\n";

    for(auto&s:st) {
        t5=cl->FindFlow(s.first);
        if(t5.protocol!=17||t5.destinationPort!=port) continue;
        nf++;
        double ftp=s.second.rxBytes*8.0/25.0/1e6;
        double fd=s.second.rxPackets>0?s.second.delaySum.GetSeconds()/s.second.rxPackets*1000.0:0;
        double fp=s.second.txPackets>0?(double)s.second.rxPackets/s.second.txPackets*100.0:0;
        tp+=ftp;
        tx+=s.second.txPackets;
        rx+=s.second.rxPackets;
        ds+=s.second.delaySum.GetSeconds();
        std::cout<<"Flow "<<nf<<": "<<t5.sourceAddress<<" -> "<<t5.destinationAddress<<"\n";
        std::cout<<"  Throughput : "<<ftp<<" Mbps\n";
        std::cout<<"  Avg Delay  : "<<fd<<" ms\n";
        std::cout<<"  PDR        : "<<fp<<" %\n";
        std::cout<<"  Tx Packets : "<<s.second.txPackets<<"\n";
        std::cout<<"  Rx Packets : "<<s.second.rxPackets<<"\n\n";
    }

    off=nf*1e6*25.0/(512*8.0);
    pdr=tx>0?rx/tx*100.0:0;
    loss=tx>0?100.0-pdr:0;
    dly=rx>0?ds/rx*1000.0:0;
    tr=off>0?tx/off*100.0:0;

    std::cout<<"==========================================\n";
    std::cout<<"Throughput         : "<<tp<<" Mbps\n";
    std::cout<<"Packet Loss        : "<<loss<<" %\n";
    std::cout<<"PDR                : "<<pdr<<" %\n";
    std::cout<<"Transmission Ratio : "<<tr<<" %\n";
    std::cout<<"End-to-End Delay   : "<<dly<<" ms\n";

    std::ifstream chk(out);
    hdr=chk.peek()==std::ifstream::traits_type::eof();
    chk.close();
    std::ofstream o(out,std::ios::app);
    if(hdr) o<<"Protocol,Condition,ThroughputMbps,PacketLossPercent,PDRPercent,TransmissionRatioPercent,EndToEndDelayMs,TxPackets,RxPackets\n";
    o<<pr<<","<<cd<<","<<tp<<","<<loss<<","<<pdr<<","<<tr<<","<<dly<<","<<tx<<","<<rx<<"\n";
    o.close();

    Simulator::Destroy();
    return 0;
}
