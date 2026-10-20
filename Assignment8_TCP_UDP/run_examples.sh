#!/usr/bin/env bash
set -e
# Run from the directory containing ./ns3
./ns3 run "scratch/assign8 --bottleneckRate=2Mbps"
./ns3 run "scratch/assign8 --bottleneckRate=5Mbps"
./ns3 run "scratch/assign8 --bottleneckRate=10Mbps"
