#!/bin/bash

# example
#/vagrant/provisioning/ansible/bin/infra/all.sh -c local -s vb -ip yes -ss '' -l all -ir yes

set -e
##
set +x
###
CONFIG=$2
SITE=$4
###

PLAYBOOK="infra.yml"
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

HOSTPATTERN="nfs_servers"
source ../playbook.sh  -p "${PLAYBOOK}"  -l "${HOSTPATTERN}" -a "${APP}" -c "${CONFIG}" -s "${SITE}" -v "${VERBOSITY}" -ip "${IP}" -ss "${SITESALT}" $@

####################
cd "$(dirname "$0")"
####################

HOSTPATTERN="cluster"
source ../playbook.sh  -p "${PLAYBOOK}"  -l "${HOSTPATTERN}" -a "${APP}" -c "${CONFIG}" -s "${SITE}" -v "${VERBOSITY}" -ip "${IP}" -ss "${SITESALT}" $@


####################
cd "$(dirname "$0")"
####################

HOSTPATTERN="server_monitors"
source ../playbook.sh  -p "${PLAYBOOK}"  -l "${HOSTPATTERN}" -a "${APP}" -c "${CONFIG}" -s "${SITE}" -v "${VERBOSITY}" -ip "${IP}" -ss "${SITESALT}" $@

####################
cd "$(dirname "$0")"
####################

HOSTPATTERN="cluster_controllers"
source ../playbook.sh  -p "${PLAYBOOK}"  -l "${HOSTPATTERN}" -a "${APP}" -c "${CONFIG}" -s "${SITE}" -v "${VERBOSITY}" -ip "${IP}" -ss "${SITESALT}" $@

####################
cd "$(dirname "$0")"
####################
