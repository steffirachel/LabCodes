# Assignment 7 — UDP Traffic and Port-Based Application Delivery

## What the question asks
Create two nodes connected by a point-to-point link. Send UDP traffic from Node 1 to Node 2 using two server ports (5000 and 6000). Change link latency to 2, 5, 10 and 20 ms, compare throughput, and capture packets for Wireshark.

## What this program does
- Creates two nodes and a 10 Mbps point-to-point link.
- Installs the Internet stack and assigns IP addresses.
- Installs UDP servers on ports 5000 and 6000 on the receiving node.
- Starts two UDP clients on the sending node. Each client targets a different port.
- Uses FlowMonitor to print transmitted packets, received packets, packet-loss percentage, throughput and mean delay.
- Enables PCAP tracing and writes a FlowMonitor XML file.

## Run it
From the directory containing the NS-3 `ns3` script:

```bash
./ns3 run scratch/udpns3
./ns3 run "scratch/udpns3 --delay=2ms --rateMbps=1"
./ns3 run "scratch/udpns3 --delay=5ms --rateMbps=1"
./ns3 run "scratch/udpns3 --delay=10ms --rateMbps=1"
./ns3 run "scratch/udpns3 --delay=20ms --rateMbps=1"
```

The `--delay` value changes link latency. `--rateMbps` is the offered rate for each flow. Keep the rate fixed when studying latency so that one variable changes at a time.

## Output: what each field means
- **Destination port:** 5000 or 6000 identifies which UDP server should receive the packet.
- **Tx packets:** packets recorded as transmitted for that flow.
- **Rx packets:** packets received for that flow.
- **Loss (%):** `(Tx - Rx) / Tx × 100`.
- **Throughput (Mbps):** received bits divided by the measured flow duration.
- **Mean delay (ms):** average packet delay reported by FlowMonitor.
- **PCAP:** packet-level capture for inspection in Wireshark.
- **FlowMonitor XML:** detailed flow statistics in machine-readable form.

## How to inspect the ports in Wireshark
Open the generated `udp-port-trace-*.pcap` file. Use this display filter:

```text
udp.port == 5000 || udp.port == 6000
```

Select a packet and inspect **User Datagram Protocol → Destination Port**. Port 5000 and port 6000 distinguish the two applications even though both use the same destination IP address.

## Observation table
Fill in the template `observations_template.csv` using the actual terminal results for each latency. Record each flow separately.

## Inference
The destination port identifies the receiving application. Changing propagation delay usually increases end-to-end delay. Throughput may stay similar when the offered rate is well below the link capacity; it is not guaranteed to fall just because latency increases. Use your measured values to state the actual trend.

## Limitations
The code uses `UdpServer`/`UdpClient` applications and FlowMonitor. FlowMonitor delay is a simulation metric; it is not a physical measurement. Run the experiment for every requested delay rather than predicting numeric results.
