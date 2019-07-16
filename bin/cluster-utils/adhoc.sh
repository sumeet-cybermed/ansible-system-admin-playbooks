#!/bin/bash

source "$(dirname "$0")/../init.sh"
promptUserBeforeRunningAdHocCommand "$@"

set +e #dont exit if any command fails - else we get kicked out of remote linux terminals - need to check exit status in scripts
##

###
set -x #echo on


echo "---------------------### RUNNING AD HOC COMMAND: '$@' "

/usr/bin/ansible cluster -i ${INVENTORY} -b -a "${RUN_COMMAND}"
