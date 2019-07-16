#!/bin/bash

#include common script
source "$(dirname "$0")/../init.sh"
promptUserBeforeRunningAdHocCommand "$@"

set +e #dont exit if any command fails - else we get kicked out of remote linux terminals - need to check exit status in scripts
set -x #echo on

echo "---------------------### Finding Bootstrapped Node "

# see http://galeracluster.com/documentation-webpages/monitoringthecluster.html

#all vars

/usr/bin/ansible cluster  -i ${INVENTORY}  -b -a "gluster peer status"
/usr/bin/ansible cluster  -i ${INVENTORY}  -b -a "gluster volume status all"
/usr/bin/ansible cluster  -i ${INVENTORY}  -b -a "gluster volume info all"

/usr/bin/ansible cluster  -i ${INVENTORY}  -b -a "gluster volume heal "${RUN_COMMAND}" info "
/usr/bin/ansible cluster  -i ${INVENTORY}  -b -a "gluster volume heal "${RUN_COMMAND}" info split-brain"

/usr/bin/ansible cluster  -i ${INVENTORY}  -b -a "ls -la /var/www/"
/usr/bin/ansible cluster  -i ${INVENTORY}  -b -a "touch /var/www/test.txt"
/usr/bin/ansible cluster  -i ${INVENTORY}  -b -a "cat /var/www/test.txt"


echo "---------------------### Finding Bootstrap Node "$0" ###-----------------------"
