#!/bin/bash


#include common script
source "$(dirname "$0")/../init.sh"
promptUserBeforeRunningAdHocCommand "$@"

set +e #dont exit if any command fails - else we get kicked out of remote linux terminals - need to check exit status in scripts
set -x #echo on

echo "---------------------### Finding Bootstrapped Node "

# see http://galeracluster.com/documentation-webpages/monitoringthecluster.html

#all vars
/usr/bin/ansible cluster  -i ${INVENTORY}  -b -a "for Q in `gluster volume list`; do   gluster reset $Q ;done"

echo "---------------------### Finding Bootstrap Node "$0" ###-----------------------"
