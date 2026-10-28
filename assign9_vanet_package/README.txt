ASSIGNMENT 9: TCP CONGESTION CONTROL USING NS-3
================================================

PACKAGE CONTENTS
----------------
scratch/assn9.cc
    The complete C++ NS-3 program.

README.txt
    Setup instructions, program explanation, important terms, output
    interpretation, and inference guidance.

xml/vanet-topology.xml
    A simple XML description of the simulated topology. This is a
    documentation/topology file. The C++ program does not read this XML;
    the topology is created directly in C++.

IMPORTANT: This package includes the same code structure discussed in the
conversation. It has not been compiled or run against your exact NS-3 build.
Check the notes about TCP algorithm availability and trace output below.

1. AIM
------
To simulate a simplified Intelligent Transportation System (ITS) network
with five vehicles sending traffic information to a traffic server, and
compare TCP congestion-control variants using throughput, delay, packet
loss, fairness, and congestion-window traces.

2. SCENARIO
-----------
- Five vehicle nodes act as TCP senders.
- One router represents a roadside forwarding point.
- One server receives traffic from all five vehicles.
- Each vehicle-to-router link: 10 Mbps and 2 ms delay.
- Shared router-to-server bottleneck: 2 Mbps and 20 ms delay.
- Bottleneck queue: DropTail, maximum 10 packets.
- Simulation duration: 20 seconds.
- Senders run from 1 second to 19 seconds.
- Each sender uses a different TCP port: 5000 to 5004.
- The program attempts NewReno, Cubic, and BBR in sequence.

This is a simplified wired network model inspired by the vehicle-to-roadside
infrastructure scenario. It does NOT model vehicle movement or a real wireless
VANET/802.11p radio channel.

3. WHAT THE PROGRAM DOES
------------------------
1. Checks whether a requested TCP implementation exists in this NS-3 build.
2. Creates five vehicle nodes, a router, and a server.
3. Installs the Internet stack and assigns IPv4 addresses.
4. Creates five vehicle-to-router links and one shared bottleneck link.
5. Starts a TCP receiver on the server for each vehicle.
6. Starts each vehicle's TCP sender.
7. Runs FlowMonitor to collect flow statistics.
8. Prints throughput, delay, packet loss, and fairness.
9. Attempts to record congestion-window changes in a CSV file.
10. Repeats the simulation for each available TCP variant and prints a
    comparison table.

4. HOW TO SET UP AND RUN FROM SCRATCH
-------------------------------------
A. Open your Ubuntu/WSL terminal.

B. Go to your NS-3 root folder (the folder containing the ./ns3 script).
   Example:
       cd ~/ns-3

   If your NS-3 folder is named ns-3.38, use:
       cd ~/ns-3.38

C. Extract the downloaded ZIP on your computer.

D. Copy the source file into the NS-3 scratch folder. If the extracted
   folder is in Downloads, the path may vary. Example:
       cp ~/Downloads/assign9_vanet_package/scratch/assn9.cc ~/ns-3/scratch/assn9.cc

   If the package was extracted elsewhere, change the source path accordingly.
   You can check the destination with:
       ls scratch

   You should see:
       assn9.cc

E. Run the program from the NS-3 root folder:
       ./ns3 run "scratch/assn9"

   Do not type the .cc extension in the run target.

F. The program automatically attempts all three variants. You do not need
   to pass --tcp arguments.

5. OUTPUT FILES
---------------
The program attempts to create:
- cwnd_NewReno.csv
- cwnd_Cubic.csv
- cwnd_Bbr.csv

Each file is intended to contain:
Time,OldCwnd,NewCwnd

These files are created in the current working directory, usually the NS-3
root folder, because that is where the run command is launched.

The congestion-window trace connection can depend on socket creation and
the NS-3 build. If the program prints "Warning: Congestion window trace
unavailable", the trace did not connect; do not claim the CSV contains
complete congestion-window data.

6. IMPORTANT TERMS
------------------

TCP (Transmission Control Protocol)
A transport protocol that provides reliable, ordered delivery of data.
It retransmits lost data and adjusts its sending behaviour when congestion
is detected.

TCP CONGESTION CONTROL
The rules TCP uses to control how much data it sends into the network.
It helps prevent a congested network from being overloaded.

TCP NEWRENO
A widely used TCP congestion-control algorithm. It adjusts the congestion
window in response to acknowledgements and packet loss.

TCP CUBIC
A TCP congestion-control algorithm that uses a cubic growth function for
its congestion window. Whether TcpCubic exists depends on the NS-3 build.

TCP BBR
Bandwidth Bottleneck and Round-trip propagation time (BBR) estimates the
bottleneck delivery rate and round-trip propagation time to control sending.
Whether TcpBbr exists depends on the NS-3 build.

VANET
Vehicular Ad Hoc Network: a network involving vehicles communicating with
other vehicles or roadside infrastructure. This example is only a simplified
wired representation, not a complete wireless VANET simulation.

NODE
A simulated device, such as a vehicle, router, or server.

ROUTER
A node that forwards packets between network links.

BOTTLENECK
The link with less capacity than the traffic offered to it. Here the shared
link is 2 Mbps while each vehicle's access link is 10 Mbps.

QUEUE
A waiting line for packets before transmission. A queue can fill when packets
arrive faster than they can be sent.

DROPTAIL
A queue discipline that drops arriving packets when its queue is full.

THROUGHPUT (Mbps)
The amount of successfully received data per second.
Higher throughput usually means more data was delivered in the same time.

DELAY (ms)
The time taken by a packet to travel from source to destination, as measured
by FlowMonitor's per-packet delay statistics.
Lower delay is generally preferable for time-sensitive traffic.

PACKET LOSS
Packets that FlowMonitor identifies as lost. Loss can occur when congestion
causes queue overflow, although interpretation depends on simulation and
monitoring details.

CONGESTION WINDOW (cwnd)
TCP's limit on the amount of unacknowledged data that may be in flight.
Changes in cwnd show how TCP adjusts its sending behaviour.

FAIRNESS / JAIN'S FAIRNESS INDEX
A value between 0 and 1 that estimates how evenly throughput is shared
between the five vehicle flows.
- Near 1: the flows receive similar throughput.
- Lower value: some flows receive much more throughput than others.
The code calculates fairness using all five configured flow throughputs.

FLOWMONITOR
An NS-3 tool that collects flow-level statistics such as received bytes,
packets, delay, and lost packets.

CSV
A comma-separated text file that can be opened in a spreadsheet program.

7. HOW TO INTERPRET THE VALUES
------------------------------
Compare the final comparison table for each TCP variant.

THROUGHPUT:
- Higher is generally better if the network delivers the data reliably.
- A higher value means more data was received per second.
- The sum of five flows cannot be expected to exceed the bottleneck capacity
  in steady state, though measured values depend on the measurement window.

DELAY:
- Lower is generally better for urgent road warnings and safety messages.
- A high delay can indicate queueing at the bottleneck.
- Consider delay together with throughput and loss.

PACKET LOSS:
- Lower is generally better.
- A nonzero value may indicate congestion or packet drops.
- Zero reported loss does not prove that a real network would never lose data.

FAIRNESS:
- Closer to 1 means more equal throughput among the five vehicles.
- A high total throughput with poor fairness can mean one or two flows get
  more of the shared capacity than others.

CONGESTION WINDOW:
- Increasing cwnd means TCP is allowing more data to be in flight.
- A reduction can occur after congestion or loss signals.
- Use the CSV only if the trace connected and the file contains data.

8. INFERENCE TEMPLATE
---------------------
Do not copy a guessed winner. Fill this using the actual output from your run:

"Among the TCP variants that ran successfully, __________ achieved the
highest total throughput of __________ Mbps. __________ showed the lowest
average delay of __________ ms. The lowest packet loss was __________,
with __________ lost packets. The fairness index was highest for __________
at __________, indicating the most even sharing of the bottleneck among the
five flows. Therefore, for this simulation, __________ appears most suitable
when considering __________. The result is limited to this simplified wired
bottleneck model and may differ in a real wireless VANET."

Possible conclusion guidance:
- For urgent safety messages, low delay and dependable delivery matter.
- For sending large camera or sensor data, throughput also matters.
- Fairness matters when several vehicles share the same infrastructure.
- No single TCP algorithm should be declared best without comparing the
  actual measured values.

9. IMPORTANT LIMITATIONS / TROUBLESHOOTING
------------------------------------------
A. TCP variant not available:
   The program checks for ns3::TcpNewReno, ns3::TcpCubic, and ns3::TcpBbr.
   If the type is absent, it prints that the variant is unavailable and skips
   it. This is especially important with older NS-3 releases. A skipped
   variant has no valid results and must not be included as if it ran.

B. Congestion-window trace warning:
   If you see "Warning: Congestion window trace unavailable", the main
   flow statistics may still be printed, but the cwnd CSV may be empty or
   incomplete. Do not use it to draw a cwnd conclusion.

C. Program not found:
   Check the exact filename:
       ls scratch
   If it is assn9.cc, run:
       ./ns3 run "scratch/assn9"

D. Compilation errors:
   Keep the complete compiler error text. NS-3 API support may differ between
   versions. Do not assume the program successfully ran if the build fails.

10. XML FILE
------------
xml/vanet-topology.xml documents the conceptual nodes, links, and parameters.
The C++ simulation does not parse this XML. It is included as a readable
topology description, not as a runnable NS-3 XML configuration.
