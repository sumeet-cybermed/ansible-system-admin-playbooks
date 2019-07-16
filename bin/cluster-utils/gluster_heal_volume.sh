#!/bin/bash

#include common script
source "$(dirname "$0")/../init.sh"
promptUserBeforeRunningAdHocCommand "$@"

set +e #dont exit if any command fails - else we get kicked out of remote linux terminals - need to check exit status in scripts
set -x #echo on

echo "---------------------### Finding Bootstrapped Node "

# see http://galeracluster.com/documentation-webpages/monitoringthecluster.html

#all vars

/usr/bin/ansible cluster  -i ${INVENTORY}  -b -a "gluster volume heal "${RUN_COMMAND}""

eval "$(dirname "$0")/gluster_get_status.sh $@"

echo "---------------------### Finding Bootstrap Node "$0" ###-----------------------"
