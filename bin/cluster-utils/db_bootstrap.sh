#!/bin/bash

#include common script
source "$(dirname "$0")/../init.sh"
promptUserBeforeRunningAdHocCommand "$@"

set +e #dont exit if any command fails - else we get kicked out of remote linux terminals - need to check exit status in scripts
set +x #echo on


echo "---------------------### Bootstrapping DB "$0" ###-----------------------"

set -x #echo off

echo "---------------------### stopping services ###-----------------------"
/usr/bin/ansible master  -i ${INVENTORY}  -b -a "service mysql stop"
/usr/bin/ansible backup  -i ${INVENTORY}  -b -a "service mysql stop"

echo "
---------------------### setting safe_to_bootstrap: 1 in master:/var/lib/mysql/grastate.dat ###-----------------------
"

/usr/bin/ansible master  -i ${INVENTORY}  -b -a "sed -i 's/safe_to_bootstrap: 0/safe_to_bootstrap: 1/' /var/lib/mysql/grastate.dat"

echo "
---------------------### SET safe_to_bootstrap: 1 in master:/var/lib/mysql/grastate.dat ###-----------------------
"

echo "---------------------### starting services ###-----------------------"
/usr/bin/ansible master  -i ${INVENTORY}  -b -a "service mysql start --wsrep-new-cluster"
/usr/bin/ansible backup  -i ${INVENTORY}  -b -a "service mysql start"

/usr/bin/ansible cluster  -i ${INVENTORY}  -b -a "service mysql status"

#
cd "$(dirname "$0")"

./db_find_bootstrap_node.sh -l "${HOSTPATTERN}" -a "${APP}" -c "${CONFIG}" -s "${SITE}" -v "${VERBOSITY}" -ip "${IP}" -ss "${SITESALT}" $@


echo "---------------------### END Bootstrapping DB "$0" ###-----------------------"
