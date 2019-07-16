#!/bin/bash

#include common script
source "$(dirname "$0")/../init.sh"
promptUserBeforeRunningAdHocCommand "$@"

set +e #dont exit if any command fails - else we get kicked out of remote linux terminals - need to check exit status in scripts
set -x #echo on


eval "$(dirname "$0")/db_bootstrap.sh $@"
eval "$(dirname "$0")/gluster_bootstrap.sh $@"

