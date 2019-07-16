<?php

#
# Copyright (C) 2010-2016 FromDual GmbH
#


/*

	sendData($pParameter, $aData, $aDataToSend)

*/

require_once("lib/sendCachedData.inc");

// ---------------------------------------------------------------------
function sendData($pParameter, $aData, $aDataToSend)
// ---------------------------------------------------------------------
{
	$rc = OK;

// print_r($pParameter);
// print_r($aData);
// print_r($aDataToSend);


	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '    ' . __FUNCTION__); }

	writeDataToCacheFile($pParameter, $aData, $aDataToSend);

	// fpmmm can be run in 2 modes now: locally or MaaS
	// We run fpmmm locally
	if ( $pParameter['MaaS'] != 'on' ) {

		$rc = checkConnectionToZabbixServer($pParameter);

		// Connection to zabbix server seems OK
		if ( $rc == OK ) {

			if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, "      Connection to zabbix server seems OK."); }
			$rc = sendCachedData($pParameter);
		}
		// No success connecting to zabbix server
		else {
			$rc = 1800;
			mylog($pParameter['LogFile'], WARN, '      ZabbixServer=' . $pParameter['ZabbixServer']. ', ZabbixServerPort=' . $pParameter['ZabbixServerPort'] . ', Hostname=' . $pParameter['Hostname']);
			mylog($pParameter['LogFile'], WARN, "      Connection to zabbix server failed (rc=$rc)!");
		}
	}
	// We run fpmmm as MaaS
	else {

		if ( ($pParameter['Methode'] == 'http')
			|| ($pParameter['Methode'] == 'https')
			) {
				$rc = uploadData($pParameter);
		}
		else {
			$rc = 1801;
			mylog($pParameter['LogFile'], ERR, '      Could NOT upload data to MaaS Server. Unknown methode: ' . $pParameter['Methode'] . " (rc=$rc).");
		}
	}

	return $rc;
}

?>
