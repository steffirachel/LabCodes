#!/usr/bin/env bash
set -e
# Run from the directory containing ./ns3
./ns3 run "scratch/assign11 --speed=1"
./ns3 run "scratch/assign11 --speed=5"
./ns3 run "scratch/assign11 --speed=10"
