<?php

#
# Copyright (C) 2010-2016 FromDual GmbH
#


/*

	processInnodbInformation()

*/

require_once("lib/InnoDbStatus.inc");

// ---------------------------------------------------------------------
function processInnodbInformation($dbh, $pParameter)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '    ' . __FUNCTION__); }

	$hGlobalVariables = array();
	$hGlobalStatus = array();
	$hInnodbStatus = array();

	$aGlobalVariablesToSend = array(
		'ignore_builtin_innodb'
	, 'innodb_buffer_pool_instances'
	, 'innodb_buffer_pool_size'
	, 'innodb_file_format'
	, 'innodb_file_per_table'
	, 'innodb_flush_log_at_trx_commit'
	, 'innodb_flush_method'
	, 'innodb_force_recovery'
	, 'innodb_io_capacity'
	, 'innodb_log_buffer_size'
	, 'innodb_log_file_size'
	, 'innodb_log_files_in_group'
	, 'innodb_page_size'
);

	$aGlobalStatusToSend = array(
		'Innodb_buffer_pool_hit_ratio'
	, 'Innodb_buffer_pool_max_dirty_pages'
	, 'Innodb_buffer_pool_pages_data'
	, 'Innodb_buffer_pool_pages_dirty'
	, 'Innodb_buffer_pool_pages_free'
	, 'Innodb_buffer_pool_pages_misc'
	, 'Innodb_buffer_pool_pages_total'
	, 'Innodb_buffer_pool_max_dirty_pages_b'
	, 'Innodb_buffer_pool_pages_data_b'
	, 'Innodb_buffer_pool_pages_dirty_b'
	, 'Innodb_buffer_pool_pages_free_b'
	, 'Innodb_buffer_pool_pages_misc_b'
	, 'Innodb_buffer_pool_pages_total_b'
	, 'Innodb_buffer_pool_reads'
	, 'Innodb_buffer_pool_read_requests'
	, 'Innodb_data_fsyncs'
	, 'Innodb_data_pending_fsyncs'
	, 'Innodb_data_pending_reads'
	, 'Innodb_data_pending_writes'
	, 'Innodb_data_reads'
	, 'Innodb_data_writes'
	, 'Innodb_data_read'
	, 'Innodb_data_written'
	, 'Innodb_log_waits'
	, 'Innodb_os_log_fsyncs'
	, 'Innodb_os_log_pending_fsyncs'
	, 'Innodb_os_log_pending_writes'
	, 'Innodb_os_log_written'
	, 'Innodb_row_lock_time'
	, 'Innodb_row_lock_waits'
	, 'Innodb_rows_deleted'
	, 'Innodb_rows_inserted'
	, 'Innodb_rows_read'
	, 'Innodb_rows_updated'
	);

	$aInnodbStatusToSend = array(
		'Innodb_compress_time'
	, 'Innodb_hash_node_size'
	, 'Innodb_hash_searches'
	, 'Innodb_last_checkpoint_at'
	, 'Innodb_log_flushed_up_to'
	, 'Innodb_log_sequence_number'
	, 'Innodb_mutex_os_waits'
	, 'Innodb_mutex_spin_rounds'
	, 'Innodb_mutex_spin_waits'
	, 'Innodb_non_hash_searches'
	, 'Innodb_open_read_views_inside_innodb'
	, 'Innodb_pages_flushed_up_to'
	, 'Innodb_queries_in_queue'
	, 'Innodb_queries_inside_innodb'
	, 'Innodb_rows_locked'
	, 'Innodb_rows_modified'
	, 'Innodb_trx_committing'
	, 'Innodb_trx_history_list_length'
	, 'Innodb_trx_lock_memory'
	, 'Innodb_trx_lock_wait'
	, 'Innodb_trx_rolling_back'
	, 'Innodb_trx_running'
	, 'Innodb_uncompress_time'
);

	initValues($hGlobalVariables, $aGlobalVariablesToSend);
	initValues($hGlobalStatus, $aGlobalStatusToSend);
	initValues($hInnodbStatus, $aInnodbStatusToSend);

	// Set defaults
	$hGlobalVariables['innodb_io_capacity']           = 200;
	$hGlobalVariables['innodb_buffer_pool_instances'] = 1;
	$hGlobalVariables['innodb_file_per_table']        = 'Antelope';
	$hGlobalVariables['innodb_page_size']             = 16384;

	// $dbh = getDatabaseConnection($pParameter);

	if ( $dbh === false ) {
		$rc = 1275;
		if ( $pParameter['LogLevel'] >= ERR ) { mylog($pParameter['LogFile'], DBG, "    Database connection failed (rc=$rc)."); }
		return $rc;
	}

	getGlobalVariables($pParameter, $dbh, $hGlobalVariables);
	getGlobalStatus($pParameter, $dbh, $hGlobalStatus);
	$ret = getInnoDbStatus($pParameter, $dbh, $hInnodbStatus);

	// releaseDatabaseConnection($dbh);

	// Do some post calculations
	$hGlobalStatus['Innodb_buffer_pool_pages_data_b'] = $hGlobalVariables['innodb_page_size'] * $hGlobalStatus['Innodb_buffer_pool_pages_data'];
	$hGlobalStatus['Innodb_buffer_pool_pages_dirty_b'] = $hGlobalVariables['innodb_page_size'] * $hGlobalStatus['Innodb_buffer_pool_pages_dirty'];
	$hGlobalStatus['Innodb_buffer_pool_pages_free_b'] = $hGlobalVariables['innodb_page_size'] * $hGlobalStatus['Innodb_buffer_pool_pages_free'];
	// We get from time to time: Type of received value [3.02231454903657e+23] is not suitable...
	$hGlobalStatus['Innodb_buffer_pool_pages_misc_b'] = intval($hGlobalVariables['innodb_page_size'] * $hGlobalStatus['Innodb_buffer_pool_pages_misc']);
	$hGlobalStatus['Innodb_buffer_pool_pages_total_b'] = $hGlobalVariables['innodb_page_size'] * $hGlobalStatus['Innodb_buffer_pool_pages_total'];

	if ( ($hGlobalStatus['Innodb_buffer_pool_read_requests'] + $hGlobalStatus['Innodb_buffer_pool_reads']) == 0 ) {
		$hGlobalStatus['Innodb_buffer_pool_hit_ratio'] = 0.0;
	}
	else {
		// Not good because it is average over all time!
		$hGlobalStatus['Innodb_buffer_pool_hit_ratio'] = round($hGlobalStatus['Innodb_buffer_pool_read_requests'] / ($hGlobalStatus['Innodb_buffer_pool_read_requests'] + $hGlobalStatus['Innodb_buffer_pool_reads']) * 100.0, 2);
	}
	$hGlobalStatus['Innodb_buffer_pool_max_dirty_pages'] = floor($hGlobalStatus['Innodb_buffer_pool_pages_total'] * $hGlobalVariables['innodb_max_dirty_pages_pct'] / 100);
	$hGlobalStatus['Innodb_buffer_pool_max_dirty_pages_b'] = $hGlobalVariables['innodb_page_size'] * $hGlobalStatus['Innodb_buffer_pool_max_dirty_pages'];

	if ( $hGlobalVariables['innodb_flush_method'] == '' ) {
		$hGlobalVariables['innodb_flush_method'] = 'fsync';
	}

	sendData($pParameter, $hGlobalVariables, $aGlobalVariablesToSend);
	sendData($pParameter, $hGlobalStatus, $aGlobalStatusToSend);
	sendData($pParameter, $hInnodbStatus, $aInnodbStatusToSend);

	return $rc;
}

?>
