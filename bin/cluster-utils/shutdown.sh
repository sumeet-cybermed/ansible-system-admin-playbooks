#!/bin/bash

#include common script
source "$(dirname "$0")/../init.sh"
promptUserBeforeRunningAdHocCommand "$@"

set +e #dont exit if any command fails - else we get kicked out of remote linux terminals - need to check exit status in scripts
set -x #echo on


echo "--------------------- $0 "

# see http://galeracluster.com/documentation-webpages/monitoringthecluster.html
/usr/bin/ansible all  -i ${INVENTORY}  -b -a "service nginx stop"
#
/usr/bin/ansible all  -i ${INVENTORY}  -b -a "service mysql stop"
#
/usr/bin/ansible all  -i ${INVENTORY}  -b -a "service glusterfsd stop"


# for rhel & centos
/usr/bin/ansible backup  -i ${INVENTORY}  -b -a "systemctl poweroff"
/usr/bin/ansible master  -i ${INVENTORY}  -b -a "systemctl poweroff"

# for others
# /usr/bin/ansible all  -i ${INVENTORY}  -b -a "init 6"
