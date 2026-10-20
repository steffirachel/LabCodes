# Assignment 7 — UDP Traffic and Port-Based Application Delivery

## What the question asks

- Create Node 1 and Node 2 connected by a point-to-point link.
- Run two UDP client/server flows between the same nodes.
- Flow 1 uses destination port **5000** and Flow 2 uses port **6000**.
- Change the link delay to 2, 5, 10, and 20 ms.
- Measure throughput and use PCAP/Wireshark to verify that the port number identifies the correct server application.

## Files

- `udpns3.cc` — the NS-3 simulation.
- The simulation creates `udp-port-trace-*.pcap` and `udp-port-flowmon.xml` when it runs.

## How the code works

1. Creates two nodes and connects them using `PointToPointHelper`, following the cheat sheet.
2. Installs the Internet stack and assigns addresses from `10.1.1.0/24`.
3. Installs UDP servers on Node 2 at ports 5000 and 6000.
4. Installs two UDP clients on Node 1. Both send 512-byte packets at the selected offered rate (default 1 Mbps per flow).
5. Uses FlowMonitor to calculate transmitted/received packets, packet loss, throughput, and mean delay.
6. Enables PCAP tracing so packets can be checked in Wireshark.

## How to run

Copy `udpns3.cc` into the `scratch` folder of your NS-3 installation. From the NS-3 root folder, run:

```bash
./ns3 run scratch/udpns3.cc
```

Run each required link delay separately:

```bash
./ns3 run 'scratch/udpns3.cc --delay=2ms'
./ns3 run 'scratch/udpns3.cc --delay=5ms'
./ns3 run 'scratch/udpns3.cc --delay=10ms'
./ns3 run 'scratch/udpns3.cc --delay=20ms'
```

Optional: change the offered rate for each flow:

```bash
./ns3 run 'scratch/udpns3.cc --delay=5ms --rateMbps=1'
```

If your NS-3 release uses a different run syntax, use the syntax shown by your installed release's examples. This is an NS-3 source file, not a standalone program to compile with plain `g++`.

## What the result means

The terminal table shows one row per UDP destination port:

- **Tx packets:** packets sent by the flow.
- **Rx packets:** packets received at the destination.
- **Loss (%):** `(Tx - Rx) / Tx × 100`.
- **Throughput:** successfully received bits divided by the measured flow duration, in Mbps.
- **Mean delay:** average one-way packet delay reported by FlowMonitor, in milliseconds.

The two rows should have destination ports 5000 and 6000. They share the same IP addresses, but UDP destination ports distinguish the applications.

## Wireshark check

1. Open one generated `udp-port-trace-*.pcap` file in Wireshark.
2. Use display filter `udp.dstport == 5000` to view Flow 1.
3. Use display filter `udp.dstport == 6000` to view Flow 2.
4. Check the destination-port column/UDP details to verify that the flows use different ports.

## Observation table

Fill this using the numbers printed by your own runs; do not copy guessed values.

| Link delay | Flow 1 throughput (Mbps) | Flow 2 throughput (Mbps) | Flow 1 loss (%) | Flow 2 loss (%) |
|---|---:|---:|---:|---:|
| 2 ms | | | | |
| 5 ms | | | | |
| 10 ms | | | | |
| 20 ms | | | | |

## Inference

With a fixed-rate, one-way UDP sender and a 10 Mbps link, increasing propagation delay alone normally changes packet arrival time more than throughput. Throughput should stay near the offered rate if the link is not congested and packets are not lost. Small differences can occur because of the finite simulation window. The key port-based inference is that port 5000 and port 6000 are separate UDP destinations even though both flows use the same pair of IP addresses.
