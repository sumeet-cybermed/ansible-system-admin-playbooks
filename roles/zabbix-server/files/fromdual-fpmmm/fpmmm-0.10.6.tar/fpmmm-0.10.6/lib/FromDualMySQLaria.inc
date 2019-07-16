<?php

#
# Copyright (C) 2010-2016 FromDual GmbH
#


/*

	processAriaInformation($pParameter)

*/

// ---------------------------------------------------------------------
function processAriaInformation($dbh, $pParameter)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '    ' . __FUNCTION__); }

	$hGlobalVariables = array();
	$hGlobalStatus    = array();

	$aGlobalVariablesToSend = array(
		'maria_pagecache_buffer_size'
	, 'maria_block_size'
	, 'maria_pagecache_blocks'
	, 'aria_pagecache_buffer_size'
	, 'aria_block_size'
	, 'aria_pagecache_blocks'
	);

	$aGlobalStatusToSend = array(
	  'Maria_pagecache_blocks_not_flushed'
	, 'Maria_pagecache_blocks_unused'
	, 'Maria_pagecache_blocks_used'
	, 'Maria_pagecache_reads'
	, 'Maria_pagecache_read_requests'
	, 'Maria_pagecache_writes'
	, 'Maria_pagecache_write_requests'
	, 'Maria_transaction_log_syncs'
	, 'Aria_pagecache_blocks_not_flushed'
	, 'Aria_pagecache_blocks_unused'
	, 'Aria_pagecache_blocks_used'
	, 'Aria_pagecache_reads'
	, 'Aria_pagecache_read_requests'
	, 'Aria_pagecache_writes'
	, 'Aria_pagecache_write_requests'
	, 'Aria_transaction_log_syncs'
	);

	initValues($hGlobalVariables, $aGlobalVariablesToSend);
	initValues($hGlobalStatus, $aGlobalStatusToSend);

// 	$dbh = getDatabaseConnection($pParameter);

	if ( $dbh === false ) {
		$rc = 1200;
		if ( $pParameter['LogLevel'] >= ERR ) { mylog($pParameter['LogFile'], DBG, "    Database connection failed (rc=$rc)."); }
		return $rc;
	}

	getGlobalVariables($pParameter, $dbh, $hGlobalVariables);
	getGlobalStatus($pParameter, $dbh, $hGlobalStatus);

// 	releaseDatabaseConnection($dbh);

	// Do some post calculations

	$hGlobalVariables['maria_pagecache_blocks'] = $hGlobalVariables['maria_block_size'] == 0 ? 0 : intval($hGlobalVariables['maria_pagecache_buffer_size'] / $hGlobalVariables['maria_block_size']);
	$hGlobalVariables['aria_pagecache_blocks']  = $hGlobalVariables['aria_block_size'] == 0 ? 0 : intval($hGlobalVariables['aria_pagecache_buffer_size'] / $hGlobalVariables['aria_block_size']);

	sendData($pParameter, $hGlobalVariables, $aGlobalVariablesToSend);
	sendData($pParameter, $hGlobalStatus, $aGlobalStatusToSend);

	return $rc;
}

?>
