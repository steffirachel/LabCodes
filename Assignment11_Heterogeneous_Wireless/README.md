# Assignment 11 — Performance Analysis of a Heterogeneous Wireless Network

## What the question asks
A smart campus uses Wi-Fi, Bluetooth and Zigbee devices. Users move at example speeds of 1, 5 and 10 m/s. Compare received signal strength, throughput, packet loss and end-to-end delay.

## What this supplied program actually models
This C++ program models **Wi-Fi only**: three mobile user nodes communicate with a Wi-Fi gateway using UDP. It uses RandomWalk2d mobility, FlowMonitor and a Friis propagation model to estimate received signal power at sampled times. It does **not** implement Bluetooth and Zigbee stacks. Treat this as the Wi-Fi part of the wider assignment, not a full three-technology implementation.

## Run each mobility speed
```bash
./ns3 run "scratch/assign11 --speed=1"
./ns3 run "scratch/assign11 --speed=5"
./ns3 run "scratch/assign11 --speed=10"
```

Use the same setup and only change speed between runs.

## Output: what each field means
- **Tx / Rx:** transmitted and received packets per flow.
- **Loss (%):** percentage of transmitted packets not received according to FlowMonitor.
- **Throughput (Mbps):** successfully received data rate.
- **Mean delay (ms):** average packet delay.
- **Estimated received power (dBm):** a model-based estimate derived from distance and the Friis propagation-loss model.
- **Signal CSV:** `assign11-signal-strength.csv` stores time, mobile node, distance and estimated received power.
- **PCAP/XML:** packet captures and detailed FlowMonitor results.

## Observation and inference
Use `observations_template.csv` to record the results for 1, 5 and 10 m/s. Faster movement can cause the distance to the gateway to change more quickly, which can change estimated received power and connectivity. The direction and size of changes depend on the random movement and simulation settings. Run several repetitions if your lab expects statistically stable comparisons.

## Important modeling note
The program estimates signal power using the Friis model; it does not measure real hardware signal strength. It uses Wi-Fi in ad-hoc mode, so it is a simplified wireless scenario rather than a complete campus infrastructure network. A complete comparison with Bluetooth and Zigbee requires appropriate modules/models and separate configuration.
