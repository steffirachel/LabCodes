# NS-3.42 source

**File:** `assn10_ns3_342.cc`

## Given
A six-node routed network with UDP traffic, tested under normal, congestion, and link-failure conditions.

## Run
Copy this file to `~/ns-3/scratch/assn10.cc`, then run from `~/ns-3`:

```bash
./ns3 run scratch/assn10
```

## Result
Prints packets sent/received, PDR, average end-to-end delay, and routing-table snapshot changes for nine scenarios.

## Inference
Compare normal, congestion, and failure results using your actual output. Snapshot changes are approximate. `OSPF-approx` uses Global Routing, not real OSPF.

The file is labelled for the NS-3.42 environment discussed in the conversation, but it should still be rebuilt locally.
