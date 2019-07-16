<?php

#
# Copyright (C) 2010-2016 FromDual GmbH
#


/*

	writeDataToCacheFile($aData, $aDataToSend)

*/

// ---------------------------------------------------------------------
function writeDataToCacheFile($pParameter, $aData, $aDataToSend)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '      ' . __FUNCTION__); }

	// Check if Cache File exists, if not create it
	if ( ! file_exists($pParameter['CacheFile']) ) {

		$ret = clearCacheFile($pParameter, $pParameter['CacheFile']);
		if ( $ret != OK ) {
			$rc = 1850;
			mylog($pParameter['LogFile'], ERR, " Cannot clear file " . $pParameter['CacheFile'] . " (rc=$rc)!");
			return $rc;
		}
	}
	// If ChacheFile exists, check that it is writable
	else {

		if ( ! is_writeable($pParameter['CacheFile']) ) {
			$rc = 1851;
			mylog($pParameter['LogFile'], ERR, " Cannot write to file " . $pParameter['CacheFile'] . " (rc=$rc)!");
			return $rc;
		}
	}

	// We had some cases on systems where a too big cache files let
	// the system collapse or at least it was not sending data.
	// Thus check for size ( > 10M) and shrink file before
	$a = stat($pParameter['CacheFile']);
	$size = $a['size'];
	$max_size = 10 * 1024 * 1024;
	if ( $size > $max_size ) {
		$rc = 1852;
		// truncate did not help. Because we had good experience with rm we
		// use unlink here
		unlink($pParameter['CacheFile']);
		$msg = sprintf("Cache file %s is too big (%d byte). Removing it.", $pParameter['CacheFile'], $size);
		mylog($pParameter['LogFile'], WARN, "    $msg (rc=$rc).");
	}

	$lDomain = $pParameter['Domain'];
	// some stuff in server module requires no domain
	if ( $lDomain != '' ) {
		$lDomain .= '.';
	}
	$lTimestamp = time();

	if ( $pParameter['TimeShift'] != 0.0 ) {
		$lTimestamp = $lTimestamp + intval($pParameter['TimeShift'] * 3600);
	}

	// Write data to cache file first. We assume this is more efficent than
	// calling zabbix sender zillions of times
	if ( ! $fh = fopen($pParameter['CacheFile'], 'a+') ) {
		$rc = 1853;
		$a = error_get_last();
		$msg = sprintf("Cannot open file %s.", $pParameter['CacheFile']);
		mylog($pParameter['LogFile'], ERR, "    $msg (rc=$rc).");
		mylog($pParameter['LogFile'], ERR, "    Reason: " . $a['message']);
		return $rc;
	}

	foreach ( $aData as $key => $value ) {

		// check if key is in ToSend array
		if ( in_array($key, $aDataToSend) ) {
			fprintf($fh, "%s %s %d \"%s\"\n", $pParameter['Hostname'], $lDomain.$key, $lTimestamp, $aData[$key]);
			if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, '        ' . sprintf("%s %s %d \"%s\"", $pParameter['Hostname'], $lDomain.$key, $lTimestamp, $aData[$key])); }
		}
	}   // foreach
	fclose($fh);
	return $rc;
}

?>
