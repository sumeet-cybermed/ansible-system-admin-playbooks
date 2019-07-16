#!/bin/bash


#include common script
source "$(dirname "$0")/../init.sh"
promptUserBeforeRunningAdHocCommand "$@"

set +e #dont exit if any command fails - else we get kicked out of remote linux terminals - need to check exit status in scripts
set -x #echo on


echo "---------------------### Setting Bootstrap Node to "$5" ###-----------------------"

cd "$(dirname "$0")/../.."

COMMAND="\"SET GLOBAL wsrep_provider_options='pc.bootstrap=1';\""

/usr/bin/ansible cluster  -i ${INVENTORY} -l "${RUN_COMMAND}" -b -a "mysql -e $COMMAND"

echo "---------------------### END Setting Bootstrap Node to "$5" ###-----------------------"
