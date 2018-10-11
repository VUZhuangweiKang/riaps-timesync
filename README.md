# riaps-timesync

Time synchronization service, instrumentation and tests for the RIAPS platform.

[![Build Status](https://travis-ci.com/RIAPS/riaps-timesync.svg?token=QQcruvP29rqE9b8AjB5C&branch=master)](https://travis-ci.com/RIAPS/riaps-timesync)

## Time synchronization configuration tool: timesyncctl

The release (.deb) packages contain a configuration tool, called **timesyncctl** for an easy, semi-automated way for configuring a node for distributed time synchronization.
The tool configures and checks the status of the ChronoCape board (if needed) in the device tree and controls all services (`GPSd, chrony, phc2sys, ptp4l`, etc) needed
in a specific role.

Notes:

 - The tool is installed under `/opt/riaps/*<arch>*/bin`, where `*<arch>*` is either `armhf` or `amd64`. You should either make this directory part of your `PATH` or execute the tool with a full-path specification.

 - Some operations (`config` and `restart`) needs root priviledges, thus you need to run the tool as *root* or with the `sudo` command

The following time synchronization roles are supported:

 - **master**: the node is synchronized to (1) GPS (using the ChronoCape board) or (2) NTP (well-known internet server) or (3) NTP (the control/VM host) in this order. Selecting the best reference clock is automatic. It also acts as a PTP master on the local network.

 - **standalone**: same as master, but PTP master role is disabled.
 
 - **slave**: the node is forced to use PTP in slave mode. Needs a PTP master on the local network.

Running **timesyncctl**:

 - `timesyncctl config`: will list the available roles.
 - `timesyncctl config <role>`: will change the node's configuration for the given role and will restart the services. Needs root priviledges.
 - `timesyncctl restart`: will restart the services in the current role. Needs root priviledges.
 - `timesyncctl status`: check the status of the services and device tree in the current role.

Examples:

To configure the node for a GPS master role:

    sudo /opt/riaps/armhf/bin/timesyncctl config gps-master

To check if everything is as expected in the current role:

    /opt/riaps/armhf/bin/timesyncctl status
