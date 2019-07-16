#!/bin/bash

set -e #exit if any command fails
set +x
###

CONFIG=$2
SITE=$4

PLAYBOOK="cluster_controller.yml"
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

HOSTPATTERN="cluster_controllers"
source ../playbook.sh  -p "${PLAYBOOK}"  -l "${HOSTPATTERN}" -a "${APP}" -c "${CONFIG}" -s "${SITE}" -v "${VERBOSITY}" -ip "${IP}" -ss "${SITESALT}" $@

