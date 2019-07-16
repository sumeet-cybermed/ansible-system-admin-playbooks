#!/bin/bash

set -e #exit if any command fails
set +x
###


CONFIG=$2
SITE=$4

PLAYBOOK="server_monitor.yml"
APP="infra"
VERBOSITY="vv"
IP="yes"
SITESALT=""
######
##
source "$(dirname "$0")/../functions.sh"
promptUserBeforeRunningPlaybook "$@"
##
rm -f /var/log/ansible*.log
##

set -x #echo on


####################
cd "$(dirname "$0")"
####################
#
#HOSTPATTERN="server_monitors"
#source ../playbook.sh  -p "${PLAYBOOK}"  -l "${HOSTPATTERN}" -a "${APP}" -c "${CONFIG}" -s "${SITE}" -v "${VERBOSITY}" -ip "${IP}" -ss "${SITESALT}" $@
#
#####################
#cd "$(dirname "$0")"
#####################

#also install agents on cluster as needed
HOSTPATTERN="cluster:monitored_servers"
source ../playbook.sh  -p "${PLAYBOOK}"  -l "${HOSTPATTERN}" -a "${APP}" -c "${CONFIG}" -s "${SITE}" -v "${VERBOSITY}" -ip "${IP}" -ss "${SITESALT}" $@


####################
cd "$(dirname "$0")"
####################
