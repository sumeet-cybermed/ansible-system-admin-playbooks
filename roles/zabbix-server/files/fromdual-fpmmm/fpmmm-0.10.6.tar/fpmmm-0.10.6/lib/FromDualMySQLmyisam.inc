<?php

#
# Copyright (C) 2010-2016 FromDual GmbH
#


/*

	processMyisamInformation($pParameter)

*/

// ---------------------------------------------------------------------
function processMyisamInformation($dbh, $pParameter)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '    ' . __FUNCTION__); }

	$hGlobalVariables = array();
	$hGlobalStatus = array();

	$aGlobalVariablesToSend = array(
		'key_buffer_size'
	, 'key_buffer_blocks'
	, 'key_cache_block_size'
	, 'flush_time'
	);

	$aGlobalStatusToSend = array(
		'Key_blocks_not_flushed'
	, 'Key_blocks_unused'
	, 'Key_blocks_not_flushed'
	, 'Key_blocks_unused'
	, 'Key_blocks_used'
	, 'Key_blocks_used_now'
	, 'Key_blocks_warm'
	, 'Key_blocks_not_flushed_b'
	, 'Key_blocks_unused_b'
	, 'Key_blocks_not_flushed_b'
	, 'Key_blocks_unused_b'
	, 'Key_blocks_used_b'
	, 'Key_blocks_used_now_b'
	, 'Key_blocks_warm_b'
	, 'Key_buffer_hit_rate'
	, 'Key_read_requests'
	, 'Key_reads'
	, 'Key_write_requests'
	, 'Key_writes'
	);

	initValues($hGlobalVariables, $aGlobalVariablesToSend);
	initValues($hGlobalStatus, $aGlobalStatusToSend);

	// $dbh = getDatabaseConnection($pParameter);

	if ( $dbh == false ) {
		$rc = 1375;
		if ( $pParameter['LogLevel'] >= ERR ) { mylog($pParameter['LogFile'], DBG, "    Database connection failed (rc=$rc)."); }
		return $rc;
	}

	getGlobalVariables($pParameter, $dbh, $hGlobalVariables);
	getGlobalStatus($pParameter, $dbh, $hGlobalStatus);

	// releaseDatabaseConnection($dbh);

	// Do some post calculations
	$hGlobalVariables['key_buffer_blocks'] = $hGlobalVariables['key_cache_block_size'] == 0 ? 0 : floor($hGlobalVariables['key_buffer_size'] / $hGlobalVariables['key_cache_block_size']);
	$hGlobalStatus['Key_blocks_used_now']  = $hGlobalVariables['key_buffer_blocks'] - $hGlobalStatus['Key_blocks_unused'];
	$hGlobalStatus['Key_blocks_not_flushed_b'] = $hGlobalVariables['key_cache_block_size'] * $hGlobalStatus['Key_blocks_not_flushed'];
	$hGlobalStatus['Key_blocks_unused_b'] = $hGlobalVariables['key_cache_block_size'] * $hGlobalStatus['Key_blocks_unused'];
	$hGlobalStatus['Key_blocks_used_b'] = $hGlobalVariables['key_cache_block_size'] * $hGlobalStatus['Key_blocks_used'];
	$hGlobalStatus['Key_blocks_used_now_b'] = $hGlobalVariables['key_cache_block_size'] * $hGlobalStatus['Key_blocks_used_now'];
	$hGlobalStatus['Key_blocks_warm_b'] = $hGlobalVariables['key_cache_block_size'] * $hGlobalStatus['Key_blocks_warm'];

	if ( ($hGlobalStatus['Key_read_requests'] + $hGlobalStatus['Key_reads']) == 0 ) {
		$hGlobalStatus['Key_buffer_hit_rate'] = 0.0;
	}
	else {
		$hGlobalStatus['Key_buffer_hit_rate'] = round($hGlobalStatus['Key_read_requests'] / ($hGlobalStatus['Key_read_requests'] + $hGlobalStatus['Key_reads']) * 100.0, 2);
	}

	sendData($pParameter, $hGlobalVariables, $aGlobalVariablesToSend);
	sendData($pParameter, $hGlobalStatus, $aGlobalStatusToSend);

	return $rc;
}

?>
