#!/bin/bash

set -x #echo on
set -e #exit if any cpommand fails

#
yum -y install libtool cmake Judy protobuf libevent Judy-devel protobuf-devel libevent-devel


mkdir -p /var/downloads/pinba
cd /var/downloads/pinba

# rm -Rf pinba_engine

# git clone --depth=1 https://github.com/tony2001/pinba_engine.git

#prepare mysql include files
mkdir -p /tmp/source_mysql/include
cp -Rf /usr/include/mysql/* /tmp/source_mysql/include 
cp -Rf /usr/include/mysql/* /tmp/source_mysql

#
cd pinba_engine

./buildconf.sh

./configure \
       --with-mysql=/tmp/source_mysql \
       --with-judy=/usr \
       --with-event=/usr \
       --with-protobuf=/usr \
       --with-libdir=/usr/lib64/mysql/plugin
       
 
make install

#
cd /usr/local/lib
mv /usr/local/lib/libpinba* /usr/lib64/mysql/plugin

#
/usr/sbin/semanage port -a -t mysqld_port_t -p udp 30002 

#
mysql –u root –p 'cx1@MySql' -e "INSTALL PLUGIN pinba SONAME 'libpinba_engine.so';"
service mysqld restart;

mysql –u root –p 'cx1@MySql' -e "CREATE DATABASE pinba;"
mysql –u root –p 'cx1@MySql' -e -D pinba < default_tables.sql

#
netstat -ln|grep 30002

#
phpize
./configure --with-pinba=/usr
make install

#
ls /usr/lib64/php/modules | grep pinba

echo  "
extension=pinba.so
pinba.enabled=1
" > /etc/php.d/pinba.ini

#pinboard
cd /var/www

rm -Rf pinboard
git clone --depth=1 https://github.com/intaro/pinboard.git --branch=v1.5.2
#
cd pinboard

#instal dependencies
curl -sS https://getcomposer.org/installer | php
php composer.phar install

# Initialize app (command will create additional tables and define crontab task):

./console migrations:migrate
./console register-crontab

# Point the document root of your webserver or virtual host to the web/ directory. Read more in Silex documentation. Example for nginx + php-fpm:

echo 
"
server {
    listen 80;
    server_name pinboard;
    root /var/www/pinboard/web;

    #site root is redirected to the app boot script
    location = / {
        try_files @site @site;
    }

    #all other locations try other files first and go to our front controller if none of them exists
    location / {
        try_files $uri $uri/ @site;
    }

    #return 404 for all php files as we do have a front controller
    location ~ \.php$ {
       return 404;
    }

    location @site {
        fastcgi_pass php;
        include fastcgi_params;
        fastcgi_param  SCRIPT_FILENAME $document_root/index.php;
        fastcgi_param HTTPS $https if_not_empty;
    }

    location ~ /\.(ht|svn|git) {
        deny  all;
    }
}
" > /etc/nginx/conf.d/pinboard-php.conf;
