<?php

#
# Copyright (C) 2010-2016 FromDual GmbH
#


/*

	processMemcachedInformation($pParameter)

*/

// ---------------------------------------------------------------------
function processMemcachedInformation($pParameter)
// ---------------------------------------------------------------------
{
	$rc = OK;
	$lDistribution = 'unknown';

	$lOs = getOs();
	if ( $lOs == 'Linux' ) {
		$lDistribution = getDistribution();
	}

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '    ' . __FUNCTION__); }

	// todo: this should be moved to the memcached section!
	if ( ! in_array('memcached', get_loaded_extensions()) ) {
		$rc = 1325;
		if ( $pParameter['LogLevel'] >= WARN ) {
			mylog($pParameter['LogFile'], WARN, "    The module memcaced is not installed. You can continue");
			mylog($pParameter['LogFile'], WARN, "    without but you cannot use memcached monitoring functionality.");
			mylog($pParameter['LogFile'], WARN, "    Please fix this as follows:");

			switch ($lDistribution) {
			case 'CentOS':
			case 'RedHat':
			case 'Fedora':
				$cmd = 'sudo yum install php-memcached';
				mylog($pParameter['LogFile'], WARN, "    shell> $cmd\n");
				break;
			case 'SUSE':
			case 'openSUSE project':
				$cmd = 'sudo zypper install php5-memcached';
				mylog($pParameter['LogFile'], WARN, "    shell> $cmd\n");
				break;
			case 'Ubuntu':
			case 'Debian':
				$cmd = 'sudo apt-get install php5-memcached';
				mylog($pParameter['LogFile'], WARN, "    shell> $cmd\n");
				break;
			default:
				$cmd = 'echo "Unknown distribution: ' . $lDistribution . '"';
				mylog($pParameter['LogFile'], WARN, "    OS/Distribution $lOs/$lDistribution is not yet supported.\n");
			}
		}
		return $rc;
	}


	$aMemcachedStatusFields = array(
	  'uptime', 'time', 'version', 'pointer_size'
	, 'rusage_user_seconds', 'rusage_user_microseconds'
	, 'rusage_system_seconds', 'rusage_system_microseconds'
	, 'curr_connections', 'total_connections', 'connection_structures'
	, 'cmd_get', 'cmd_set', 'get_hits', 'get_misses'
	, 'bytes_read', 'bytes_written', 'limit_maxbytes'
	, 'threads', 'bytes', 'curr_items'
	, 'total_items', 'evictions'
	);

	$client = new Memcached();
	$client->addServer($pParameter['MemcachedHost'], $pParameter['MemcachedPort']);
	$aMemcached = $client->getStats();
	$aMemcachedStats = $aMemcached[$pParameter['MemcachedHost'] . ':' . $pParameter['MemcachedPort']];

	//check if memcached looks up and running

	if ( count($aMemcachedStats) == 0 ) {
		if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], DBG, "Memcached seems to be down."); }
		return $rc;
	}

	$aMemcachedStatus = array();

	foreach ( $aMemcachedStatusFields as $field ) {
		$aMemcachedStatusToSend ['memcached.' . $field] = $field;
		$aMemcachedStatus['memcached.' . $field]        = $aMemcachedStats[$field];
	}

	$rc = sendData($pParameter, $aMemcachedStatus, $aMemcachedStatusToSend);
	return $rc;
}

?>
