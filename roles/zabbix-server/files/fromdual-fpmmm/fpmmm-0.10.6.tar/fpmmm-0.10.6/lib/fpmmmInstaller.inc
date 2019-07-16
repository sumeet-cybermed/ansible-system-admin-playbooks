<?php

#
# Copyright (C) 2010-2016 FromDual GmbH
#


/*

	checkFpmmmRequirements($pParameter, $pRemoveLockFile)
	askForConfigurationFile($default = '/etc/fpmmm/fpmmm.conf')
	askForZabbixServer($default = '127.0.0.1')
	askForFpmmmLogFile($default = '/var/log/fpmmm/fpmmm.log')
	askForFpmmmCacheFileBase($default = '/var/cache/fpmmm')
	askForHostName($default)
	askForInstanceName($default)
	checkZabbixHostName($aConfiguration, $pExecutable, $pDebug)
	askForHostType()
	askForFpmmmModules($pHostType)
	askForFpmmmAgent($default = '/opt/fpmmm/bin/fpmmm')
	askForFpmmmUser($default = 'zabbix')
	askForDatabaseCredentials($pUser = 'fpmmm', $pHost = 'localhost', $pPort = 3306)
	askForZabbixAdminUser($default = 'Admin')
	askForZabbixAdminPassword($default = 'zabbix')

*/

// ---------------------------------------------------------------------
function checkFpmmmRequirements($pParameter, $pRemoveLockFile)
// ---------------------------------------------------------------------
{
	$rc = OK;
	$lDistribution = 'unknown';

	$lOs = getOs();
	if ( $lOs == 'Linux' ) {
		$lDistribution = getDistribution();
	}
	else {
		print "WARNING: $lOs is not supported yet.\n";
	}

	// print_r(get_defined_functions());
	// print_r(get_loaded_extensions());

	// See also checkMyEnvRequirements

	// if ( extension_loaded('mysqli') === false ) {
	if ( ! in_array('mysqli', get_loaded_extensions()) ) {
		$rc = 1950;
		fprintf(STDOUT, "WARNING: The PHP extension mysqli is not installed. You cannot run this tool\n");
		fprintf(STDOUT, "without it. Please install the package containing mysqli first (rc=%d):\n\n", $rc);

		switch ($lDistribution) {
		case 'CentOS':
		case 'RedHat':
		case 'Fedora':
			$cmd = "sudo yum install php-cli php-process php-mysqli";
			fprintf(STDOUT, "shell> $cmd\n\n");
			break;
		case 'SUSE':
		case 'openSUSE project':
			$cmd = "sudo zypper install php5 php5-posix php5-mysql";
			fprintf(STDOUT, "shell> $cmd\n\n");
			break;
		case 'Ubuntu':
		case 'Debian':
			$cmd = "sudo apt-get install php5-cli php5-mysqlnd";
			fprintf(STDOUT, "shell> $cmd\n\n");
			break;
		default:
			$cmd = 'echo "Unknown distribution: ' . $lDistribution . '"';
			fprintf(STDOUT, "OS/Distribution $lOs/$lDistribution is not yet supported.\n");
		}
// 		$ret = removeAgentLock($pParameter, $pRemoveLockFile);
// 		cleanAndQuit($pParameter, $pRemoveLockFile, $rc);
	}

	return $rc;
}

// ---------------------------------------------------------------------
function askForConfigurationFile($default = '/etc/fpmmm/fpmmm.conf')
// ---------------------------------------------------------------------
{
	$rc = OK;

	$aFpmmmConfCandidates = array(
  '/etc/fpmmm.conf'
, '/etc/fpmmm/fpmmm.conf'
, '/etc/zabbix/fpmmm.conf'
	);
	if ( ! in_array($default, $aFpmmmConfCandidates) ) {
		array_push($aFpmmmConfCandidates, $default);
	}

	// foreach ( $aFpmmmConfCandidates as $d ) {
		// if ( if_file($d) && is_dir($d) ) {
			// $default = $d;
			// continue;
		// }
	// }

	fprintf(STDOUT, "\nLocation of fpmmm configuration file.\n");
	fprintf(STDOUT, "\nExample:\n");

	foreach ( $aFpmmmConfCandidates as $conf ) {

		$d = $conf == $default ? '*' : ' ';
		fprintf(STDOUT, "%s %s\n", $d, $conf);
	}

	fprintf(STDOUT, "\nfpmmm agent configuration = ");
	$lConfigurationFile = trim(readline());

	if ( $lConfigurationFile == '' ) {
		$lConfigurationFile = $default;
	}

	return array($rc, $lConfigurationFile);
}

// ---------------------------------------------------------------------
function askForZabbixServer($default = '127.0.0.1')
// ---------------------------------------------------------------------
{
	$rc = OK;

	$aZabbixServerCandidates = array(
	  '127.0.0.1'
	);
	if ( ! in_array($default, $aZabbixServerCandidates) ) {
		array_push($aZabbixServerCandidates, $default);
	}

	$myself = gethostbyname(gethostname());
	if ( ! in_array($myself, $aZabbixServerCandidates) ) {
		array_push($aZabbixServerCandidates, $myself);
	}

	fprintf(STDOUT, "\nLocation of Zabbix server.\n");
	fprintf(STDOUT, "\nExample:\n");

	foreach ( $aZabbixServerCandidates as $server ) {

		$d = $server == $default ? '*' : ' ';
		fprintf(STDOUT, "%s %s\n", $d, $server);
	}

	fprintf(STDOUT, "\nZabbix Server  = ");
	$lZabbixServer = trim(readline());
	if ( $lZabbixServer == '' ) {
		$lZabbixServer = $default;
	}

	return array($rc, $lZabbixServer);
}

// ---------------------------------------------------------------------
function askForFpmmmLogFile($default = '/var/log/fpmmm/fpmmm.log')
// ---------------------------------------------------------------------
{
	$rc = OK;

	$aFpmmmLogFileCandidates = array(
	  '/var/log/fpmmm.log'
	, '/var/log/fpmmm/fpmmm.log'
	);
	if ( ! in_array($default, $aFpmmmLogFileCandidates) ) {
		array_push($aFpmmmLogFileCandidates, $default);
	}

	fprintf(STDOUT, "\nLocation of fpmmm log file.\n");
	fprintf(STDOUT, "\nExample:\n");

	foreach ($aFpmmmLogFileCandidates as $log ) {

		$d = $log == $default ? '*' : ' ';
		fprintf(STDOUT, "%s %s\n", $d, $log);
	}

	fprintf(STDOUT, "\nfpmmm Log File = ");
	$lLogFile = trim(readline());
	if ( $lLogFile == '' ) {
		$lLogFile = $default;
	}

	return array($rc, $lLogFile);
}

// ---------------------------------------------------------------------
function askForFpmmmCacheFileBase($default = '/var/cache/fpmmm')
// ---------------------------------------------------------------------
{
	$rc = OK;

	$aFpmmmCacheFileBaseCandidates = array(
	  '/var/cache/fpmmm'
	);
	if ( ! in_array($default, $aFpmmmCacheFileBaseCandidates) ) {
		array_push($aFpmmmCacheFileBaseCandidates, $default);
	}

	fprintf(STDOUT, "\nLocation of fpmmm cache files.\n");
	fprintf(STDOUT, "\nExample:\n");

	foreach ( $aFpmmmCacheFileBaseCandidates as $cachebase ) {

		$d = $cachebase == $default ? '*' : ' ';
		fprintf(STDOUT, "%s %s\n", $d, $cachebase);
	}

	fprintf(STDOUT, "\nfpmmm Cache File Base = ");
	$lCacheFileBase = trim(readline());
	if ( $lCacheFileBase == '' ) {
		$lCacheFileBase = $default;
	}

	return array($rc, $lCacheFileBase);
}

// ---------------------------------------------------------------------
function askForHostName($default)
// ---------------------------------------------------------------------
{
	$rc = OK;

	$aHostNameCandidates = array(
	  'DB-Server'
	, php_uname('n')
	);
	if ( ! in_array($default, $aHostNameCandidates) ) {
		array_push($aHostNameCandidates, $default);
	}

	fprintf(STDOUT, "\nName of host (= this machine) to monitor.\n");
	fprintf(STDOUT, "This value must match EXACTLY the hostname specified in Zabbix!\n");
	fprintf(STDOUT, "\nExample:\n");

	foreach ( $aHostNameCandidates as $host ) {

		$d = $host == $default ? '*' : ' ';
		fprintf(STDOUT, "%s %s\n", $d, $host);
	}

	fprintf(STDOUT, "\nHost name = ");
	$lHostName = trim(readline());
	if ( $lHostName == '' ) {
		$lHostName = $default;
	}

	return array($rc, $lHostName);
}

// ---------------------------------------------------------------------
function askForInstanceName($default)
// ---------------------------------------------------------------------
{
	$rc = OK;

	$aInstanceNameCandidates = array(
	  'mysqld'
	);
	if ( ! in_array($default, $aInstanceNameCandidates) ) {
		array_push($aInstanceNameCandidates, $default);
	}

	fprintf(STDOUT, "\nName of database (= instance) to monitor.\n");
	fprintf(STDOUT, "This value must match EXACTLY the hostname specified in Zabbix!\n");
	fprintf(STDOUT, "\nExample:\n");

	foreach ( $aInstanceNameCandidates as $instance ) {

		$d = $instance == $default ? '*' : ' ';
		fprintf(STDOUT, "%s %s\n", $d, $instance);
	}

	fprintf(STDOUT, "\nInstance name = ");
	$lInstanceName = trim(readline());
	if ( $lInstanceName == '' ) {
		$lInstanceName = $default;
	}

	return array($rc, $lInstanceName);
}

// ---------------------------------------------------------------------
function checkZabbixHostName($aConfiguration, $pExecutable, $pDebug)
// ---------------------------------------------------------------------
{
	$rc = OK;

	$key = 'FromDual.MySQL' . '.' . 'nonsense';
	$value = 'nonsense';

	$aArgs = array(
	  $pExecutable
	, '--zabbix-server', $aConfiguration['zabbixServer']
	, '--port', strval($aConfiguration['zabbixServerPort'])
	, '--host', $aConfiguration['hostName']
	, '--$key', $key
	, '--value', "'" . $value . "'"
	, '-vv'
	);

	if ( $pDebug > 0 ) {
		print($aArgs);
	}
	$p1 = popen($aArgs, 'r');
	list($stdoutdata, $stderrdata) = $p1.communicate();
	$rc = $p1.returncode;
	$sterrdata = '';
	if ( $pDebug > 0 ) {
		print('rc=' . $rc);
		print('stdout: ' . $stdoutdata);
		print('stderr: ' . $stderrdata);
	}

	return array($rc, $stderrdata);
}

// ---------------------------------------------------------------------
function askForHostType()
// ---------------------------------------------------------------------
{
	$rc = OK;

	$aHostTypeCandidates = array('server', 'database', 'both');
	$default = 'both';

	// print("\nExample: ");

	$question = "\nType of host to monitor: server, database or both (s/d/B)? ";
	$answers = array('s', 'd', 'b');
	$default = 'b';
	$key = answerQuestion($question, $answers, $default);

	if ( $key == 's' ) {
		$lHostType = 'server';
	}
	elseif ( $key == 'd' ) {
		$lHostType = 'database';
	}
	elseif ( $key == 'b' ) {
		$lHostType = 'both';
	}
	else {
		$lHostType = 'error';
		$rc = 1951;
	}

	return array($rc, $lHostType);
}

// ---------------------------------------------------------------------
function askForFpmmmModules($pHostType, $default)
// ---------------------------------------------------------------------
{
	$rc = OK;

	$aHostModules             = 'fpmmm server';
	$aHostModulesExtended     = 'drbd memcached';
	$aDatabaseModules         = 'mysql myisam innodb process security';
	$aDatabaseModulesExtended = 'master slave galera';
	$allModules = $aHostModules . ' ' . $aHostExtended . ' ' . $aDatabaseModules . ' ' . $aDatabaseModulesExtended;

	fprintf(STDOUT, "\nfpmmm modules to use for monitoring (space separated).\n");
	fprintf(STDOUT, "Available modules are: " . $allModules . "\n");
	fprintf(STDOUT, "\nExample:\n");
	fprintf(STDOUT, '  All              : ' . $allModules . "\n");
	fprintf(STDOUT, '  Host             : ' . $aHostModules . "\n");
	fprintf(STDOUT, '  Host extended    : ' . $aHostModulesExtended . "\n");
	fprintf(STDOUT, '  Database         : ' . $aDatabaseModules . "\n");
	fprintf(STDOUT, '  Database extended: ' . $aDatabaseModulesExtended . "\n");
	fprintf(STDOUT, '* Guessed modules  : ' . $default . "\n");

	fprintf(STDOUT, "\nModules = ");
	$lModules = trim(readline());
	if ( $lModules == '' ) {
		$lModules = $default;
	}

	return array($rc, $lModules);
}

// ---------------------------------------------------------------------
function askForFpmmmAgent($default = '/opt/fpmmm/bin/fpmmm')
// ---------------------------------------------------------------------
{
	$rc = OK;

	$aFpmmmAgentCandidates = array(
	  '/opt/fpmmm/bin/fpmmm'
	, '/usr/bin/fpmmm'
	, '/usr/local/bin/fpmmm'
	);
	if ( ! in_array($default, $aFpmmmAgentCandidates) ) {
		array_push($aFpmmmAgentCandidates, $default);
	}

	fprintf(STDOUT, "\nLocation of fpmmm agent.\n");
	fprintf(STDOUT, "\nExample:\n");

	foreach ( $aFpmmmAgentCandidates as $agent ) {

		$d = $agent == $default ? '*' : ' ';
		fprintf(STDOUT, "%s %s\n", $d, $agent);
	}

	fprintf(STDOUT, "\nfpmmm agent = ");
	$lFpmmmAgent = trim(readline());
	if ( $lFpmmmAgent == '' ) {
		$lFpmmmAgent = $default;
	}

	return array($rc, $lFpmmmAgent);
}

// ---------------------------------------------------------------------
function askForFpmmmUser($default = 'zabbix')
// ---------------------------------------------------------------------
{
	$rc = OK;

	$aFpmmmUserCandidates = array(
		'zabbix'
	, 'mysql'
	, 'root'
	, 'fpmmm'
	);

	$me = posix_getpwuid(getmyuid());

	if ( ! in_array($me['name'], $aFpmmmUserCandidates) ) {
		array_push($aFpmmmUserCandidates, $me['name']);
	}

	if ( ! in_array($default, $aFpmmmUserCandidates) ) {
		array_push($aFpmmmUserCandidates, $default);
	}

	fprintf(STDOUT, "\nFpmmm (agent) O/S user.\n");
	fprintf(STDOUT, "\nExample:\n");

	foreach ( $aFpmmmUserCandidates as $user ) {

		if ( $user == $default ) {
			fprintf(STDOUT, '* ' . $user . "\n");
		}
		else {
			fprintf(STDOUT, '  ' . $user . "\n");
		}
	}

	fprintf(STDOUT, "\nfpmmm user = ");
	$lFpmmmUser = trim(readline());
	if ( $lFpmmmUser == '' ) {
		$lFpmmmUser = $default;
	}

	return array($rc, $lFpmmmUser);
}

// ---------------------------------------------------------------------
function askForDatabaseCredentials($pUser = 'fpmmm', $pHost = 'localhost', $pPort = 3306)
// ---------------------------------------------------------------------
{
	$rc = OK;

	// MySQL User

	$aMysqlUserCandidates = array(
		'root'
	, 'fpmmm'
	, 'monitor'
	);
	if ( ! in_array($pUser, $aMysqlUserCandidates) ) {
		array_push($aMysqlUserCandidates, $pUser);
	}

	fprintf(STDOUT, "\nMySQL database user for fpmmm checks.\n");
	fprintf(STDOUT, "\nExample:\n");

	foreach ( $aMysqlUserCandidates as $user ) {

		if ( $user == $pUser ) {
			fprintf(STDOUT, '* ' . $user . "\n");
		}
		else {
			fprintf(STDOUT, '  ' . $user . "\n");
		}
	}

	fprintf(STDOUT, "\nMySQL user = ");
	$lMysqlUser = trim(readline());
	if ( $lMysqlUser == '' ) {
		$lMysqlUser = $pUser;
	}

	// MySQL Password

	fprintf(STDOUT, "\nMySQL password = ");
	$lMysqlPassword = trim(readline());
	// $lMysqlPassword = getPassword($prompt = "\nMySQL password = ");


	// MySQL Host

	$aMySqlHostCandidates = array(
	  'localhost'
	, '127.0.0.1'
	);
	if ( ! in_array(gethostbyname(gethostname()), $aMySqlHostCandidates) ) {
		array_push($aMySqlHostCandidates, gethostbyname(gethostname()));
	}
	if ( ! in_array($pHost, $aMySqlHostCandidates) ) {
		array_push($aMySqlHostCandidates, $pHost);
	}

	fprintf(STDOUT, "\nMySQL host.\n");
	fprintf(STDOUT, "\nExample:\n");

	foreach ( $aMySqlHostCandidates as $host ) {

		if ( $host == $pHost ) {
			fprintf(STDOUT, '* ' . $host . "\n");
		}
		else {
			fprintf(STDOUT, '  ' . $host . "\n");
		}
	}

	fprintf(STDOUT, "\nMySQL host = ");
	$lMysqlHost = trim(readline());
	if ( $lMysqlHost == '' ) {
		$lMysqlHost = $pHost;
	}

	// MySQL Port

	$aMySqlPortCandidates = array(
		3306
	);
	if ( ! in_array($pPort, $aMySqlPortCandidates) ) {
		array_push($aMySqlPortCandidates, $pPort);
	}

	fprintf(STDOUT, "\nMySQL port.\n");
	fprintf(STDOUT, "\nExample:\n");

	foreach  ( $aMySqlPortCandidates as $port ) {

		if ( $port == $pPort ) {
			fprintf(STDOUT, '* ' . $port . "\n");
		}
		else {
			fprintf(STDOUT, '  ' . $port . "\n");
		}
	}

	fprintf(STDOUT, "\nMySQL port = ");
	$lMysqlPort = trim(readline());

	if ( $lMysqlPort == '' ) {
		$lMysqlPort = $pPort;
	}

	return array($rc, $lMysqlUser, $lMysqlPassword, $lMysqlHost, $lMysqlPort);
}

// ---------------------------------------------------------------------
function askForZabbixAdminUser($default = 'Admin')
// ---------------------------------------------------------------------
{
	$rc = OK;

	$aZabbixAdminUserCandidates = array(
		'Admin'
	, 'admin'
	);
	if ( ! in_array($default, $aZabbixAdminUserCandidates) ) {
		array_push($aZabbixAdminUserCandidates, $default);
	}

	fprintf(STDOUT, "\nZabbix administrator user.\n");
	fprintf(STDOUT, "\nExample:\n");

	foreach ( $aZabbixAdminUserCandidates as $user ) {

		$d = $user == $default ? '*' : ' ';
		fprintf(STDOUT, "%s %s\n", $d, $user);
	}

	fprintf(STDOUT, "\nZabbix administrator user = ");
	$lZabbixAdminUser = trim(readline());
	if ( $lZabbixAdminUser == '' ) {
		$lZabbixAdminUser = $default;
	}

	return array($rc, $lZabbixAdminUser);
}

// ---------------------------------------------------------------------
function askForZabbixAdminPassword($default = 'zabbix')
// ---------------------------------------------------------------------
{
	$rc = OK;

	$aZabbixAdminPasswordCandidates = array(
		'zabbix'
	);
	if ( ! in_array($default, $aZabbixAdminPasswordCandidates) ) {
		array_push($aZabbixAdminPasswordCandidates, $default);
	}

	fprintf(STDOUT, "\nZabbix administrator password.\n");
	fprintf(STDOUT, "\nExample:\n");

	foreach ( $aZabbixAdminPasswordCandidates as $password ) {

		$d = $password == $default ? '*' : ' ';
		fprintf(STDOUT, "%s %s\n", $d, $password);
	}

	fprintf(STDOUT, "\nZabbix administrator password = ");
	$lZabbixAdminPassword = trim(readline());
	if ( $lZabbixAdminPassword == '' ) {
		$lZabbixAdminPassword = $default;
	}

	return array($rc, $lZabbixAdminPassword);
}

// ---------------------------------------------------------------------

?>
