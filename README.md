# riaps-timesync

Time synchronization service, instrumentation and tests for the RIAPS platform.

[![Build Status](https://travis-ci.com/RIAPS/riaps-timesync.svg?token=QQcruvP29rqE9b8AjB5C&branch=master)](https://travis-ci.com/RIAPS/riaps-timesync)

Each release consists the following files:

- `riaps-timesync_amd64_<version>.deb` and `riaps-timesync_armhf_<version>.deb` contain the C-based library of the timesync services. Install one of these (based on your platform) using `dpkg -i riaps-timesync_<arch>_<version>.deb`

- `riaps_ts-<version>-py3-none-any.whl` platform independent Python interface for the timesync services. Install this with `pip install riaps_ts-<version>-py3-none-any.whl`

- `riaps-timesync-config.tar.gz` sample configuration files for running PTP and using the ChronoCape GPS time reference. Untar from the root folder: `tar zxvf riaps-timesync-config.tar.gz -C /`
