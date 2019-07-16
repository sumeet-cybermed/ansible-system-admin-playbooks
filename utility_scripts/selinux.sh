# Please note that SELinux adds 2-8% overheads to typical RHEL or CentOS installation.

clear

#change to premissive mode
echo "
------------------
selinux = permissive and  restart services
------------------"

setenforce 0;sestatus ;
service nginx restart
service mysql restart

echo "
------------------
selinux grep for success=no
------------------"

#try the operation
cat /var/log/audit/audit.log | grep success=no
 

echo "
------------------
selinux using audit2allow
------------------"
 
#
mkdir -p /opt/selinux/modules && cd /opt/selinux/modules 

#
grep nginx /var/log/audit/audit.log | audit2why
grep nginx /var/log/audit/audit.log | audit2allow -m nginx > nginx.te

#
grep mysqld /var/log/audit/audit.log | audit2why
grep mysqld /var/log/audit/audit.log | audit2allow -m mysql > mysql.te;cat mysql.te;

#nginx = webserver
grep nginx /var/log/audit/audit.log | audit2allow -M nginx;semodule -i nginx.pp;

#php-fpm
grep php-fpm /var/log/audit/audit.log | audit2allow -M php-fpm;semodule -i php-fpm.pp;

#mysql for socket
grep mysqld /var/log/audit/audit.log | audit2allow -M mysql;semodule -i mysql.pp;

#mysqld_safe for db access
grep mysqld_safe /var/log/audit/audit.log | audit2allow -M mysql_safe;semodule -i mysql_safe.pp;

#crond for jobs
grep crond /var/log/audit/audit.log | audit2allow -M crond;semodule -i crond.pp;

#
echo "
------------------
selinux = enforcing and  restart services
------------------"

setenforce 1;sestatus ;

tail -f -n 15 /var/log/audit/audit.log &
tail -f -n 10 /var/log/nginx/*error*.log &

service nginx restart
service mysql restart
service php-fpm restart

echo "
------------------
services status 
------------------"

systemctl status nginx.service;
systemctl status mysql.service;
systemctl status php-fpm.service;

#for nginx with caching:
chown -Rf nginx:nginx /var/lib/nginx


# journalctl  -r -u mysql.service
# journalctl  -r -u nginx.service

# getsebool -a 
# getsebool -a | grep off
# getsebool -a | grep on

# https://oracle-base.com/articles/mysql/mysql-installation-on-linux
# http://crashmag.net/change-the-default-mysql-data-directory-with-selinux-enabled
