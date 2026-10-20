# Assignment 8 — Simulation and Performance Analysis of TCP and UDP Traffic

## What the question asks

- Four end nodes communicate through two intermediate routers.
- TCP flow: Node 0 to Node 3.
- UDP flow: Node 1 to Node 2.
- Access links: 10 Mbps and 2 ms.
- Bottleneck link: default 2 Mbps and 10 ms.
- DropTail queue: 50 packets; simulation duration: 20 seconds.
- TCP send size: 1024 bytes; UDP packet size: 512 bytes and default offered rate 1 Mbps.
- Compare packet loss, throughput, delay, and TCP retransmissions. Repeat with bottleneck rates 2, 5, and 10 Mbps. Optional command-line options also allow queue size, UDP rate, and bottleneck delay to change.

## Files

- `assign8.cc` — the NS-3 simulation.
- On running, it creates `assign8-bottleneck-*.pcap` and `assign8-flowmon.xml`.

## How the code works

1. Creates six nodes: four end nodes and two routers.
2. Uses point-to-point helpers for the four access links and the middle bottleneck link.
3. Gives each link its own IPv4 subnet and enables global routing.
4. Installs a TCP BulkSend source at Node 0 and TCP PacketSink at Node 3.
5. Installs a UDP client at Node 1 and UDP server at Node 2.
6. FlowMonitor reports Tx/Rx packets, packet loss, throughput, and mean delay.
7. PCAP captures allow TCP retransmissions to be inspected in Wireshark.

## How to run

Copy `assign8.cc` into your NS-3 `scratch` folder and run these commands from the NS-3 root folder:

```bash
./ns3 run scratch/assign8.cc
./ns3 run 'scratch/assign8.cc --bottleneckRate=5Mbps'
./ns3 run 'scratch/assign8.cc --bottleneckRate=10Mbps'
```

Optional experiments from the assignment:

```bash
# UDP offered rate
./ns3 run 'scratch/assign8.cc --udpRateMbps=0.5'
./ns3 run 'scratch/assign8.cc --udpRateMbps=2'
./ns3 run 'scratch/assign8.cc --udpRateMbps=5'

# Queue size
./ns3 run 'scratch/assign8.cc --queuePackets=20'
./ns3 run 'scratch/assign8.cc --queuePackets=100'

# Bottleneck propagation delay
./ns3 run 'scratch/assign8.cc --bottleneckDelay=5ms'
./ns3 run 'scratch/assign8.cc --bottleneckDelay=20ms'
./ns3 run 'scratch/assign8.cc --bottleneckDelay=50ms'
```

The exact run syntax can vary slightly by NS-3 version. This is NS-3 code and must be run inside an NS-3 source/build directory, not with plain `g++` alone.

## Understanding the printed values

- **Tx/Rx:** packets transmitted and received for the flow.
- **Loss (%):** percentage of transmitted packets not received according to FlowMonitor.
- **Throughput (Mbps):** received bytes converted to bits per second over the measured receive interval.
- **Mean delay (ms):** FlowMonitor's total packet delay divided by received packets.

TCP retransmission rate is not a standard FlowMonitor summary field. Use the generated PCAP to measure it:

1. Open `assign8-bottleneck-*.pcap` in Wireshark.
2. Apply `tcp.analysis.retransmission` to show detected TCP retransmissions.
3. Use Wireshark's packet count for retransmissions and total TCP packets/segments for the same capture.
4. State the method you used. One simple percentage is `detected retransmitted TCP segments / total TCP segments × 100`. Keep the denominator definition consistent across all runs.

## Observation table

Fill in actual values from each run.

| Bottleneck rate | TCP throughput | UDP throughput | Packet loss | Mean delay | TCP retransmission rate (Wireshark) |
|---|---:|---:|---:|---:|---:|
| 2 Mbps | | | | | |
| 5 Mbps | | | | | |
| 10 Mbps | | | | | |

## Inference

At 2 Mbps, the bottleneck has the least capacity, so combined traffic is more likely to queue. Increasing the bottleneck rate generally gives packets more room to pass, which can increase throughput and reduce queue-related loss and delay. A larger queue may reduce immediate drops but can increase waiting time. A larger propagation delay increases end-to-end delay even if the link rate is unchanged. Actual results depend on TCP congestion control, traffic timing, and the finite simulation interval, so use your measured output rather than inventing values.
