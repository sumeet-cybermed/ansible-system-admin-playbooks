#!/bin/bash
#include common script
. /vagrant/provisioning/ansible/bin/init.sh

echo "---------------------### RUNNING AD HOC COMMAND: "$@"
cd "$(dirname "$0")/.."

/usr/bin/ansible "127.0.0.1" -i hosts -b -a "$@"
