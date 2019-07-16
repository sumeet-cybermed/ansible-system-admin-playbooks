#!/bin/bash

#include common script
# no need as this runs on host, not guest
. /vagrant/provisioning/ansible/bin/init.sh

set -x #echo on
set -e #exit if any cpommand fails

# run base playbook for roles, etc
echo "---------------------### Provisioning  ###-----------------------"
$ANS_BIN /vagrant/provisioning/ansible/lxc_container.yml --extra-vars "$@" -i'127.0.0.1,' -v
