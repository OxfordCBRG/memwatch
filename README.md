# Overview

`memwatch` allows a multi-user system to avoid thrashing, by preventing user processes from consuming more than a set percentage of the system memory. It is installed as a `systemd` service.

This is envisioned to be a superior system to placing all users into their own `cgroup`, as it allows any user to use any amount of memory so long as there is sufficient free. Once memory becomes constrained, `memwatch` will identify which user is accessing the largest amount of physical memory, find their most memory hungry process, and then call a handler script. A template script is provided which can kill the process identified.

`memwatch` is released under an MIT license (see the LICENSE file for details) and was written by Duncan Tooke.

# Requirements

- `memwatch` is written for Linux, and relies on the `/proc` filesystem to function
- Compiling the code requires a C++ compiler with c++-17 support

# Building

- Ensure that you have both `rpm-build` and `rpmdevtools` installed
- If you haven't done so already, run `rpmdev-setuptree` 
- Download the tarball for one of the tagged releases; the file should have a name similar to `memwatch-1.0.tar.gz`
- Build the source and binary files with `rpmbuild -ta TARFILE`, substituting `TARFILE` for the path to the download
- Install the RPM found in `~/rpmbuild/RPMS/x86_64/`
- **Copy `hook.example` to `hook`, or replace with a script of your choice**

Alternatively, untar the source and run `make`, then manually install the files by hand.

# Files Installed

- **/usr/share/doc/memwatch/LICENSE**: the license for the software
- **/usr/share/doc/memwatch/README.md**: this document
- **/usr/sbin/memwatch**: the main `memwatch` binary
- **/usr/libexec/memwatch/hook.example**: an example handler script
- **/usr/lib/systemd/service/memwatch.service**: the `systemd` service file

# How To Use

`memwatch` is a standard `systemd` service and can also be run manually for testing. It has no dynamically configurable options. **Remember to create `/usr/libexec/memwatch/hook` using the template provided or your own script, or no action will be taken when `memwatch` detects an over-memory situation**.

# Notes On The Design

- A MAX_MEM_PERCENT limit of 90% of the physical memory is calculated, and set as the maximum permitted total RSS (memory beyond this limit can still be used as a file system cache and is not wasted)
- `memwatch` then periodically calculates the total RSS of each user's processes, together with the total RSS of all processes on the system
- If the total system RSS is above the limit, the table of users is consulted to determine which user is accessing the most physical memory
- The largest process owned by this user is identified, and passed to a configurable handler script; by default, an example script can kill the process
- The handler script can be written in any language and with any behaviour desired
- All processes belonging to UIDs at or below MAX_SYSTEM_UID are ignored for the purposes of calculating which user is accessing the most memory; this is set to 999 by default based on the guidance for Red Hat Enterprise Linux
- By default, the SAMPLE_RATE_SECONDS is set to 2; this is to minimise periods in which processes that rapidly allocate memory can thrash the system
- RSS as reported by `/proc/[PID]/statm` is not a completely accurate measure, however experience has shown it to be sufficiently close enough to fulfil the requirements of the program; at the same time, caution should be exercised before increasing MAX_MEM_PERCENT higher than 90, or the deviation in reported RSS may be sufficient that `memwatch` never detects a low-memory situation.

# Contributing to memwatch

We'd be open to receiving contributions for any of the following:

- Bug fixes or simplicity/reliability/security improvements
- Packaging the code in non-RPM formats, e.g. DEB
- Improvements to default values