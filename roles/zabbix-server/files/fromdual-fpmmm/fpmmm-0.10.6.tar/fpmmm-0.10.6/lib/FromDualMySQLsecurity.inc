<?php

#
# Copyright (C) 2010-2016 FromDual GmbH
#


/*

	processSecurityInformation($pParameter)

*/

require_once("lib/SecurityInformation.inc");

// ---------------------------------------------------------------------
function processSecurityInformation($dbh, $pParameter)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '    ' . __FUNCTION__); }

	$hGlobalVariables = array();
	$aSecurityInformation = array();

	$aGlobalVariablesToSend = array(
		'local_infile'
	, 'old_passwords'
	, 'skip_external_locking'
);

	$aSecurityInformationToSend = array(
		'empty_user'
	, 'test_database'
	, 'root_from_remote'
	, 'user_with_grant_privilege'
	, 'user_with_super_privilege'
	, 'user_with_create_user_privilege'
	);

	initValues($hGlobalVariables, $aGlobalVariablesToSend);
	initValues($aSecurityInformation, $aSecurityInformationToSend);

	// Set defaults
//   $hGlobalVariables['innodb_io_capacity']           = 200;

// 	$dbh = getDatabaseConnection($pParameter);

	if ( $dbh === false ) {
		$rc = 1500;
		if ( $pParameter['LogLevel'] >= ERR ) { mylog($pParameter['LogFile'], DBG, "    Database connection failed (rc=$rc)."); }
		return $rc;
	}

	getGlobalVariables($pParameter, $dbh, $hGlobalVariables);
	getSecurityInformation($pParameter, $dbh, $aSecurityInformation);

// 	releaseDatabaseConnection($dbh);

	sendData($pParameter, $hGlobalVariables, $aGlobalVariablesToSend);
	sendData($pParameter, $aSecurityInformation, $aSecurityInformationToSend);

	return $rc;
}

?>
