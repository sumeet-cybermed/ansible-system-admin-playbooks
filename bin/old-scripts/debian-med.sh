#!/bin/bash

#include common script
. /vagrant/provisioning/ansible/bin/init.sh

# sumeet - init all sites & listeners
for file in /vagrant/provisioning/sites-data/webapps/debian-med/*.yml
do
    echo "---------------------### Provisioning $file ###-----------------------"
    $ANS_BIN /vagrant/provisioning/ansible/debian-med.yml -i'127.0.0.1,' --extra-vars="@$file" -v
done
#sumeet END
