#!/bin/bash

set -e
set -x

ssh root@10.10.10.254 "set +e;set -x;bash /vagrant/provisioning/ansible/utility_scripts/02-get_vagrant_scripts.sh; bash /vagrant/provisioning/ansible/utility_scripts/06-change-ip-addr.sh 254 $1;bash /vagrant/provisioning/ansible/utility_scripts/06-change-ip-addr.sh 201 $1"

ssh root@10.10.10.254
#"reboot"
