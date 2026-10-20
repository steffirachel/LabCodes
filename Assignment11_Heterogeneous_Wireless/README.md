# Assignment 11 — Performance Analysis of a Heterogeneous Wireless Network

## What the question asks

Build a smart-campus scenario with Wi-Fi, Bluetooth, and Zigbee devices and gateways. Apply mobility at 1, 5, and 10 m/s, and compare received signal strength, throughput, packet loss, and end-to-end delay.

## Important scope note — read before submitting

The supplied NS-3 cheat sheet shows Wi-Fi and mobility helpers, but it does not include a Bluetooth stack. Standard NS-3 installations also do not generally provide a complete Bluetooth protocol stack in the same way they provide Wi-Fi. IEEE 802.15.4/LR-WPAN support may be available for a Zigbee-radio-level model, but a complete Zigbee network layer is not automatically provided by simply using the cheat-sheet commands.

Therefore, **the included program is a runnable Wi-Fi mobility baseline, not a claim that Bluetooth and Zigbee have been fully simulated**. It produces real NS-3 Wi-Fi traffic metrics and a Friis-model estimate of received power. To claim a complete three-technology comparison, your lab must provide/configure an appropriate Bluetooth model and an IEEE 802.15.4/LR-WPAN (or Zigbee) model. Do not describe the Wi-Fi output as measured Bluetooth or Zigbee output.

## Files

- `assign11.cc` — three mobile Wi-Fi users communicating with one stationary gateway.
- Run outputs: `assign11-flowmon.xml`, `assign11-signal-strength.csv`, and `assign11-wifi-*.pcap`.

## How the code works

1. Creates three mobile nodes and one gateway node.
2. Uses `WifiHelper`, `WifiMacHelper`, `YansWifiPhyHelper`, and a channel with Friis propagation loss.
3. Keeps the gateway stationary and moves the users with `RandomWalk2dMobilityModel`.
4. Installs the Internet stack and assigns IP addresses, following the cheat sheet.
5. Sends UDP packets from each user to a UDP server at the gateway.
6. Uses FlowMonitor to report transmitted/received packets, loss, throughput, and mean delay.
7. Samples each mobile node's distance from the gateway and estimates received power using the Friis propagation model. These values are written to CSV; they are model estimates, not hardware measurements.
8. Saves PCAP files for optional Wireshark inspection.

## How to run

Copy `assign11.cc` into your NS-3 `scratch` folder. From the NS-3 root folder:

```bash
./ns3 run 'scratch/assign11.cc --speed=1'
./ns3 run 'scratch/assign11.cc --speed=5'
./ns3 run 'scratch/assign11.cc --speed=10'
```

Run each speed separately and save/copy the terminal output before the next run, because the output files use the same names and can be overwritten. The program uses the cheat-sheet RandomWalk model with boundaries from -50 m to +50 m.

## Reading the outputs

### Terminal / FlowMonitor

- **Tx / Rx:** transmitted and received packets.
- **Loss (%):** `(Tx - Rx) / Tx × 100`.
- **Throughput (Mbps):** successfully received bits per second.
- **Mean delay (ms):** mean one-way delay reported by FlowMonitor.

### `assign11-signal-strength.csv`

Columns are time, mobile node number, distance to gateway in metres, and estimated received power in dBm. In this model, greater distance usually means lower received power. Mobility changes distance over time, so the estimated signal power also changes.

### PCAP / Wireshark

Open one of the generated `assign11-wifi-*.pcap` files in Wireshark to inspect the Wi-Fi packets. PCAP is for packet-level inspection; the CSV is the explicit estimated-power output in this starter program.

## Observation table

Fill this table from your actual runs. For signal strength, report a summary from the CSV (for example, average estimated power over all nodes and samples).

| Speed | Average estimated received power (dBm) | Throughput (Mbps) | Packet loss (%) | Mean delay (ms) |
|---|---:|---:|---:|---:|
| 1 m/s | | | | |
| 5 m/s | | | | |
| 10 m/s | | | | |

## Inference

As speed increases, users change position more quickly. This can make the distance to the gateway and estimated received power vary more rapidly. If a mobile node moves farther away, the Friis model predicts weaker received power. Throughput, packet loss, and delay may also change because wireless connectivity changes, but the exact direction and size must come from the actual run; speed alone does not guarantee that every metric worsens in every short simulation.

For the assignment's full comparison, repeat the same measurement process with actual Bluetooth and Zigbee/LR-WPAN models available in your lab, keeping the comparison conditions and reporting method consistent. Clearly label the results by technology and distinguish real model measurements from any simplified assumptions.
