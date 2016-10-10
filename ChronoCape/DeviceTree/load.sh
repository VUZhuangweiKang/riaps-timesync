#!/bin/bash

CAPE=CHRONOCAPE

if [ $# -eq 1 ]; then
	CAPE=$1
fi
echo "Loading $CAPE"
sudo sh -c "echo $CAPE > /sys/devices/platform/bone_capemgr/slots"
cat /sys/devices/platform/bone_capemgr/slots
