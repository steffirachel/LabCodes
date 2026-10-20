# Assignment 9 — TCP Congestion Control for an Intelligent Transportation System

## What the question asks
Model five vehicles sending traffic to a traffic-control server through a roadside router. Compare TCP Reno, TCP Cubic and TCP BBR with respect to throughput, delay, packet loss, fairness and congestion window. Explain why low delay and reliable communication matter for intelligent transportation.

## What this program does
- Creates five vehicle nodes, a roadside router and a server.
- Gives each vehicle a separate access link and shares a configurable road-to-server bottleneck.
- Starts a TCP sink and bulk sender for each vehicle.
- Allows the TCP implementation to be selected using a command-line parameter.
- Prints FlowMonitor throughput, packet loss and mean delay per flow.
- Calculates Jain's fairness index across the observed flows.
- Writes congestion-window changes to `assign9-cwnd.csv` when the trace is supported and creates PCAP and FlowMonitor XML files.

## Run Reno and Cubic
```bash
./ns3 run "scratch/assign9 --tcpType=ns3::TcpNewReno --bottleneckRate=2Mbps"
./ns3 run "scratch/assign9 --tcpType=ns3::TcpCubic --bottleneckRate=2Mbps"
```

Try BBR only if your NS-3 version provides its TypeId:
```bash
./ns3 run "scratch/assign9 --tcpType=ns3::TcpBbr --bottleneckRate=2Mbps"
```

If the program says the TypeId is unavailable, record that your installation does not provide that TCP implementation. Do not label Reno or Cubic results as BBR results.

## Output: what each field means
- **Tx / Rx:** transmitted and received packets.
- **Loss (%):** packet loss recorded by FlowMonitor.
- **Throughput (Mbps):** delivered data rate for each flow.
- **Mean delay (ms):** average measured flow delay.
- **Jain fairness index:** ranges from 0 to 1; a value closer to 1 means the measured flows received more equal throughput.
- **Congestion-window CSV:** timestamp and old/new congestion-window sizes in bytes.
- **PCAP:** packets captured on the road-to-server link.

## How to compare the algorithms
Run the same topology and bottleneck rate for every supported TCP algorithm. Record the measured values in `observations_template.csv`. Compare throughput, delay, loss, fairness and the congestion-window trace. Do not assume a variant is always best: the result depends on the topology, traffic, NS-3 version and configuration.

## Why low delay and reliability matter
Vehicle safety messages such as collision warnings or sudden-braking alerts may need to arrive quickly. Late or missing messages can make a warning less useful. Reliable delivery matters for receiving complete and timely information; however, not every traffic flow has the same deadline or reliability requirement.

## Limitations
BBR is version-dependent and may not be present. The supplied code attempts to connect to a congestion-window trace; exact trace behavior can vary by NS-3 version. The code does not directly measure a dedicated retransmission-rate metric.
