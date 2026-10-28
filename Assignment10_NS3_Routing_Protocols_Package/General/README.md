# General-target source

**File:** `assn10_general.cc`

## Target
Intended for CMake-based NS-3 versions 3.37 and newer. This is not a guarantee that it compiles unchanged on every release.

## Run
Copy this file to `~/ns-3/scratch/assn10.cc`, then run from `~/ns-3`:

```bash
./ns3 run scratch/assn10
```

## Result
Prints sent/received packets, PDR, average delay, and routing-table snapshot changes for nine scenarios.

## Inference
Compare the measured results for normal traffic, congestion, and link failure. Global Routing is not actual OSPF. Snapshot changes are an estimate based on periodic checks.
