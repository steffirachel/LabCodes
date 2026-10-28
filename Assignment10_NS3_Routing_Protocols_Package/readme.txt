1. Aim
To compare the performance of different routing protocols — RIP, AODV, and Global Routing — under normal, congestion, and link-failure conditions using NS-3.
2. Explanation
Network topology
The simulation uses six nodes:
- Node 0 and Node 1: Send data packets.
- Intermediate router nodes: Forward packets toward the destination.
- Destination node: Receives the packets.
- Network links: Connect the nodes and provide possible paths for packets to travel.
Three routing approaches are compared:
Routing approach	Simple explanation
RIP	Uses routing information to select paths and updates routes as the network changes.
AODV	Discovers routes when needed, rather than maintaining routes to every destination all the time.
Global Routing	Uses NS-3's global-routing mechanism to calculate routes from network topology information.

Note: Global Routing here is not a full implementation of OSPF.
3. Experimental conditions









A. Normal condition

The network operates normally. Packets travel from the source nodes through the routers to the destination.












B. Congestion condition

Additional UDP traffic is introduced. More packets compete for network resources, which may increase delay or packet loss.












C. Link-failure condition

A network link is disabled during the simulation. This tests whether packets can reach the destination through another available path.



4. Output
The following is the actual RIP output you previously shared from your NS-3.42 run.
RIP — Normal condition
Protocol: RIP, Condition: normal
Packets Sent: 10253
Packets Received: 6671
PDR: 65.0639%
Average Delay: 598.681 ms
Routing Table Changes: 1


Explanation:
- Packets Sent: 10,253 packets were transmitted.
- Packets Received: 6,671 packets reached the destination.
- PDR: Approximately 65.06% of the sent packets were received.
- Average Delay: The average delay was approximately 598.68 milliseconds.
- Routing Table Changes: One change was observed in the routing-table snapshots.
RIP — Congestion condition
Protocol: RIP, Condition: congestion
Packets Sent: 29531
Packets Received: 6199
PDR: 20.9915%
Average Delay: 2235.14 ms
Routing Table Changes: 2


Explanation: Additional traffic increased the number of packets sent. The PDR dropped to approximately 20.99%, while average delay increased to approximately 2.24 seconds. This indicates poorer packet delivery and slower delivery under the simulated congestion condition.
RIP — Link-failure condition
Protocol: RIP, Condition: failure
Packets Sent: 10253
Packets Received: 6668
PDR: 65.0346%
Average Delay: 595.095 ms
Routing Table Changes: 5


Explanation: After a link was disabled, 6,668 packets were received out of 10,253 sent. Five routing-table snapshot changes were observed. This suggests changes in the recorded routing information during the failure scenario; it does not directly count routing-update messages.
Other protocols
Your previously shared output stopped at Protocol: AODV, Condition: normal. Therefore, the AODV congestion/failure results and Global Routing results are not available here. Do not copy invented values into your lab record; run the program to obtain those results.
5. Result
The simulation compares routing performance using packet delivery ratio, average delay, and observed routing-table changes. In the RIP results, congestion significantly reduced packet delivery ratio and increased average delay compared with the normal condition.
6. Inference
1. Under normal conditions, RIP delivered approximately 65.06% of transmitted packets.
2. Under congestion, RIP's packet delivery ratio fell to approximately 20.99%, and average delay increased significantly.
3. Under link failure, routing-table snapshots changed more frequently than in the normal scenario, indicating changes in the recorded routing information.
4. Packet delivery ratio and average delay help evaluate network performance under different conditions.
5. Comparing RIP, AODV, and Global Routing results helps identify how the routing approaches behave under normal traffic, congestion, and link failure.
Conclusion: In the RIP results provided, congestion had a much greater impact on packet delivery and delay than the observed link-failure scenario. The best-performing routing approach overall can only be determined after comparing the actual results for all three approaches.