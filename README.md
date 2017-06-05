# riaps-timesync

Time synchronization service, instrumentation and tests for the RIAPS platform.

[![Build Status](https://travis-ci.com/RIAPS/riaps-timesync.svg?token=QQcruvP29rqE9b8AjB5C&branch=master)](https://travis-ci.com/RIAPS/riaps-timesync)

## Time synchronization configuration tool: tsman

The release (.deb) packages contain a configuration tool, called **tsman** for an easy, semi-automated way for configuring a node for distributed time synchronization.
The tool configures and checks the status of the ChronoCape board (if needed) in the device tree and controls all services (`GPSd, chrony, phc2sys, ptp4l`, etc) needed
in a specific role.

Notes:

 - The tool is installed under `/opt/riaps/*<arch>*/bin`, where `*<arch>*` is either `armhf` or `amd64`. You should either make this directory part of your `PATH` or execute the tool with a full-path specification.

 - Some operations (`config` and `restart`) needs root priviledges, thus you need to run the tool as *root* or with the `sudo` command

The following time synchronization roles are supported:

 - **gps-master**: the node is synchornized to GPS (using the ChronoCape board) and acts as a PTP master on the local network. If the GPS signal is not present (or not reliable), the node falls back to NTP-based timesync (but still acts as a PTP master). Needs ChoronCape board and/or internet access.

 - **ntp-master**: the node is synchornized to NTP and acts as a PTP master on the local network. Needs internet access.

 - **slave**: the node is forced to use PTP in slave mode. Needs a PTP master on the local network.

Running **tsman**:

 - `tsman config`: will list the available roles.
 - `tsman config <role>`: will change the node's configuration for the given role and will restart the services. Needs root priviledges.
 - `tsman restart`: will restart the services in the current role. Needs root priviledges.
 - `tsman status`: check the status of the services and device tree in the current role.

Examples:

To configure the node for a GPS master role:

    sudo /opt/riaps/armhf/bin/tsman config gps-master

To check if everything is as expected in the current role:

    /opt/riaps/armhf/bin/tsman status