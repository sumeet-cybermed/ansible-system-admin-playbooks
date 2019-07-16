#!/bin/bash

set -x #echo on
set -e #exit if any cpommand fails



wget https://copr.fedoraproject.org/coprs/isimluk/OpenSCAP/repo/epel-7/isimluk-OpenSCAP-epel-7.repo > /etc/yum.repos.d/isimluk-OpenSCAP-epel-7.repo
yum install - y openscap-utils scap-workbench scap-security-guide

# #include common script
# . /vagrant/provisioning/ansible/bin/init.sh

# # run base playbook for roles, etc
# echo "---------------------### Provisioning  ###-----------------------"
$ANS_BIN /vagrant/provisioning/ansible/roles/adv-security/hardening-io/ansible/provisioning/secure.yml -i'127.0.0.1,' -v
$ANS_BIN /vagrant/provisioning/ansible/roles/cis/BigAl/CIS/site.yml -i'127.0.0.1,' -v -C
