<?php

#
# Copyright (C) 2010-2016 FromDual GmbH
#


// ---------------------------------------------------------------------
function processGaleraInformation($dbh, $pParameter)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '    ' . __FUNCTION__); }

	$hGlobalVariables = array();
	$hGlobalStatus = array();

	$aGlobalVariablesToSend = array(
	  'wsrep_auto_increment_control'
	, 'wsrep_causal_reads'
	, 'wsrep_cluster_address'
	, 'wsrep_cluster_name'
	, 'wsrep_node_name'
	, 'wsrep_on'
	, 'wsrep_provider'
	, 'wsrep_provider_options'
	, 'wsrep_slave_threads'
	, 'wsrep_sst_method'
	, 'wsrep_sst_receive_address'
	);

	$aGlobalStatusToSend = array(
	  'wsrep_apply_oooe'
	, 'wsrep_apply_oool'
	, 'wsrep_apply_window'
	, 'wsrep_cert_deps_distance'
	, 'wsrep_cert_index_size'
	, 'wsrep_cluster_conf_id'
	, 'wsrep_cluster_size'
	, 'wsrep_cluster_status'
	, 'wsrep_commit_oooe'
	, 'wsrep_commit_oool'
	, 'wsrep_commit_window'
	, 'wsrep_connected'
	, 'wsrep_flow_control_paused'
	, 'wsrep_flow_control_recv'
	, 'wsrep_flow_control_sent'
	, 'wsrep_flow_control_waits'
	, 'wsrep_last_committed'
	, 'wsrep_local_bf_aborts'
	, 'wsrep_local_cert_failures'
	, 'wsrep_local_commits'
	, 'wsrep_local_recv_queue'
	, 'wsrep_local_recv_queue_avg'
	, 'wsrep_local_send_queue'
	, 'wsrep_local_send_queue_avg'
	, 'wsrep_local_state'
	, 'wsrep_local_state_comment'
	, 'wsrep_local_status'
	, 'wsrep_protocol_version'
	, 'wsrep_provider_version'
	, 'wsrep_provider_version_mr'
	, 'wsrep_ready'
	, 'wsrep_received'
	, 'wsrep_received_bytes'
	, 'wsrep_replicated'
	, 'wsrep_replicated_bytes'
	);

	initValues($hGlobalVariables, $aGlobalVariablesToSend);
	initValues($hGlobalStatus, $aGlobalStatusToSend);

// 	$dbh = getDatabaseConnection($pParameter);

	if ( $dbh === false ) {
		$rc = 1250;
		if ( $pParameter['LogLevel'] >= ERR ) { mylog($pParameter['LogFile'], DBG, "    Database connection failed (rc=$rc)."); }
		return $rc;
	}

	// This is new behaviour and necessary since Galera 3.9 approx.
	$sql = 'SET SESSION wsrep_causal_reads=0';
	$dbh->query($sql);

	getGlobalVariables($pParameter, $dbh, $hGlobalVariables);
	getGlobalStatus($pParameter, $dbh, $hGlobalStatus);

// 	releaseDatabaseConnection($dbh);

	// Do some post calculations

	if ( preg_match("/([0-9]{1,2}).([0-9]{1,2}).([0-9]{1,2})/", $hGlobalStatus['wsrep_provider_version'], $matches) ) {
		$hGlobalStatus['wsrep_provider_version_mr'] = sprintf("%02d%02d%02d", $matches[1], $matches[2], $matches[3]);
	}
	else {
		$hGlobalStatus['wsrep_provider_version_mr'] = 'error';
	}

	sendData($pParameter, $hGlobalVariables, $aGlobalVariablesToSend);
	sendData($pParameter, $hGlobalStatus, $aGlobalStatusToSend);

	return $rc;
}

?>
