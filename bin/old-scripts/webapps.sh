#!/bin/bash

#include common script
. /vagrant/provisioning/ansible/bin/init.sh

echo There are $# arguments to $0:
echo first argument: $1
echo second argument: $2
echo third argument: $3
echo here they are again: $@

filemask=""
if [ "$1" == "" ]; then
    filemask='**'
else
    filemask=$1
fi

filepath="/vagrant/provisioning/ansible/sites-data/webapps/$filemask/*.yml"

for file in $filepath
do
    echo "---------------------### Provisioning $file ###-----------------------"
    $ANS_BIN /vagrant/provisioning/ansible/webapps.yml -i'127.0.0.1,' --extra-vars="@$file" -v
done
#sumeet END
