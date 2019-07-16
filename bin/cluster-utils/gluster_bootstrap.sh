#!/bin/bash

#include common script
source "$(dirname "$0")/../init.sh"
promptUserBeforeRunningAdHocCommand "$@"

set +e #dont exit if any command fails - else we get kicked out of remote linux terminals - need to check exit status in scripts
set -x #echo on


echo "--------------------- $0 "

# see http://galeracluster.com/documentation-webpages/monitoringthecluster.html


/usr/bin/ansible cluster  -i ${INVENTORY}  -b -a "service glusterfsd stop"
# /usr/bin/ansible cluster  -i ${INVENTORY}  -b -a "glusterd --xlator-option *.upgrade=on -N"
/usr/bin/ansible cluster  -i ${INVENTORY}  -b -a "service glusterfsd start"
/usr/bin/ansible cluster  -i ${INVENTORY}  -b -a "systemctl status glusterfsd.service"
 
eval "$(dirname "$0")/gluster_get_status.sh $@"

echo "---------------------END $0 "
