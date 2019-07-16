#!/bin/bash

#sudo ~/vagrant/utility_scripts/change_host_name.sh ion-rhel7-desktop test-cxlm

echo "usage: $0 oldhostname newhostname"

set +x #echo on
set +e #echo on

oldhost="$1"
newhost="$2"

if [ "$oldhost" == "" ]; then
    echo "ERROR: oldhost argument missing"
    exit
fi

if [ "$newhost" == "" ]; then
    echo "ERROR: newhost argument missing"
    exit
fi

echo "changing hostname from: $oldhost to $newhost"

#global inplace replace
sedString="s/\.$oldhost/\.$newhost/g"

echo "$sedString"

#all nginx conf files
sed -i $sedString /etc/nginx/conf.d/*.conf

#dashboard - default
sed -i $sedString /var/www/content.txt

#dashboard - developer
sed -i $sedString /var/www/dashboard/content.txt

#xhprof config
sed -i $sedString /opt/xhprof/xhprof_lib/config.php

##
cat /etc/nginx/conf.d/php-mail.conf | true

#
service nginx restart