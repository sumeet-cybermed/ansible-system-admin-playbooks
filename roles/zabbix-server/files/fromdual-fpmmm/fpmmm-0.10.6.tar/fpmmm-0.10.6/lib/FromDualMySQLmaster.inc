<?php

#
# Copyright (C) 2010-2016 FromDual GmbH
#


/*

	getMasterStatus($aParameter, $dbh, &$aMasterStatus)
	getBinaryLogs($aParameter, $dbh, &$aBinaryLogs)
	getSlaveCount($aParameter, $dbh, &$aSlaveHosts)
	processMasterInformation($aParameter)

*/

// ---------------------------------------------------------------------
function getMasterStatus($aParameter, $dbh, &$aMasterStatus)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $aParameter['LogLevel'] >= INFO ) { mylog($aParameter['LogFile'], INFO, '    ' . __FUNCTION__); }

	// +----------------+----------+--------------+------------------+
	// | File           | Position | Binlog_Do_DB | Binlog_Ignore_DB |
	// +----------------+----------+--------------+------------------+
	// | bin-log.000001 |     1676 |              |                  |
	// +----------------+----------+--------------+------------------+

	$aMasterStatus['Binlog_position']      = '0';
	$aMasterStatus['Binlog_file']          = '';
	$aMasterStatus['Binlog_number']        = '0';
	$aMasterStatus['Binlog_do_filter']     = '';
	$aMasterStatus['Binlog_ignore_filter'] = '';

	$sql = 'SHOW MASTER STATUS';

	if ( $result = $dbh->query($sql) ) {

		if ( $row = $result->fetch_assoc() ) {

			$aMasterStatus['Binlog_position']  = intval($row['Position']);
			$aMasterStatus['Binlog_file']      = strval($row['File']);

			if ( preg_match("/^.*\.(\d{4,6})$/", $aMasterStatus['Binlog_file'], $matches) ) {
				$aMasterStatus['Binlog_number'] = intval($matches[1]);
			}
			else {
				$rc = 1304;
				$msg = "Could not parse binlog file: " . $aMasterStatus['Binlog_file'];
				mylog($aParameter['LogFile'], ERR, '    ' . $msg . " (rc=$rc).");
				$aMasterStatus['Binlog_number'] = 0;
			}
			$aMasterStatus['Binlog_do_filter']     = $row['Binlog_Do_DB'];
			$aMasterStatus['Binlog_ignore_filter'] = $row['Binlog_Ignore_DB'];
		}
	}
	else {
		$rc = 1301;
		$msg = "Invalid query: $sql, Errno: " . $dbh->errno . ': ' .  $dbh->error;
		mylog($aParameter['LogFile'], ERR, '    ' . $msg . " (rc=$rc).");
	}

	if ( $aParameter['LogLevel'] >= DBG )
	{
		mylog($aParameter['LogFile'], DBG, '    Master status:');
		foreach ( array_keys($aMasterStatus) as $key ) {
			mylog($aParameter['LogFile'], DBG, "      key: $key, value: " . $aMasterStatus[$key]);
		}
	}
	return $rc;
}

// ---------------------------------------------------------------------
function getBinaryLogs($aParameter, $dbh, &$aBinaryLogs)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $aParameter['LogLevel'] >= INFO ) { mylog($aParameter['LogFile'], INFO, '    ' . __FUNCTION__); }

	// +----------------+-----------+
	// | Log_name       | File_size |
	// +----------------+-----------+
	// | bin-log.000001 |      1717 |
	// | bin-log.000002 |       107 |
	// +----------------+-----------+

	$aBinaryLogs['Binlog_count'] = '0';
	$aBinaryLogs['Binlog_total_size'] = '0';

	$sql = 'SHOW BINARY LOGS';
	if ( $result = $dbh->query($sql) ) {

		while ( $row = $result->fetch_assoc() ) {
				$aBinaryLogs['Binlog_count'] += 1;
				$aBinaryLogs['Binlog_total_size'] += $row['File_size'];
		}
	}
	else {
		$rc = 1302;
		$msg = "Invalid query: $sql, Errno: " . $dbh->errno . ': ' .  $dbh->error;
		mylog($aParameter['LogFile'], ERR, '    ' . $msg . " (rc=$rc).");
	}

	if ( $aParameter['LogLevel'] >= DBG ) {
		mylog($aParameter['LogFile'], DBG, '    Master status:');
		foreach ( array_keys($aBinaryLogs) as $key ) {
			mylog($aParameter['LogFile'], DBG, "      key: $key, value: $aBinaryLogs[$key]");
		}
	}
	return $rc;
}

// ---------------------------------------------------------------------
function getSlaveCount($aParameter, $dbh, &$aSlaveHosts)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $aParameter['LogLevel'] >= INFO ) { mylog($aParameter['LogFile'], INFO, '    ' . __FUNCTION__); }

	// +------------+-----------+------+-----------+
	// | Server_id  | Host      | Port | Master_id |
	// +------------+-----------+------+-----------+
	// |  192168010 | iconnect2 | 3306 | 192168011 |
	// | 1921680101 | athena    | 3306 | 192168011 |
	// +------------+-----------+------+-----------+

	$aSlaveHosts['Slave_count'] = '0';

	$sql = 'SHOW SLAVE HOSTS';
	if ( $result = $dbh->query($sql) ) {
		while ( $row = $result->fetch_assoc() ) {
			$aSlaveHosts['Slave_count'] += 1;
		}
	}
	else {
		$rc = 1303;
		$msg = "Invalid query: $sql, Errno: " . $dbh->errno . ': ' .  $dbh->error;
		mylog($aParameter['LogFile'], ERR, '    ' . $msg . " (rc=$rc).");
	}

	if ( $aParameter['LogLevel'] >= DBG ) {
		mylog($aParameter['LogFile'], DBG, '    Master status:');
		foreach ( array_keys($aSlaveHosts) as $key ) {
			mylog($aParameter['LogFile'], DBG, "      key: $key, value: $aSlaveHosts[$key]");
		}
	}
	return $rc;
}

// ---------------------------------------------------------------------
function processMasterInformation($dbh, $aParameter)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $aParameter['LogLevel'] >= INFO ) { mylog($aParameter['LogFile'], INFO, '    ' . __FUNCTION__); }

	$aMasterStatusToSend = array(
		'Binlog_avg_event_size'
	, 'Binlog_count'
	, 'Binlog_do_filter'
	, 'Binlog_ignore_filter'
	, 'Binlog_event_count'
	, 'Binlog_file'
	, 'Binlog_number'
	, 'Binlog_position'
	, 'Binlog_total_size'
	, 'Slave_count'
	);

	$aGlobalVariablesToSend = array(
		'auto_increment_increment'
	, 'auto_increment_offset'
	, 'binlog_row_image'
	, 'binlog_format'
	, 'binlog_cache_size'
	, 'binlog_stmt_cache_size'
	, 'expire_logs_days'
	, 'log_bin'
	, 'sync_binlog'
	, 'sync_master_info'
	);

	$ret = initValues($aMasterStatus, $aMasterStatusToSend);
	$ret = initValues($aGlobalVariables, $aGlobalVariablesToSend);

	// Set default values

	$aGlobalVariables['binlog_row_image'] = 'FULL';
	$aGlobalVariables['binlog_format']    = 'STATEMENT';

// 	$dbh = getDatabaseConnection($aParameter);

	if ( $dbh === false ) {
		$rc = 1300;
		if ( $aParameter['LogLevel'] >= ERR ) { mylog($aParameter['LogFile'], DBG, "    Database connection failed (rc=$rc)."); }
		return $rc;
	}

	$ret = getMasterStatus($aParameter, $dbh, $aMasterStatus);
	if ( $ret != OK ) {
		$rc = 1305;
		$msg = "Function getMasterStatus failed";
		mylog($aParameter['LogFile'], ERR, '    ' . $msg . " (rc=$rc).");
	}
	
	// No Binary Logging is enabled
	if ( $aMasterStatus['Binlog_file'] == '' ) {
		$rc = 1308;
		$msg = "Binary Log is disabled. Module 'master' does not make sense for host " . $aParameter['Hostname'] . "...";
		mylog($aParameter['LogFile'], WARN, '    ' . $msg . " (rc=$rc).");
		$rc = OK;
	}
	else {

		$ret = getBinaryLogs($aParameter, $dbh, $aMasterStatus);
		if ( $ret != OK ) {
			$rc = 1306;
			$msg = "Function getMasterStatus failed";
			mylog($aParameter['LogFile'], ERR, '    ' . $msg . " (rc=$rc).");
		}
	}

	$ret = getSlaveCount($aParameter, $dbh, $aMasterStatus);
	if ( $ret != OK ) {
		$rc = 1307;
		$msg = "Function getMasterStatus failed";
		mylog($aParameter['LogFile'], ERR, '    ' . $msg . " (rc=$rc).");
	}

	$ret = getGlobalVariables($aParameter, $dbh, $aGlobalVariables);

// 	$ret = releaseDatabaseConnection($dbh);

	// Do some post calculations
	if ( $aMasterStatus['Binlog_file'] == '' ) {
		$aMasterStatus['Binlog_file'] = '';
	}

	$ret = sendData($aParameter, $aMasterStatus, $aMasterStatusToSend);
	$ret = sendData($aParameter, $aGlobalVariables, $aGlobalVariablesToSend);

	return $rc;
}

?>
