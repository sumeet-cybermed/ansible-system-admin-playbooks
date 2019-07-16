grant all privileges on *.* to 'root'@'%' identified by '';
grant all privileges on *.* to 'root'@'localhost' identified by '';
grant all privileges on *.* to 'root'@'127.0.0.1' identified by '';
grant all privileges on *.* to 'root'@'::1' identified by '';
grant all privileges on *.* to 'root'@'::1' identified by '';
flush privileges;

/sbin/iptables -A INPUT -p tcp --destination-port {{db.mysql_listen_port}} -j ACCEPT
