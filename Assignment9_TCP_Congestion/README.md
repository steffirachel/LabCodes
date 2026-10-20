# Assignment 9 — TCP Congestion Control Algorithms for a VANET

## What the question asks

Model vehicles sending traffic/camera/road-condition data through a roadside router to a traffic-control server. Compare TCP Reno, TCP Cubic, and TCP BBR for throughput, delay, packet loss, fairness, and congestion window. Explain why low delay and reliable delivery matter in intelligent transportation.

## Files

- `assign9.cc` — NS-3 simulation with five vehicle senders, a roadside router, a server, and a shared configurable bottleneck.
- Run outputs: `assign9-flowmon.xml`, `assign9-cwnd.csv`, and `assign9-road-link-*.pcap`.

## How the code works

1. Creates five vehicle nodes, one roadside router, and one traffic server.
2. Connects each vehicle to the roadside router using a 10 Mbps, 2 ms point-to-point link.
3. Connects the router to the server through a shared bottleneck (default 2 Mbps, 20 ms), so the TCP flows compete for capacity.
4. Runs one TCP BulkSend flow from every vehicle to its own server port.
5. Selects the TCP variant using the `tcpType` command-line argument.
6. FlowMonitor prints packet counts, loss percentage, throughput, and mean delay for each flow.
7. Calculates Jain's fairness index from the measured flow throughputs. A value closer to 1 indicates more equal throughput sharing.
8. Attempts to record TCP congestion-window changes in `assign9-cwnd.csv` and saves PCAP for packet inspection.

## How to run

Copy `assign9.cc` into the `scratch` folder of your NS-3 installation. Run from the NS-3 root folder:

```bash
./ns3 run 'scratch/assign9.cc --tcpType=ns3::TcpNewReno'
./ns3 run 'scratch/assign9.cc --tcpType=ns3::TcpCubic'
./ns3 run 'scratch/assign9.cc --tcpType=ns3::TcpBbr'
```

**Important:** TCP BBR availability depends on the NS-3 version/build. If the third command prints that the TCP type is unavailable, that is a limitation of the installed NS-3 build, not a simulation result. Do not label Reno or Cubic output as BBR output. Use the TCP variants actually supported by your lab installation, or ask your instructor which BBR module/version is installed.

Optional: change the shared link rate consistently across all TCP variants:

```bash
./ns3 run 'scratch/assign9.cc --tcpType=ns3::TcpNewReno --bottleneckRate=5Mbps'
```

Repeat with the same settings for every TCP type so the comparison is fair. Use the syntax supported by your installed NS-3 version if it differs.

## What the output means

- **Throughput:** received data per second for each vehicle flow.
- **Packet loss:** packets reported lost by FlowMonitor divided by packets transmitted.
- **Mean delay:** average one-way delay for received packets.
- **Jain fairness index:** ` (sum of flow throughputs)^2 / (number of flows × sum of squared flow throughputs) `. The value is between 0 and 1 for non-negative throughputs; closer to 1 means more equal sharing.
- **Congestion window:** `assign9-cwnd.csv` records time and old/new congestion-window values if the installed NS-3 trace path is available. Check that the CSV has data rows after the header.

For a detailed TCP retransmission check, open the PCAP file in Wireshark and apply `tcp.analysis.retransmission`.

## Result table to complete

Record measured values separately for each algorithm.

| TCP variant | Aggregate throughput | Mean delay | Packet loss | Jain fairness (0–1) | Congestion-window observation |
|---|---:|---:|---:|---:|---|
| Reno | | | | | |
| Cubic | | | | | |
| BBR (only if supported) | | | | | |

## Inference and real-world justification

Do not assume one algorithm must win every metric. Congestion-control algorithms react differently to congestion and available bandwidth; use the simulation values to explain the observed differences. A shared bottleneck makes the vehicles compete, while the fairness index indicates whether they receive similar shares.

Low delay matters because braking warnings, collision alerts, and road hazards can become useless if delivered too late. Reliability matters because missing a safety message can prevent a vehicle or traffic controller from reacting. High throughput is useful for camera and sensor data, but safety messages also need prompt and dependable delivery. A real VANET includes wireless fading, handovers, and changing vehicle positions; this simplified program models the shared traffic bottleneck and TCP behavior, not a complete wireless vehicular channel.
