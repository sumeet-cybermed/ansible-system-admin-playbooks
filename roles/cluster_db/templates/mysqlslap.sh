mysql -q -A --connect-timeout=10 --host={{cluster_ip_addrs_list|first}} --port={{db.mysql_listen_port}} --user={{ mysql_root_username }} --password={{ mysql_root_secure_password }} -e 'drop database mysqlslap'

mysqlslap --user={{ mysql_root_username }} --password='{{ mysql_root_secure_password }}' --host={{cluster_ip_addrs_list|first}} --port={{db.mysql_listen_port}} --number-int-cols=5 --number-char-cols=20 --auto-generate-sql --verbose  --auto-generate-sql-load-type=mixed --auto-generate-sql-secondary-indexes=5  --auto-generate-sql-unique-query-number=25 --auto-generate-sql-unique-write-number=10  --commit=50 --auto-generate-sql-write-number=2000 --auto-generate-sql-execute-number=5000  --concurrency=5 --iterations=1

#--auto-generate-sql-guid-primary
#--auto-generate-sql-add-autoincrement
