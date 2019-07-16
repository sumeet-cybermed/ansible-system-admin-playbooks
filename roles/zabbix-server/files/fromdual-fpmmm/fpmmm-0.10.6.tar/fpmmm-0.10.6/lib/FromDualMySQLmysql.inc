<?php

#
# Copyright (C) 2010-2016 FromDual GmbH
#


/*

	getProcesslist($aParameter, $dbh, $aGlobalVariables)
	processProcesslistInformation($dbh, $aParameter, $aGlobalVariables)
	processMysqlInformation($dbh, $pParameter)

*/

// ---------------------------------------------------------------------
function getProcesslist($aParameter, $dbh, $aGlobalVariables)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $aParameter['LogLevel'] >= INFO ) { mylog($aParameter['LogFile'], INFO, '    ' . __FUNCTION__); }

	// ON
	$lPerformanceSchema = $aGlobalVariables['performance_schema'];

	// MySQL Community Server (GPL)
	$lVersionComment = $aGlobalVariables['version_comment'];
	$lBranch = extractBranch($lVersionComment);
	if ( $lBranch == 'unknown' ) {
		$rc = 1402;
		$msg = "Branch is $lBranch. (rc=$rc)";
		mylog($aParameter['LogFile'], ERR, '    ' . $msg);
		return array($rc, null);
	}

	// Seems to be irrelevant
	// $aGlobalVariables['mr_version']

	// If P_S is ON use this
	if ( $lPerformanceSchema == 'ON' ) {

		// MySQL 5.7.9
		// MySQL 5.6.25
		// MariaDB 10.0.21 (with P_S)
		// MariaDB 10.1.9 (with P_S)
		// MariaDB 10.2.0 (with P_S)

		// Is NOT usefull because P_S stages are disabled by default!
		$sql = "SELECT processlist_state AS state, COUNT(*) AS cnt
  FROM performance_schema.threads
 WHERE type = 'FOREGROUND'
   AND processlist_user IS NOT NULL
   AND processlist_command != 'Sleep'
   AND processlist_id != CONNECTION_ID()
 GROUP BY processlist_state
 ORDER BY processlist_state";

		$sql = "SELECT state, COUNT(*) AS cnt
  FROM information_schema.processlist
 WHERE command != 'Sleep'
   AND id != CONNECTION_ID()
   AND user != 'system user'
   AND user != 'event_scheduler'
 GROUP BY state
 ORDER BY state";
	}
	// otherwise use I_S
	else {

		// MySQL 5.1.73
		// MySQL 5.5.38
		// MariaDB 10.2.0 (w/o P_S)
		// MariaDB 10.1.9 (w/o P_S)
		// MariaDB 10.0.21 (w/o P_S)

		$sql = "SELECT state, COUNT(*) AS cnt
  FROM information_schema.processlist
 WHERE command != 'Sleep'
   AND id != CONNECTION_ID()
   AND user != 'system user'
   AND user != 'event_scheduler'
 GROUP BY state
 ORDER BY state";
	}

	if ( $result = $dbh->query($sql) ) {

		while ( $row = $result->fetch_assoc() ) {
				$aProcesslist[$row['state']] = $row['cnt'];
		}
	}
	else {
		$rc = 1401;
		$msg = "Invalid query: $sql, Errno: " . $dbh->errno . ': ' .  $dbh->error . " (rc=$rc).";
		mylog($aParameter['LogFile'], ERR, '    ' . $msg);
		return array($rc, null);
	}

	if ( $aParameter['LogLevel'] >= DBG ) {
		$msg = 'Processlist:';
		mylog($aParameter['LogFile'], DBG, '    ' . $msg);
		foreach ( $aProcesslist as $key => $value ) {
			$msg = "key: $key, value: " . $value;
			mylog($aParameter['LogFile'], DBG, "      " . $msg);
		}
	}

	return array($rc, $aProcesslist);
}

// ---------------------------------------------------------------------
function processProcesslistInformation($dbh, $aParameter, $aGlobalVariables)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $aParameter['LogLevel'] >= INFO ) { mylog($aParameter['LogFile'], INFO, '    ' . __FUNCTION__); }

	$aProcesslistToSend = array(
	  'other'
	, 'After create'
	, 'Analyzing'
	, 'checking permissions'
	, 'Checking table'
	, 'cleaning up'
	, 'closing tables'
	, 'converting HEAP to MyISAM'
	, 'copy to tmp table'
	, 'Copying to group table'
	, 'Copying to tmp table'
	, 'altering table'
	, 'Copying to tmp table on disk'
	, 'Creating index'
	, 'Creating sort index'
	, 'creating table'
	, 'Creating tmp table'
	, 'committing alter table to storage engine'
	, 'deleting from main table'
	, 'deleting from reference tables'
	, 'discard_or_import_tablespace'
	, 'end'
	, 'executing'
	, 'Execution of init_command'
	, 'freeing items'
	, 'FULLTEXT initialization'
	, 'init'
	, 'Killed'
	, 'logging slow query'
	, 'login'
	, 'manage keys'
	, 'NULL'
	, 'Opening tables'
	, 'optimizing'
	, 'preparing'
	, 'Purging old relay logs'
	, 'query end'
	, 'Reading from net'
	, 'Receiving from client'
	, 'Removing duplicates'
	, 'removing tmp table'
	, 'rename'
	, 'rename result table'
	, 'Reopen tables'
	, 'Repair by sorting'
	, 'preparing for alter table'
	, 'Repair done'
	, 'Repair with keycache'
	, 'Rolling back'
	, 'Saving state'
	, 'Searching rows for update'
	, 'Sending data'
	, 'Sending to client'
	, 'setup'
	, 'Sorting for group'
	, 'Sorting for order'
	, 'Sorting index'
	, 'Sorting result'
	, 'statistics'
	, 'System lock'
	, 'update'
	, 'Updating'
	, 'updating main table'
	, 'updating reference tables'
	, 'User lock'
	, 'User sleep'
	, 'Waiting for commit lock'
	, 'Waiting for global read lock'
	, 'Waiting for tables'
	, 'Waiting for table flush'
	, 'Waiting for lock_type lock'
	, 'Waiting for table level lock'
	, 'Waiting for event metadata lock'
	, 'Waiting for global read lock'
	, 'Waiting for schema metadata lock'
	, 'Waiting for stored function metadata lock'
	, 'Waiting for stored procedure metadata lock'
	, 'Waiting for table metadata lock'
	, 'Waiting for trigger metadata lock' 
	, 'Waiting on cond'
	, 'Writing to net'
	);

	list($rc, $aProcesslist) = getProcesslist($aParameter, $dbh, $aGlobalVariables);

	// Do some post calculations
	// ...
	foreach ( $aProcesslistToSend as $key ) {
		if ( ! isset($aProcesslist[$key] ) ) {
			$aProcesslist[$key] = 0;
		}
	}

	sendData($aParameter, $aProcesslist, $aProcesslistToSend);
/*
  get all the singele queries like in innodb status\G
SELECT CONCAT_WS(':', user, host, db, time, state, REPLACE(info, "\n", ' '), '  ', ' ')
  FROM information_schema.processlist
 WHERE command != 'Sleep'
   AND id != CONNECTION_ID()
   and user != 'system user'
   and user != 'event_scheduler'
;
*/
	return $rc;
}

// ---------------------------------------------------------------------
function processMysqlInformation($dbh, $pParameter)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '    ' . __FUNCTION__); }

	$hGlobalVariables = array();
	$hGlobalStatus = array();

	$aGlobalVariablesToSend = array(
	  'alive'
	, 'character_set_server'
	, 'datadir'
	, 'default_storage_engine'
	, 'event_scheduler'
	, 'foreign_key_checks'
	, 'general_log'
	, 'log_output'
	, 'log_queries_not_using_indexes'
	, 'log_slave_updates'
	, 'log_warnings'
	, 'long_query_time'
	, 'max_allowed_packet'
	, 'max_connections'
	, 'max_user_connections'
	, 'max_heap_table_size'
	, 'mr_version'
	, 'open_files_limit'
	, 'query_cache_size'
	, 'query_cache_type'
	, 'read_buffer_size'
	, 'skip_networking'
	, 'slow_query_log'
	, 'sort_buffer_size'
	, 'sql_mode'
	, 'table_cache'
	, 'table_definition_cache'
	, 'table_open_cache'
	, 'thread_cache_size'
	, 'thread_stack'
	, 'tx_isolation'
	, 'tmp_table_size'
	, 'version'
	);

	$aGlobalStatusToSend = array(
		'Aborted_clients'
	, 'Access_denied_errors'
	, 'Aborted_connects'
	, 'Binlog_bytes_written'   // MariaDB
	, 'Binlog_cache_disk_use'
	, 'Binlog_cache_use'
	, 'Binlog_stmt_cache_disk_use'
	, 'Binlog_stmt_cache_use'
	, 'Bytes_received'
	, 'Bytes_sent'
	, 'Com_call_proc'
	, 'Com_delete'
	, 'Com_delete_multi'
	, 'Com_insert'
	, 'Com_insert_select'
	, 'Com_replace'
	, 'Com_replace_select'
	, 'Com_select'
	, 'Com_update'
	, 'Com_update_multi'
	, 'Connections'
	, 'Created_tmp_disk_tables'
	, 'Created_tmp_files'
	, 'Created_tmp_tables'
	, 'Handler_delete'
	, 'Handler_read_first'
	, 'Handler_read_key'
	, 'Handler_read_last'
	, 'Handler_read_next'
	, 'Handler_read_prev'
	, 'Handler_read_rnd'
	, 'Handler_read_rnd_next'
	, 'Handler_update'
	, 'Handler_write'
	, 'Max_used_connections'
	, 'Opened_files'
	, 'Opened_tables'
	, 'Opened_table_definitions'
	, 'Open_files'
	, 'Open_tables'
	, 'Open_table_definitions'
	, 'Qcache_block_per_query'
	, 'Qcache_byte_per_block'
	, 'Qcache_free_blocks'
	, 'Qcache_free_memory'
	, 'Qcache_hits'
	, 'Qcache_inserts'
	, 'Qcache_lowmem_prunes'
	, 'Qcache_not_cached'
	, 'Qcache_queries_in_cache'
	, 'Qcache_total_blocks'
	, 'Qcache_used_blocks'
	, 'Qcache_used_memory'
	, 'Queries'
	, 'Questions'
	, 'Select_full_join'
	, 'Select_full_range_join'
	, 'Select_range_check'
	, 'Select_scan'
	, 'Slow_queries'
	, 'Sort_merge_passes'
	, 'Sort_range'
	, 'Sort_rows'
	, 'Sort_scan'
	, 'Table_locks_immediate'
	, 'Table_locks_waited'
	, 'Threads_cached'
	, 'Threads_connected'
	, 'Threads_created'
	, 'Threads_running'
	, 'Uptime'
	);

	initValues($hGlobalVariables, $aGlobalVariablesToSend);
	initValues($hGlobalStatus, $aGlobalStatusToSend);

	// This should be done automatically by initValues
	// $hGlobalVariables['alive'] = 0;
	// $dbh = getDatabaseConnection($pParameter);

	if ( $dbh === false ) {
		$rc = 1400;
		if ( $pParameter['LogLevel'] >= ERR ) { mylog($pParameter['LogFile'], DBG, "    Database connection failed (rc=$rc)."); }
		// sendData with empty values to detect DB down!
		// todo: this is a bad idea
		// todo: this should be done more nicely later!!!
		if ( ($key = array_search('sort_buffer_size', $aGlobalVariablesToSend)) !== false ) {
			unset($aGlobalVariablesToSend[$key]);
		}
		if ( ($key = array_search('tx_isolation', $aGlobalVariablesToSend)) !== false ) {
			unset($aGlobalVariablesToSend[$key]);
		}
		if ( ($key = array_search('thread_stack', $aGlobalVariablesToSend)) !== false ) {
			unset($aGlobalVariablesToSend[$key]);
		}
		if ( ($key = array_search('log_output', $aGlobalVariablesToSend)) !== false ) {
			unset($aGlobalVariablesToSend[$key]);
		}
		if ( ($key = array_search('log_queries_not_using_indexes', $aGlobalVariablesToSend)) !== false ) {
			unset($aGlobalVariablesToSend[$key]);
		}
		if ( ($key = array_search('default_storage_engine', $aGlobalVariablesToSend)) !== false ) {
			unset($aGlobalVariablesToSend[$key]);
		}
		sendData($pParameter, $hGlobalVariables, $aGlobalVariablesToSend);
		sendData($pParameter, $hGlobalStatus, $aGlobalStatusToSend);
		return $rc;
	}

	getGlobalVariables($pParameter, $dbh, $hGlobalVariables);
	getGlobalStatus($pParameter, $dbh, $hGlobalStatus);

	// releaseDatabaseConnection($dbh);

	// Do some post calculations

	$count = count($hGlobalVariables);
	if ( $count > 0 ) {
		$hGlobalVariables['alive'] = 1;

		// Set it to a value otherwise we get an error message!
		if ( $hGlobalVariables['sql_mode'] == '' ) {
			$hGlobalVariables['sql_mode'] = '';
		}
	}
	if ( preg_match('/([0-9]{1,2}).([0-9]{1,2}).([0-9]{1,2})/', $hGlobalVariables['version'], $matches) ) {
		$hGlobalVariables['mr_version'] = sprintf("%02d%02d%02d", $matches[1], $matches[2], $matches[3]);
	}
	else {
		$hGlobalVariables['mr_version'] = 'error';
	}
	$hGlobalStatus['Qcache_used_blocks']     = $hGlobalStatus['Qcache_total_blocks'] - $hGlobalStatus['Qcache_free_blocks'];
	$hGlobalStatus['Qcache_used_memory']     = $hGlobalVariables['query_cache_size'] - $hGlobalStatus['Qcache_free_memory'];
	if ( $hGlobalStatus['Qcache_used_blocks'] == 0 ) {
		$hGlobalStatus['Qcache_byte_per_block']  = 0;
	}
	else {
		$hGlobalStatus['Qcache_byte_per_block']  = intval($hGlobalStatus['Qcache_used_memory'] / $hGlobalStatus['Qcache_used_blocks']);
	}
	if ( $hGlobalStatus['Qcache_queries_in_cache'] == 0 ) {
		$hGlobalStatus['Qcache_block_per_query'] = 0;
	}
	else {
		$hGlobalStatus['Qcache_block_per_query'] = intval($hGlobalStatus['Qcache_used_blocks'] / $hGlobalStatus['Qcache_queries_in_cache']);
	}
	$hGlobalVariables['default_storage_engine'] = $hGlobalVariables['default_storage_engine'] == '0' ? $hGlobalVariables['storage_engine'] : $hGlobalVariables['default_storage_engine'];
	$hGlobalVariables['log_output'] = $hGlobalVariables['log_output'] == '0' ? 'FILE' : $hGlobalVariables['log_output'];
	$hGlobalVariables['datadir'] = rtrim($hGlobalVariables['datadir'], '/');

	sendData($pParameter, $hGlobalVariables, $aGlobalVariablesToSend);
	sendData($pParameter, $hGlobalStatus, $aGlobalStatusToSend);

	// $rc = processProcesslistInformation($dbh, $pParameter, $hGlobalVariables)

	return $rc;
}

?>
