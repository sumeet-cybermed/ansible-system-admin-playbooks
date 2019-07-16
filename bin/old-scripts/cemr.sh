#!/bin/bash

#debugging:
 
# Run ansible-playbook with ANSIBLE_KEEP_REMOTE_FILES=1

# create a python tracefile
# $ python -m trace --trace 
# /home/jtanner/.ansible/tmp/ansible-1387469069.32-4132751518012/command 
# 2>&1 | head 

#include common script
. /vagrant/provisioning/ansible/bin/init.sh

#common
# /vagrant/provisioning/ansible/bin/common.sh

#webapps
/vagrant/provisioning/ansible/bin/local/webapps.sh cemr

# #wordpress
# /vagrant/provisioning/ansible/bin/wordpress.sh

# #debian-med
# /vagrant/provisioning/ansible/bin/debian-med.sh | true

# #oscar
# /vagrant/provisioning/ansible/bin/oscar.sh | true

