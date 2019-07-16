<?php
class DATABASE_CONFIG {

	public $default = array(
		'datasource' => 'Database/Mysql',
		'persistent' => false,
		'host' => '{{db.mysql_listen_addr}}',
		'login' => 'cmon',
		'password' => '{{cmon_mysql_password}}',
		'database' => DB_NAME,
		'port' => '{{db.mysql_listen_port}}',
		'prefix' => '',
		//'encoding' => 'utf8',
	);

    public $test = array(
        'datasource' => 'Database/Mysql',
        'persistent' => false,
        'host' => DB_HOST,
        'login' => DB_LOGIN,
        'password' => DB_PASS,
        'database'   => 'test_dcps'
    );
}
