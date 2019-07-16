<?php

#
# Copyright (C) 2010-2016 FromDual GmbH
#


/*

	processFpmmmInformation($pParameter)

*/

// ---------------------------------------------------------------------
function processFpmmmInformation($pParameter)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '    ' . __FUNCTION__); }

	$pGlobalVariables = array();

	$pGlobalVariablesToSend = array(
	  'fpmmm_version'
	, 'fpmmm_mr_version'
	);

	initValues($pGlobalVariables, $pGlobalVariablesToSend);

	// Do some post calculations

	$pGlobalVariables['fpmmm_version'] = RELEASE;

	if ( preg_match('/([0-9]{1,2}).([0-9]{1,2})(.([0-9]{1,2}))?/', $pGlobalVariables['fpmmm_version'], $matches) ) {

		if ( isset($matches[3]) ) {
			$pGlobalVariables['fpmmm_mr_version'] = sprintf("%02d%02d%02d", $matches[1], $matches[2], $matches[4]);
		}
		else {
			$pGlobalVariables['fpmmm_mr_version'] = sprintf("%02d%02d%02d", $matches[1], $matches[2], 0);
		}
	}
	else {
		$pGlobalVariables['fpmmm_mr_version'] = 'error: ' . $pGlobalVariables['fpmmm_version'];
	}

	$ret = sendData($pParameter, $pGlobalVariables, $pGlobalVariablesToSend);
	$rc = $ret;

	return $rc;
}

?>
