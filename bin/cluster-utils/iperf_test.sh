#!/bin/bash

#include common script
source "$(dirname "$0")/../init.sh"
promptUserBeforeRunningAdHocCommand "$@"

set +e #dont exit if any command fails - else we get kicked out of remote linux terminals - need to check exit status in scripts
set -x #echo on


/usr/bin/ansible master  -i ${INVENTORY}  -b -a "iperf -s &"
/usr/bin/ansible backup  -i ${INVENTORY}  -b -a "iperf -c {{ hostvars[master]['ansible_eth0']['ipv4']['address'] }} -r -P 4"

