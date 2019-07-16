#!/bin/bash

#include common script
. /vagrant/provisioning/ansible/bin/init.sh

#init wordpress sites

for file in /vagrant/provisioning/sites-data/wordpress/*.yml
do
    echo "---------------------### Provisioning $file ###-----------------------"
    $ANS_BIN /vagrant/provisioning/ansible/wordpress.yml -i'127.0.0.1,' --extra-vars="@$file" -v
done