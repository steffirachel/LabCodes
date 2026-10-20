# Assignment 8 — Simulation and Performance Analysis of TCP and UDP Traffic Using NS-3

## What the question asks
Build four end nodes connected through two intermediate routers. Node 0 sends TCP to Node 3; Node 1 sends UDP to Node 2. Access links use 10 Mbps and 2 ms. The bottleneck defaults to 2 Mbps and 10 ms, with a 50-packet DropTail queue. Simulate for 20 seconds and vary the bottleneck bandwidth (2, 5, 10 Mbps). Optional extensions vary UDP rate, queue size and propagation delay.

## What this program does
- Creates six nodes: four end nodes and two routers.
- Connects each end node to its router using 10 Mbps / 2 ms links.
- Connects the routers using the configurable bottleneck link.
- Sets the bottleneck queue limit, installs IP, and populates routes.
- Sends TCP data from Node 0 to Node 3 and UDP data from Node 1 to Node 2.
- Uses a 1024-byte TCP send size and 512-byte UDP packets.
- Runs for 20 seconds and prints FlowMonitor packet loss, throughput and mean delay.
- Writes PCAP captures and a FlowMonitor XML file.

## Run the required bottleneck-bandwidth experiment
From the NS-3 directory:

```bash
./ns3 run "scratch/assign8 --bottleneckRate=2Mbps"
./ns3 run "scratch/assign8 --bottleneckRate=5Mbps"
./ns3 run "scratch/assign8 --bottleneckRate=10Mbps"
```

Optional experiments:

```bash
# UDP offered rate
./ns3 run "scratch/assign8 --udpRateMbps=0.5"
./ns3 run "scratch/assign8 --udpRateMbps=1"
./ns3 run "scratch/assign8 --udpRateMbps=2"
./ns3 run "scratch/assign8 --udpRateMbps=5"

# Queue size
./ns3 run "scratch/assign8 --queuePackets=20"
./ns3 run "scratch/assign8 --queuePackets=50"
./ns3 run "scratch/assign8 --queuePackets=100"

# Bottleneck delay
./ns3 run "scratch/assign8 --bottleneckDelay=5ms"
./ns3 run "scratch/assign8 --bottleneckDelay=10ms"
./ns3 run "scratch/assign8 --bottleneckDelay=20ms"
./ns3 run "scratch/assign8 --bottleneckDelay=50ms"
```

Use one parameter change per comparison and keep the other settings fixed.

## Output: what each field means
- **Tx / Rx:** transmitted and received packets.
- **Loss (%):** `(Tx - Rx) / Tx × 100`.
- **Throughput:** received bits per second converted to Mbps.
- **Mean delay:** average end-to-end packet delay from FlowMonitor.
- **PCAP files:** packet captures, especially for the bottleneck link.
- **FlowMonitor XML:** detailed per-flow information.

## Observation and inference
Use `observations_template.csv`. For bandwidth, increasing the bottleneck capacity generally lets more traffic pass and can reduce congestion. A smaller queue can drop packets sooner but may limit queueing delay; a larger queue can reduce some drops while allowing larger delays. These are expected tendencies, not guaranteed measured outcomes.

## Requirement caveat
The assignment asks for TCP retransmission rate. This source prints FlowMonitor packet statistics but does not calculate a dedicated TCP retransmission count/rate. Do not report FlowMonitor packet loss as TCP retransmissions; they are different metrics. If your instructor strictly requires retransmission rate, it must be added with a version-appropriate TCP trace or packet-analysis method.
