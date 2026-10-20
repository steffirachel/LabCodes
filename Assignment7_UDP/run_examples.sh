#!/usr/bin/env bash
set -e
# Run from the directory containing ./ns3
./ns3 run "scratch/udpns3 --delay=2ms --rateMbps=1"
./ns3 run "scratch/udpns3 --delay=5ms --rateMbps=1"
./ns3 run "scratch/udpns3 --delay=10ms --rateMbps=1"
./ns3 run "scratch/udpns3 --delay=20ms --rateMbps=1"
