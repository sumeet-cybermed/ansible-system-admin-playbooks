#!/bin/bash

set -x #echo off

#include common script
source "$(dirname "$0")/../init.sh"

set -e #exit if any command fails
set +x #echo off

# we presume this script was called with source from a nested subdir and ansible playbooks are one dir down
cd "$(dirname "$0")/../.."

pwd

##echo inventory file
#echo "--------inventory file----------------"
#cat ${INVENTORY}
#echo "--------END inventory file----------------"

INSTALL_COMMAND=""
CONFIG_COMMAND=""

#playbook vars_files
VAR_FILES="app_vars='${APP_VARS_FILE}' config_vars='${CONFIG_VARS_FILE}' site_vars='${SITE_VARS_FILE}'"

#extra-vars to be passed
COMBINED_EXTRA_VARS="${VAR_FILES} is_initial_run='${IS_INITIAL_RUN}' ${EXTRA_VARS} site_salt='${SITE_SALT}' "

BASE_COMMAND="${ANS_BIN} ${PLAYBOOK} -i ${INVENTORY} --limit ${LIMIT} -${VERBOSITY}"

# # run only the 'install_packages or always' tags to prevent download conflicts in shared hosts directory when testing with virtual box
if [ "yes" == "${PACKAGES}" ]; then
	INSTALL_COMMAND="${BASE_COMMAND} --tags  'install_packages,always' -e \"${COMBINED_EXTRA_VARS} serial_var='1'\" "
	
	if [ -n "${SKIP_TAGS}" ]; then
		INSTALL_COMMAND="$INSTALL_COMMAND --skip-tags '${SKIP_TAGS}' "
	fi

fi

CONFIG_COMMAND="${BASE_COMMAND} --tags '${TAGS}' --skip-tags 'install_packages${SKIP_TAGS}' -e \"${COMBINED_EXTRA_VARS} serial_var='0'\" ${HELP}"

echo "---------------------### Running '$0 $@' with Current Directory '$PWD' ###-----------------------"


#run the command
if [ -n "${INSTALL_COMMAND}" ]; then
    set -x #echo on
    eval ${INSTALL_COMMAND}
    set +x #echo off
fi

if [ -n "${CONFIG_COMMAND}" ]; then
    #run the command
    set -x #echo on
    eval ${CONFIG_COMMAND}
    set +x #echo off
fi

echo "---------------------### END Run '$0 $@' with Current Directory '$PWD' ###-----------------------"
