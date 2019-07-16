#!/bin/bash

#include common script
. /vagrant/provisioning/ansible/bin/init.sh

# run base playbook for roles, etc
echo "---------------------### Provisioning  ###-----------------------"
$ANS_BIN /vagrant/provisioning/ansible/infra.yml -i'127.0.0.1,' -v

#"-e 'NODE_ENV=staging GIT_BRANCH=develop'"
# - name: Node apps deploy
  # hosts: all
  # gather_facts: no
  # vars:
    # git_branch: "{{ GIT_BRANCH|default(develop) }}"
    # node_env: "{{ NODE_ENV|default(staging) }}"
  # tasks:
    # - command echo "{{ node_env }}"
    # - command echo "{{ git_branch }}"
    