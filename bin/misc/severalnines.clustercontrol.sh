#!/bin/bash


set +e #dont exit if any command fails - else we get kicked out of remote linux terminals - need to check exit status in scripts
set -x #echo on

#for cluster deployment run non locally the git usernamer and password should first be cached locally on master node so that password prompt  doesnt hang in limbo!!
# bash /vagrant/provisioning/ansible/utility_scripts/02-get_vagrant_scripts.sh

cd /vagrant/provisioning/ansible
ansible-playbook -i severalnines.clustercontrol.hosts.txt severalnines.clustercontrol.yml -vv
