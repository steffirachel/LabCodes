# Assignment 10 — NS-3 Routing Protocol Comparison

## Files
- `NS3_342/assn10_ns3_342.cc` — version-labelled source for the NS-3.42 environment discussed in the conversation.
- `General/assn10_general.cc` — general-target source intended for CMake-based NS-3 3.37 and newer. It is not verified on every release.
- Each source folder includes its own README.

## Run instructions
Choose **one** file, copy it into your NS-3 installation's `scratch` folder as `assn10.cc`, then run from the NS-3 root.

Example if the extracted package is located at `~/Downloads/Assignment10_NS3_Routing_Protocols_Package`:

```bash
cp ~/Downloads/Assignment10_NS3_Routing_Protocols_Package/NS3_342/assn10_ns3_342.cc ~/ns-3/scratch/assn10.cc
cd ~/ns-3
./ns3 run scratch/assn10
```

For the general version:

```bash
cp ~/Downloads/Assignment10_NS3_Routing_Protocols_Package/General/assn10_general.cc ~/ns-3/scratch/assn10.cc
cd ~/ns-3
./ns3 run scratch/assn10
```

Change the paths if you extracted the ZIP somewhere else. Run only one source file at a time.

## What it measures
The program compares RIP, AODV, and Global Routing under normal traffic, added UDP traffic (congestion), and a link-failure condition. It prints sent/received packets, PDR, average delay, and changes observed in periodic routing-table snapshots.

## Notes for the lab record
- Global Routing is not a full OSPF implementation. Do not call its results actual OSPF results.
- Routing-table snapshot changes are an estimate, not a direct count of every routing update.
- Numerical results depend on the code, simulator version, and run. Use your actual terminal output.
- The general file is intended for NS-3.37+ but has not been tested on every release. Older Waf-based releases use different build/run commands.
