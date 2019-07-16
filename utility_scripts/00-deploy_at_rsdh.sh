#!/bin/bash
set -x #echo on
set -e #exit if any cpommand fails

newfolder=$(date +"%m_%d_%Y")

newfolder="ccrm_$newfolder"

rm -Rf /var/www/$newfolder
mkdir -p /var/www/$newfolder
cd /var/www/$newfolder
git clone git@gitlab.com:CyberMedCRM/10-CCRM-Core.git
#enter username & password

#set permissions
chown -R nginx:nginx /var/www/$newfolder
mkdir -p /var/www/$newfolder/10-CCRM-Core/10-WebRoot/cache
chmod 0777 /var/www/$newfolder/10-CCRM-Core/10-WebRoot/cache
chmod 0777 /var/www/$newfolder/10-CCRM-Core/10-WebRoot/config.php

#import database
mysql -u root -p"cx1@MySql" -e "DROP DATABASE $newfolder;" | true
mysql -u root -p"cx1@MySql" -e "CREATE DATABASE $newfolder;"
mysql -u root -p"cx1@MySql" $newfolder < /var/www/$newfolder/10-CCRM-Core/20-DBDumps/ccrm_dump.sql
#or connect via MySQL Admin and manually restore dump file

#
rm -Rf /var/www/$newfolder/10-CCRM-Core/10-WebRoot/cache/*

echo "new web root:
root /var/www/$newfolder/10-CCRM-Core/10-WebRoot;
"

echo "to change web root
    nano /etc/nginx/conf.d/php-ccrm.conf
"


echo "to change web root
    nano /etc/nginx/conf.d/hhvm-ccrm.conf
"

echo "to change active database:
 nano /var/www/$newfolder/10-CCRM-Core/10-WebRoot/config.php;
"

echo "to restart nginx & view errors:
  service nginx stop;rm -f /var/log/nginx/*.log;service nginx start;systemctl status nginx.service;tail -f -n 10 /var/log/nginx/*error*.log &
"




