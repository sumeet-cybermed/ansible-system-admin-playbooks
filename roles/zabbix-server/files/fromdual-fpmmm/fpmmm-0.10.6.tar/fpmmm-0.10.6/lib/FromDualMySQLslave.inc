<?php

#
# Copyright (C) 2010-2016 FromDual GmbH
#


/*

	getSlaveStatus($pParameter, $dbh, $aSlaveStatus)
	processSlaveInformation($pParameter)

*/

// ---------------------------------------------------------------------
function getSlaveStatus($pParameter, $dbh, &$aSlaveStatus)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '    ' . __FUNCTION__); }

	//       Master_Log_File:
	//   Read_Master_Log_Pos: 4
	//
	//        Relay_Log_File: master-relay-bin.000001
	//         Relay_Log_Pos: 4
	//
	// Relay_Master_Log_File:
	//   Exec_Master_Log_Pos: 0
	//
	//      Slave_IO_Running: No
	//     Slave_SQL_Running: No
	//
	// Seconds_Behind_Master: NULL

	$aSlaveStatus['Read_Master_Log_Pos']   = '0';
	$aSlaveStatus['Relay_Log_Pos']         = '0';
	$aSlaveStatus['Exec_Master_Log_Pos']   = '0';
	$aSlaveStatus['Slave_IO_Running']      = '0';
	$aSlaveStatus['Slave_SQL_Running']     = '0';
	$aSlaveStatus['Seconds_Behind_Master'] = '0';
	$aSlaveStatus['Last_Error']            = '';
	$aSlaveStatus['Last_IO_Error']         = '';
	$aSlaveStatus['Last_SQL_Error']        = '';

	$sql = 'SHOW SLAVE STATUS';

	if ( $result = $dbh->query($sql) ) {

		if ( $row = $result->fetch_assoc() ) {

			$aSlaveStatus['Read_Master_Log_Pos']   = $row['Read_Master_Log_Pos'];
			$aSlaveStatus['Relay_Log_Pos']         = $row['Relay_Log_Pos'];
			$aSlaveStatus['Exec_Master_Log_Pos']   = $row['Exec_Master_Log_Pos'];
			$aSlaveStatus['Slave_IO_Running']      = $row['Slave_IO_Running'];
			$aSlaveStatus['Slave_SQL_Running']     = $row['Slave_SQL_Running'];
			$aSlaveStatus['Seconds_Behind_Master'] = $row['Seconds_Behind_Master'];
			$aSlaveStatus['Last_Error']            = $row['Last_Error'];
			$aSlaveStatus['Last_IO_Error']         = $row['Last_IO_Error'];
			$aSlaveStatus['Last_SQL_Error']        = $row['Last_SQL_Error'];
		}
		else {
			$rc = 1577;
			$msg = 'Instance ' . $pParameter['Hostname'] . ' seems not to be a Slave.';
			mylog($pParameter['LogFile'], WARN, '    ' . $msg . " (rc=$rc).");
		}
	}
	else {
		$rc = 1576;
		$msg = "Invalid query: $sql, Errno: " . $dbh->errno . ': ' .  $dbh->error;
		mylog($pParameter['LogFile'], ERR, '    ' . $msg . " (rc=$rc).");
	}

	$aSlaveStatus['Slave_IO_Running'] = $aSlaveStatus['Slave_IO_Running'] == 'Yes' ? 1 : 0;
	$aSlaveStatus['Slave_SQL_Running'] = $aSlaveStatus['Slave_SQL_Running'] == 'Yes' ? 1 : 0;

	if ( $pParameter['LogLevel'] >= DBG )
	{
		mylog($pParameter['LogFile'], DBG, '    Slave Status:');
		foreach ( array_keys($aSlaveStatus) as $key ) {
			mylog($pParameter['LogFile'], DBG, "      key: $key, value: $aSlaveStatus[$key]");
		}
	}
}

// ---------------------------------------------------------------------
function processSlaveInformation($dbh, $pParameter)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '    ' . __FUNCTION__); }

	$aSlaveStatus = array();
	$aGlobalVariables = array();

	$aSlaveStatusToSend = array(
		'Exec_Master_Log_Pos'
	, 'Read_Master_Log_Pos'
	, 'Relay_Log_Pos'
	, 'Seconds_Behind_Master'
	, 'Slave_IO_Running'
	, 'Slave_SQL_Running'
	, 'Last_Error'
	, 'Last_IO_Error'
	, 'Last_SQL_Error'
	);

	$aGlobalVariablesToSend = array(
		'read_only'
	, 'slave_skip_errors'
	, 'sync_relay_log'
	, 'sync_relay_log_info'
	);

	initValues($aSlaveStatus, $aSlaveStatusToSend);
	initValues($aGlobalVariables, $aGlobalVariablesToSend);

// 	$dbh = getDatabaseConnection($pParameter);

	if ( $dbh === false ) {
		$rc = 1575;
		if ( $pParameter['LogLevel'] >= ERR ) { mylog($pParameter['LogFile'], DBG, "    Database connection failed (rc=$rc)."); }
		return $rc;
	}

	getSlaveStatus($pParameter, $dbh, $aSlaveStatus);
	getGlobalVariables($pParameter, $dbh, $aGlobalVariables);

// 	releaseDatabaseConnection($dbh);

	// Do some post calculations

	// Do some optimizations
	if ( $aSlaveStatus['Last_Error'] == '' ) {
		if ( ($key = array_search('Last_Error', $aSlaveStatusToSend)) !== false ) {
			unset($aSlaveStatusToSend[$key]);
		}
	}
	if ( $aSlaveStatus['Last_IO_Error'] == '' ) {
		if ( ($key = array_search('Last_IO_Error', $aSlaveStatusToSend)) !== false ) {
			unset($aSlaveStatusToSend[$key]);
		}
	}
	if ( $aSlaveStatus['Last_SQL_Error'] == '' ) {
		if ( ($key = array_search('Last_SQL_Error', $aSlaveStatusToSend)) !== false ) {
			unset($aSlaveStatusToSend[$key]);
		}
	}
	// This happens when a Slave is stopped
	if ( is_null($aSlaveStatus['Seconds_Behind_Master']) ) {
		if ( ($key = array_search('Seconds_Behind_Master', $aSlaveStatusToSend)) !== false ) {
			unset($aSlaveStatusToSend[$key]);
		}
	}

	sendData($pParameter, $aSlaveStatus, $aSlaveStatusToSend);
	sendData($pParameter, $aGlobalVariables, $aGlobalVariablesToSend);

	return $rc;
}

?>
