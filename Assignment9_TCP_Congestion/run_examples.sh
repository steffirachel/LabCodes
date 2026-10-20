#!/usr/bin/env bash
set -e
# Run from the directory containing ./ns3
./ns3 run "scratch/assign9 --tcpType=ns3::TcpNewReno --bottleneckRate=2Mbps"
./ns3 run "scratch/assign9 --tcpType=ns3::TcpCubic --bottleneckRate=2Mbps"
# BBR may not exist in your NS-3 version; run separately only if supported:
# ./ns3 run "scratch/assign9 --tcpType=ns3::TcpBbr --bottleneckRate=2Mbps"
