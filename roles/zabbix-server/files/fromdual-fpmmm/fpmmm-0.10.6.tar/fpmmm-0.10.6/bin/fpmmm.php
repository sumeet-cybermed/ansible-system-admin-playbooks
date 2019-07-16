#!/usr/bin/php
<?php

#
# Copyright (C) 2010-2016 FromDual GmbH
#


set_include_path(get_include_path() . PATH_SEPARATOR . dirname(dirname(__FILE__)));

include('lib/Constants.inc');
include('lib/config.inc');
include('lib/log.inc');
include('lib/myEnv.inc');
include('lib/FromDualMySQLagent.inc');
include('lib/sendData.inc');
include('lib/writeDataToCacheFile.inc');
include('lib/uploadData.inc');
include('lib/fpmmmInstaller.inc');

$rc = OK;

$lMyName = __FILE__;
$lMyNameBase = basename($lMyName, '.php');
$basedir = dirname($lMyName);

// This is needed to find zabbix_sender!

$_ENV['PATH'] .= $_ENV['PATH'] . ':/usr/local/bin';


// Options

$shortopts  = '';

$longopts  = array(
  'config:'
, 'daemon'
, 'help'
, 'version'
);

// The parsing of options will end at the first non-option found, any-
// thing that follows is discarded. 
$aOptions = getopt($shortopts, $longopts);

$aDefaultOptions = array(
  'config'   => '/etc/zabbix/fpmmm.conf'
, 'interval' => 60
);

if ( isset($aOptions['help']) ) {
  printUsage($lMyNameBase . '.php', $aDefaultOptions);
  exit($rc);
}

if ( isset($aOptions['version']) ) {
  printf("%s\n", RELEASE);
  exit($rc);
}

// Check if options are entered correctly
// There should be one more argv (0) than Parameters
if ( (count($argv) - 1) != count($aOptions) ) {
	$rc = 1000;
	fprintf(STDERR, "Wrong amount of options (rc=%d).\n", $rc);
	exit($rc);
}


// Configuration file

$lConfFile = '';
if ( isset($aOptions['config']) && ($aOptions['config'] != '') ) {
	$lConfFile = $aOptions['config'];
}
else {
	$lConfFile = $basedir . '/etc/' . $lMyNameBase . '.conf';
}

// Daemon

if ( isset($aOptions['daemon']) ) {
	$rc = 1001;
	fprintf(STDERR, "fpmmm daemon mode is not supported yet (rc=%d).\n", $rc);
	exit($rc);
}


$gDomain = 'FromDual.MySQL';
$gRemoveLockFile = 0;

$aConfiguration = readConfigurationFile(array('LogLevel' => WARN, 'LogFile' => 'none'), $lConfFile);

$sections = getSections($aConfiguration);
// Read the default section here
$defaults = getDefaultParameter($aConfiguration, 'default');

$gParameter = $defaults;

$lDefaultsLogFile  = $defaults['LogFile'];
$lDefaultsLogLevel = $defaults['LogLevel'];

// Touch logfile

$dir = dirname($lDefaultsLogFile);
if ( ! is_dir($dir) ) {
	mkdir($dir);
}

if ( ! $LOG = @fopen($lDefaultsLogFile, 'a+') ) {
	$rc = 1002;
	fprintf(STDERR, "ERROR: Open of file %s failed (rc=%d).  \n", $lDefaultsLogFile, $rc);
	fprintf(STDERR, "ERROR: Possibly I cannot create this file because of missing privileges, wrong user or directory does not exists?\n");
	cleanAndQuit($gParameter, $gRemoveLockFile, $rc);
}
fclose($LOG);


// Here we start logging

mylog($lDefaultsLogFile, INFO, "FromDual Performance Monitor for MySQL and MariaDB (fpmmm) (" . RELEASE . ") run started.");

// Set Lock file here, gRemoveLockFile becomes 0 or 1 now:
$ret = setAgentLock($gParameter, $gRemoveLockFile);

$ret = checkFpmmmRequirements($gParameter, $gRemoveLockFile);

if ( $lDefaultsLogLevel >= INFO ) {
	mylog($lDefaultsLogFile, INFO, "  Read configuration from $lConfFile");
	mylog($lDefaultsLogFile, INFO, '  Sections found: ' . join(', ', $sections));
	mylog($lDefaultsLogFile, INFO, "  Reading default section");
}
if ( $lDefaultsLogLevel >= DBG ) {

	foreach ( array_keys($defaults) as $key ) {

		// This would be possibly a security problem...
		if ( $key == 'Password' ) {
			mylog($lDefaultsLogFile, DBG, "  $key - ********");
		}
		else {
			mylog($lDefaultsLogFile, DBG, "  $key - $defaults[$key]");
		}
	}
}

foreach ( $sections as $section ) {

	if ( $lDefaultsLogLevel >= INFO ) { mylog($lDefaultsLogFile, INFO, "  Processing section $section now"); }

	// skip default section, we read it already above
	if ( $section == 'default' ) {
		continue;
	}

	$parameter = getParameter($aConfiguration, $section);

	if ( array_key_exists('Debug', $parameter) ) {
		mylog($parameter['LogFile'], WARN, "  Parameter Debug in section $section is deprecated and will be removed in a later release.");
		mylog($parameter['LogFile'], WARN, "  Please use LogLevel instead.");
	}

	if ( $parameter['LogLevel'] >= DBG ) {

		mylog($parameter['LogFile'], DBG, "  The combined (default + section) parameters for section $section are:");
		foreach ( array_keys($parameter) as $key ) {

			// This would be possibly a security problem...
			if ( $key == 'Password' ) {
				mylog($parameter['LogFile'], DBG, "  $key - ********");
			}
			else {
				mylog($parameter['LogFile'], DBG, "  $key - " . $parameter[$key]);
			}
		}
	}


	// Skip section if it is disabled

	if ( $parameter['Disabled'] == 'true' ) {

		if ( $parameter['LogLevel'] >= INFO ) {
			mylog($parameter['LogFile'], INFO, "  Skipping section: $section because it is disabled.");
		}
		continue;
	}


	// Check if parameters are correct

	$dir = dirname($parameter['CacheFileBase']);
	if ( is_dir($dir) === false ) {
		if ( @mkdir($dir, 0777, true) === false ) {
			$rc = 1005;
			mylog($parameter['LogFile'], ERR, "  Cannot create directory $dir in section $section (rc=$rc).");
			continue;
		}
	}

	// ugly way to find out if this is a host or a database
	// todo: this should be fixed later by type option
	$dbh = false;
	// fpmmm server
	$m = explode(' ' , $parameter['Modules']);
	if ( (! in_array('fpmmm', $m)) && (! in_array('server', $m)) ) {

		$dbh = getDatabaseConnection($parameter);

		if ( $dbh === false ) {

			$rc = 1006;
			$msg = 'Database connection for ' . $section . ' failed';
			if ( $parameter['LogLevel'] >= ERR ) { mylog($parameter['LogFile'], ERR, "    $msg (rc=$rc)."); }
			$rc = OK;

			$hGlobalVariables['alive'] = 0;
		}
		else {
			$hGlobalVariables['alive'] = 1;
		}
		$aGlobalVariablesToSend = array('alive');
		$parameter['Domain'] = $gDomain . '.' . 'mysql';
		$ret = sendData($parameter, $hGlobalVariables, $aGlobalVariablesToSend);
	}


	if ( $parameter['LogLevel'] >= INFO ) { mylog($parameter['LogFile'], INFO, "  Modules for section $section: " . $parameter['Modules']); }

	$modules = preg_split('/\s+/', $parameter['Modules']);
	foreach ( $modules as $module ) {

		// FromDual.MySQL.xxx
		$parameter['Domain'] = $gDomain . '.' . $module;

		if ( ($parameter['LogLevel'] >= INFO) ) { mylog($parameter['LogFile'], INFO, "  Processing module $module for section $section now..."); }

		$class = 'FromDualMySQL' . $module;
		if ( $module == 'mpm' ) {
			$class = 'FromDualMySQL' . 'fpmmm';
		}
		$mod = dirname(dirname(__FILE__)) . '/lib/' . $class . '.inc';
		if ( ! is_readable($mod) ) {
			$rc = 1003;
			mylog($parameter['LogFile'], ERR, "  Module FromDualMySQL" . $module . " does not exist (rc=$rc).");
		}
		else {

			require_once('lib/' . $class . '.inc');

			$gParameter = $parameter;

			switch ( $module ) {
			case 'aria':
				if ( $dbh !== false ) {
					$rc = processAriaInformation($dbh, $parameter);
				}
				else {
					$rc = OK;
				}
				break;
			case 'drbd':
				$rc = processDrbdInformation($parameter);
				break;
			case 'galera':
				if ( $dbh !== false ) {
					$rc = processGaleraInformation($dbh, $parameter);
				}
				else {
					$rc = OK;
				}
				break;
			case 'innodb':
				if ( $dbh !== false ) {
					$rc = processInnodbInformation($dbh, $parameter);
				}
				else {
					$rc = OK;
				}
				break;
			case 'master':
				if ( $dbh !== false ) {
					$rc = processMasterInformation($dbh, $parameter);
				}
				else {
					$rc = OK;
				}
				break;
			case 'memcached':
				$rc = processMemcachedInformation($parameter);
				break;
			case 'mpm':
			case 'fpmmm':
				if ( $module == 'mpm' ) {
					mylog($parameter['LogFile'], WARN, "  Module mpm in section $section is deprecated and will be removed in a later release.");
					mylog($parameter['LogFile'], WARN, "  Please use fpmmm instead.");
				}
				$rc = processFpmmmInformation($parameter);
				break;
			case 'myisam':
				if ( $dbh !== false ) {
					$rc = processMyisamInformation($dbh, $parameter);
				}
				else {
					$rc = OK;
				}
				break;
			case 'mysql':
				if ( $dbh !== false ) {
					$rc = processMysqlInformation($dbh, $parameter);
				}
				else {
					$rc = OK;
				}
				break;
			case 'process':
				if ( $dbh !== false ) {
					require_once('lib/FromDualMySQLserver.inc');
					$rc = processProcessInformation($parameter);
				}
				else {
					$rc = OK;
				}
				break;
			case 'security':
				if ( $dbh !== false ) {
					$rc = processSecurityInformation($dbh, $parameter);
				}
				else {
					$rc = OK;
				}
				break;
			case 'server':
				$rc = processServerInformation($parameter, $gDomain);
				break;
			case 'slave':
				if ( $dbh !== false ) {
					$rc = processSlaveInformation($dbh, $parameter);
				}
				else {
					$rc = OK;
				}
				break;
			default:
				$rc = 1004;
				mylog($parameter['LogFile'], ERR, "    Module FromDualMySQL" . $module . " does not exist (rc=$rc).");
			}

			if ( $rc != OK ) {
				mylog($parameter['LogFile'], ERR, "    Module FromDualMySQL" . $module . " got an error (rc=$rc).");
			}

			if ( $lDefaultsLogLevel >= INFO ) { mylog($parameter['LogFile'], INFO, "  Processing module $module for section $section finished."); }

		}   // call module
	}   // module loop

	if ( $dbh !== false ) {
		releaseDatabaseConnection($dbh);
	}
}   // sections loop

$ret = removeAgentLock($gParameter, $gRemoveLockFile);
cleanAndQuit($gParameter, $gRemoveLockFile, $rc);

?>
