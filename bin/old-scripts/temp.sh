#!/bin/bash

#include common script
. /vagrant/provisioning/ansible/bin/init.sh

# run base playbook for roles, etc
echo "---------------------### Provisioning  ###-----------------------"
$ANS_BIN /vagrant/provisioning/ansible/temp.yml -i'127.0.0.1,' -v
