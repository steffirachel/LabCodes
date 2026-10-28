# Assignment 10: Comparative Evaluation of Routing Protocols Using NS-3

## 1. Aim

To compare the performance of different routing approaches — RIP, AODV, and Global Routing (labelled OSPF in this program) — under normal, congestion, and link-failure conditions using NS-3.

## 2. Software Required

- Ubuntu or WSL Ubuntu
- NS-3 installed and configured
- A C++ compiler supported by the installed NS-3 version

This experiment does not require real routers or physical network devices. The network is simulated in NS-3.

## 3. What Is Routing?

Routing is the process of selecting a path through which data packets travel from a source to a destination.

For example, when Node 0 sends packets to a destination, intermediate router nodes forward the packets along a network path.

A routing protocol helps determine which path should be used.

### Routing approaches used

**RIP (Routing Information Protocol)**

RIP exchanges routing information between routers and uses hop count to select routes.

**AODV (Ad hoc On-Demand Distance Vector)**

AODV discovers routes when they are needed. It is commonly used in mobile ad hoc network simulations.

**Global Routing**

NS-3 calculates routes using knowledge of the network topology.

**Important note:** Although the program accepts `ospf` as a protocol name, it actually uses `Ipv4GlobalRoutingHelper`. This is NS-3 Global Routing, not a full implementation of the OSPF protocol.

## 4. Network Topology

The program creates:

- 2 source nodes
- 3 intermediate router nodes
- 2 destination nodes

There are seven simulated CSMA links:

1. Source 0 to Router 0
2. Source 1 to Router 0
3. Router 0 to Router 1
4. Router 0 to Router 2
5. Router 1 to Destination 0
6. Router 2 to Destination 1
7. Router 1 to Router 2

Each link is configured with:

- Data rate: 10 Mbps
- Channel delay: 2 ms
- Subnet mask: 255.255.255.0

The alternate link between Router 1 and Router 2 provides another possible connection between the router branches.

## 5. Network Conditions

The program supports three conditions.

### A. Normal

Two source nodes send UDP traffic at 1 Mbps each.

This represents normal network operation.

### B. Congestion

Two additional UDP traffic flows are introduced at 9 Mbps each.

This increases the offered traffic and allows us to observe changes in throughput, packet loss, PDR, and delay.

The exact amount of congestion depends on the topology, routing behaviour, and traffic flows.

### C. Failure

The program schedules both ends of the Router 0–Router 1 link to be disabled at 10 seconds.

This tests how the network behaves after that link fails.

The outcome depends on whether an alternative route is available and whether the selected routing approach can use it.

## 6. How the Program Works

1. Reads the selected routing protocol and network condition from the command line.
2. Creates the source, router, and destination nodes.
3. Installs the selected routing approach.
4. Creates the seven CSMA links and assigns IP addresses.
5. Installs UDP packet-sink applications at the destination nodes.
6. Starts UDP traffic from the source nodes at 5 seconds.
7. Adds extra traffic if congestion is selected.
8. Disables the specified link at 10 seconds if failure is selected.
9. Runs the simulation until 31 seconds.
10. Uses FlowMonitor to collect packet statistics.
11. Calculates throughput, packet loss, PDR, transmission ratio, and average delay.
12. Prints the results to the terminal and appends a summary row to a CSV file.

## 7. How to Run the Program

### Step 1: Open Ubuntu

Open your Ubuntu terminal.

### Step 2: Go to the NS-3 directory

```bash
cd ~/ns-3
```

### Step 3: Save the C++ program

Save the supplied program as:

```text
~/ns-3/scratch/assn10.cc
```

If you use another filename, replace `assn10` in the commands below with that filename.

### Step 4: Build the program

```bash
./ns3 build
```

If the build succeeds, continue to the next step.

If another scratch file causes the build to fail, fix that file's compilation error as well. NS-3 may compile multiple scratch programs during a build.

### Step 5: Run RIP under normal conditions

```bash
./ns3 run "scratch/assn10 --protocol=rip --condition=normal"
```

### Step 6: Run RIP under congestion

```bash
./ns3 run "scratch/assn10 --protocol=rip --condition=congestion"
```

### Step 7: Run RIP under link failure

```bash
./ns3 run "scratch/assn10 --protocol=rip --condition=failure"
```

### Step 8: Run AODV

Normal:

```bash
./ns3 run "scratch/assn10 --protocol=aodv --condition=normal"
```

Congestion:

```bash
./ns3 run "scratch/assn10 --protocol=aodv --condition=congestion"
```

Failure:

```bash
./ns3 run "scratch/assn10 --protocol=aodv --condition=failure"
```

### Step 9: Run Global Routing

The program calls this option `ospf`, but it uses NS-3 Global Routing internally.

Normal:

```bash
./ns3 run "scratch/assn10 --protocol=ospf --condition=normal"
```

Congestion:

```bash
./ns3 run "scratch/assn10 --protocol=ospf --condition=congestion"
```

Failure:

```bash
./ns3 run "scratch/assn10 --protocol=ospf --condition=failure"
```

## 8. Command-Line Options

| Option | Meaning |
|---|---|
| `--protocol=rip` | Selects RIP |
| `--protocol=aodv` | Selects AODV |
| `--protocol=ospf` | Selects the program's Global Routing implementation |
| `--condition=normal` | Runs normal traffic |
| `--condition=congestion` | Adds extra traffic |
| `--condition=failure` | Disables the specified link |
| `--out=routing_results.csv` | Specifies the CSV output filename |

Example:

```bash
./ns3 run "scratch/assn10 --protocol=aodv --condition=normal --out=aodv_results.csv"
```

## 9. Output Explanation

The program prints per-flow statistics followed by an overall summary.

Example output format:

```text
Protocol: rip | Condition: normal
==========================================
Flow 1: SOURCE_IP -> DESTINATION_IP
  Throughput : VALUE Mbps
  Avg Delay  : VALUE ms
  PDR        : VALUE %
  Tx Packets : VALUE
  Rx Packets : VALUE

==========================================
Throughput         : VALUE Mbps
Packet Loss        : VALUE %
PDR                : VALUE %
Transmission Ratio : VALUE %
End-to-End Delay   : VALUE ms
```

The values above are placeholders to explain the format. The actual values are produced by your simulation.

### Meaning of each metric

**1. Throughput**

The amount of successfully received data per second.

Unit: Mbps (megabits per second).

Higher throughput means more data was successfully delivered per unit of time.

**2. Packet Loss**

The percentage of transmitted packets that were not received.

Formula:

Packet Loss (%) = 100 − PDR (%)

Lower packet loss generally indicates better delivery.

**3. Packet Delivery Ratio (PDR)**

The percentage of transmitted packets successfully received.

Formula:

PDR (%) = (Received Packets / Transmitted Packets) × 100

Higher PDR generally indicates more successful packet delivery.

**4. Transmission Ratio**

In this program, the ratio is calculated using the number of transmitted packets divided by an estimated number of packets based on the configured number of flows, duration, packet size, and a 1 Mbps reference rate.

This is a program-specific calculation. It is not the same as PDR and should not automatically be interpreted as a standard network metric.

**5. End-to-End Delay**

The average time taken by received packets to travel from source to destination, based on the FlowMonitor delay statistics.

Unit: milliseconds (ms).

Lower delay generally means faster packet delivery.

**6. Tx Packets**

The number of packets transmitted by a flow.

**7. Rx Packets**

The number of packets successfully received by a flow.

**8. Flow**

A flow represents traffic identified by properties such as source IP address, destination IP address, protocol, and port numbers.

## 10. CSV Output

By default, the program appends results to:

```text
routing_results.csv
```

The CSV columns are:

```text
Protocol,Condition,ThroughputMbps,PacketLossPercent,PDRPercent,TransmissionRatioPercent,EndToEndDelayMs,TxPackets,RxPackets
```

The file is created in the working directory from which the simulation runs, normally `~/ns-3` when using the commands above.

To view the file:

```bash
cat routing_results.csv
```

To open it in a spreadsheet application, copy the CSV file to a suitable location and open it with Excel or LibreOffice Calc.

**Important:** The program appends results. Running the same experiment multiple times adds more rows. The CSV is not automatically cleared between runs.

## 11. Inference

The simulation is designed to compare routing behaviour under different network conditions.

### Normal condition

The packet delivery ratio, throughput, packet loss, and delay show the performance of the selected routing approach under normal traffic.

### Congestion condition

Additional traffic increases the offered network load. If the results show higher packet loss or delay and a lower PDR, this indicates that congestion affected packet delivery.

### Link-failure condition

Disabling the Router 0–Router 1 link tests the network's response to a link failure. Changes in packet delivery and delay can indicate the impact of the failure and the routing approach's ability to use available paths.

### Comparison of routing approaches

Run the same three conditions for RIP, AODV, and Global Routing. Compare their measured throughput, packet loss, PDR, and end-to-end delay.

Do not decide which approach performs best until the actual output values have been compared.

## 12. Result

The NS-3 program is intended to evaluate routing performance using UDP traffic and FlowMonitor statistics under normal, congestion, and link-failure conditions.

The experiment's final conclusion should be based on the measured results from all selected routing approaches.

## 13. Important Code Limitations to Check

Before using the output in your lab report, be aware of the following issues in the supplied code:

1. **Routing helper installation:** The code explicitly installs RIP and AODV, but for `ospf` it does not explicitly install `Ipv4GlobalRoutingHelper` through `InternetStackHelper`. The global-routing option may therefore not behave as intended. Configure the routing helper correctly before relying on those results.

2. **Link failure:** The code uses `r0r1.Get(0).first` and `r0r1.Get(0).second` as arguments to `Ipv4::SetDown`. `Ipv4InterfaceContainer` provides interface/address pairs, not an `Ipv4` object. This line is likely to cause a compilation error and must be corrected using the appropriate IPv4 interface on each endpoint.

3. **Throughput calculation:** The program calculates per-flow throughput from received bytes divided by 25 seconds. The overall throughput is then the sum of those per-flow values. Treat this as the program's calculated metric.

4. **Flow filtering:** The program only includes UDP flows whose destination port equals 5000 in its per-flow summary. Extra congestion flows sent to ports 5001 and 5002 are excluded from that summary. Consequently, the reported summary does not fully represent all congestion traffic.

5. **CSV header:** The program checks whether the CSV file is empty before writing the header. If you reuse an existing file with an older or different format, its header will not be rewritten.

6. **AODV and RIP support:** Confirm that the installed NS-3 version supports the routing helpers and their use together with the selected topology. A successful compile alone does not prove the simulation represents the intended experiment correctly.

## 14. Final Conclusion

This experiment demonstrates how routing performance can be measured using NS-3. RIP, AODV, and NS-3 Global Routing can be compared using throughput, packet loss, PDR, and end-to-end delay. Normal, congestion, and link-failure scenarios help illustrate how network conditions influence packet delivery.

The actual results must be collected and checked before drawing conclusions about the relative performance of the routing approaches.