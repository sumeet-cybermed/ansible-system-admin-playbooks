<?php

#
# Copyright (C) 2010-2016 FromDual GmbH
#


/*

	getSecurityInformation($pParameter, $dbh, &$aSecurityInformation)

*/

// ---------------------------------------------------------------------
function getSecurityInformation($pParameter, $dbh, &$aSecurityInformation)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '    ' . __FUNCTION__); }

	$hostname = gethostname();

	$sql = 'SELECT GROUP_CONCAT(CONCAT(user, "@", host) SEPARATOR ", ") AS user FROM mysql.user WHERE user = "root" AND host NOT IN ("localhost", "127.0.0.1", "::1", "' . $hostname . '")';
	if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, '      ' . $sql); }

	if ( $result = $dbh->query($sql) ) {
		if ( $row = $result->fetch_assoc() ) {
			$aSecurityInformation['root_from_remote'] = isset($row['user']) ? $row['user'] : '';
		}
	}
	else {
		$rc = 1756;
		$msg = "Error " . $dbh->errno . " in preparing statement $sql: " . $dbh->error;
		mylog($pParameter['LogFile'], ERR, '    ' . $msg . " (rc=$rc).");
	}

	// Empty user
	$sql = 'SELECT GROUP_CONCAT(CONCAT(user, "@", host) SEPARATOR ", ") AS user FROM mysql.user WHERE user = ""';
	if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, '      ' . $sql); }

	if ( $result = $dbh->query($sql) ) {
		if ( $row = $result->fetch_assoc() ) {
				$aSecurityInformation['empty_user'] = isset($row['user']) ? $row['user'] : '';
		}
	}
	else {
		$rc = 1757;
		$msg = "Error " . $dbh->errno . " in preparing statement $sql: " . $dbh->error;
		mylog($pParameter['LogFile'], ERR, '    ' . $msg . " (rc=$rc).");
	}

	// Test database
	$sql = 'SELECT GROUP_CONCAT(schema_name SEPARATOR ", ") AS "database" FROM information_schema.schemata WHERE schema_name LIKE "test%"';
	if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, '      ' . $sql); }

	if ( $result = $dbh->query($sql) ) {
		if ( $row = $result->fetch_assoc() ) {
				$aSecurityInformation['test_database'] = isset($row['database']) ? $row['database'] : '';
		}
	}
	else {
		$rc = 1758;
		$msg = "Error " . $dbh->errno . " in preparing statement $sql: " . $dbh->error;
		mylog($pParameter['LogFile'], ERR, '    ' . $msg . " (rc=$rc).");
	}

	// User with GRANT privilege
	$sql = 'SELECT GROUP_CONCAT(CONCAT(user, "@", host) SEPARATOR ", ") AS user FROM mysql.user WHERE user != "root" AND grant_priv = "Y"';
	if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, '      ' . $sql); }

	if ( $result = $dbh->query($sql) ) {
		if ( $row = $result->fetch_assoc() ) {
			$aSecurityInformation['user_with_grant_privilege'] = isset($row['user']) ? $row['user'] : '';
		}
	}
	else {
		$rc = 1759;
		$msg = "Error " . $dbh->errno . " in preparing statement $sql: " . $dbh->error;
		mylog($pParameter['LogFile'], ERR, '    ' . $msg . " (rc=$rc).");
	}

	// User with SUPER privilege
	$sql = 'SELECT GROUP_CONCAT(CONCAT(user, "@", host) SEPARATOR ", ") AS user FROM mysql.user WHERE user != "root" AND super_priv = "Y"';
	if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, '      ' . $sql); }

	if ( $result = $dbh->query($sql) ) {
		if ( $row = $result->fetch_assoc() ) {
				$aSecurityInformation['user_with_super_privilege'] = isset($row['user']) ? $row['user'] : '';
		}
	}
	else {
		$rc = 1760;
		$msg = "Error " . $dbh->errno . " in preparing statement $sql: " . $dbh->error;
		mylog($pParameter['LogFile'], ERR, '    ' . $msg . " (rc=$rc).");
	}

	// User with CREATE USER privilege
	$sql = 'SELECT GROUP_CONCAT(CONCAT(user, "@", host) SEPARATOR ", ") AS user FROM mysql.user WHERE user != "root" AND create_user_priv = "Y"';
	if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, '      ' . $sql); }

	if ( $result = $dbh->query($sql) ) {
		if ( $row = $result->fetch_assoc() ) {
				$aSecurityInformation['user_with_create_user_privilege'] = isset($row['user']) ? $row['user'] : '';
		}
	}
	else {
		$rc = 1761;
		$msg = "Error " . $dbh->errno . " in preparing statement $sql: " . $dbh->error;
		mylog($pParameter['LogFile'], ERR, '    ' . $msg . " (rc=$rc).");
	}

	if ( $pParameter['LogLevel'] >= DBG ) {
		mylog($pParameter['LogFile'], DBG, '    Security Information:');
		foreach ( array_keys($aSecurityInformation) as $key ) {
			mylog($pParameter['LogFile'], DBG, "      key: $key, value: " . $aSecurityInformation[$key]);
		}
	}

	return $rc;
}

?>
