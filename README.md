# PCN NS-3 Assignments 7, 8, 9 and 11

This package contains four assignment folders, one C++ NS-3 program per folder, detailed instructions, run scripts, and observation-table templates.

## 1. Enter the NS-3 folder

```bash
cd ~/ns-3
```

The folder name might differ on another computer. If this path does not exist, locate the directory that contains the `ns3` script. For example:

```bash
ls
find ~ -maxdepth 3 -name ns3 -type f 2>/dev/null
```

Use `cd` with the directory that contains the `ns3` script.

## 2. Configure NS-3

```bash
./ns3 configure --enable-examples
```

This configures the NS-3 build and enables example programs.

## 3. Build NS-3

```bash
./ns3 build
```

Wait for the build to finish. If it reports an error, read the first compiler error; do not continue to run the assignment until the build succeeds.

## 4. Run an example

```bash
./ns3 run first
```

This checks that NS-3 can run its basic `first` example. If your NS-3 version does not have this example, use an example listed by:

```bash
./ns3 show targets | head -40
```

## 5. Copy this package's C++ files into your NS-3 folder

Extract this ZIP. From the extracted package, copy each `.cc` file into the `scratch` directory of your NS-3 installation. Example commands (adjust the extracted path if needed):

```bash
cp ~/Downloads/PCN_Assignments_7_8_9_11/Assignment7_UDP/udpns3.cc ~/ns-3/scratch/
cp ~/Downloads/PCN_Assignments_7_8_9_11/Assignment8_TCP_UDP/assign8.cc ~/ns-3/scratch/
cp ~/Downloads/PCN_Assignments_7_8_9_11/Assignment9_TCP_Congestion/assign9.cc ~/ns-3/scratch/
cp ~/Downloads/PCN_Assignments_7_8_9_11/Assignment11_Heterogeneous_Wireless/assign11.cc ~/ns-3/scratch/
```

If your NS-3 folder is not `~/ns-3`, replace that path with your actual folder. Create `scratch` if it is missing:

```bash
mkdir -p ~/ns-3/scratch
```

## 6. Run your own C++ program

The cheat-sheet example is:

```bash
./ns3 run scratch/hello
```

For these programs, use the corresponding target name:

```bash
./ns3 run scratch/udpns3
./ns3 run scratch/assign8
./ns3 run scratch/assign9
./ns3 run scratch/assign11
```

For programs that accept parameters, place them after `--`. Examples:

```bash
./ns3 run "scratch/udpns3 --delay=5ms --rateMbps=1"
./ns3 run "scratch/assign8 --bottleneckRate=5Mbps --bottleneckDelay=10ms --queuePackets=50 --udpRateMbps=1"
./ns3 run "scratch/assign9 --tcpType=ns3::TcpNewReno --bottleneckRate=2Mbps"
./ns3 run "scratch/assign11 --speed=5"
```

Run commands from the NS-3 directory (the directory containing `./ns3`).

## 7. Results and generated files

The programs print summary statistics in the terminal. Depending on the assignment, they also create `.pcap`, `.xml`, or `.csv` files in the NS-3 run directory. Open PCAP files with Wireshark, for example:

```bash
wireshark udp-port-trace-*.pcap
```

If Wireshark is not installed:

```bash
sudo apt update
sudo apt install wireshark
```

Use the CSV observation templates in each assignment folder to record results from your own runs. Do not copy made-up sample numbers into a lab record.

## Folder contents

- `Assignment7_UDP/`: UDP ports 5000 and 6000, link latency and throughput.
- `Assignment8_TCP_UDP/`: TCP/UDP flows, routers, bottleneck rate, delay and queue size.
- `Assignment9_TCP_Congestion/`: competing TCP flows, congestion-control variants, fairness and congestion-window trace.
- `Assignment11_Heterogeneous_Wireless/`: Wi-Fi mobility experiment and estimated received signal power.
- Each folder contains its own `README.md`, C++ source, `run_examples.sh`, and `observations_template.csv`.

## Scope and honesty notes

- These are NS-3 simulations, not programs that compile with plain `g++` outside NS-3.
- The supplied assignment 11 program models Wi-Fi only. A full Bluetooth and Zigbee simulation is not implemented by this source; stock NS-3 does not provide a complete Bluetooth stack, and Zigbee support depends on external modules/version. Do not claim that the source simulates all three technologies.
- Assignment 9 checks whether the requested TCP TypeId exists in your installed NS-3. BBR availability depends on NS-3 version and modules. If unavailable, run Reno and Cubic and record that limitation.
- This package has not been compiled against the user's specific NS-3 installation. NS-3 API differences may require small version-specific changes.
