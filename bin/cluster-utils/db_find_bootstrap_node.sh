#!/bin/bash

#include common script
source "$(dirname "$0")/../init.sh"
promptUserBeforeRunningAdHocCommand "$@"

set +e #dont exit if any command fails - else we get kicked out of remote linux terminals - need to check exit status in scripts
set +x #echo on


echo "
---------------------### Finding Bootstrapped Node --------------------
"

# see http://galeracluster.com/documentation-webpages/monitoringthecluster.html

#all vars
if [[ "all" = "${RUN_COMMAND}" ]]; then
	COMMAND="\"SHOW STATUS LIKE 'wsrep_%';\""
	/usr/bin/ansible cluster  -i ${INVENTORY}  -b -a "mysql -e ${COMMAND}"
else
	#imp vars
	COMMAND="\"SHOW STATUS LIKE 'wsrep_last_committed%';SHOW STATUS LIKE 'wsrep_cluster_%';\""
	/usr/bin/ansible cluster  -i ${INVENTORY}  -b -a "mysql -e ${COMMAND}"
fi

echo "
---------------------### END Finding Bootstrap Node "$0" ###-----------------------
"
