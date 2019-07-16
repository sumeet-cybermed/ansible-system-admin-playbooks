#!/bin/bash

#include common script
. /vagrant/provisioning/ansible/bin/init.sh

# /vagrant/provisioning/ansible/bin/common.sh

# run base playbook for roles, etc
echo "---------------------### Provisioning  ###-----------------------"
$ANS_BIN /vagrant/provisioning/ansible/phabricator.yml -i'127.0.0.1,' -v
