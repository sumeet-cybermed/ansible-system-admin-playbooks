<?php

#
# Copyright (C) 2010-2016 FromDual GmbH
#


/*

  sendCachedData($pParameter)

*/

// ---------------------------------------------------------------------
function sendCachedData($pParameter)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '    ' . __FUNCTION__); }
	if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, "      Sending cache file to zabbix server."); }

	// File exists and is NOT empty
	if ( file_exists($pParameter['CacheFile']) ) {

		if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, "      Cache file found."); }

		// This should work on CentOS/Ubuntu but also on SuSE
		$prg = 'zabbix_sender zabbix-sender';
		// Caution: Returns 1 because one of both does NOT exist...
		list($ret, $exe, $output, $stderr) = my_exec("which $prg");

		if ( ! isset($exe) ) {
			$rc = 1775;
			mylog($pParameter['LogFile'], ERR, "Cannot find program $exe.");
		}

		// We had some cases on systems where a too big cache files let
		// the system collapse or at least it was not sending data.
		// Thus check for size (> 10M) and shrink file before

		$a = stat($pParameter['CacheFile']);
		$lFilesize = $a['size'];
		$max_size = 10 * 1024 * 1024;
		if ( $lFilesize > $max_size ) {
			$rc = 1777;
			// truncate did not help. Because we had good experience with rm we
			// use unlink here
			unlink($pParameter['CacheFile']);
			$msg = sprintf("Cache file %s is too big (%d byte). Removing it.", $pParameter['CacheFile'], $lFilesize);
			mylog($pParameter['LogFile'], WARN, "    $msg (rc=$rc).");
		}

		$cmd = "$exe --zabbix-server " . $pParameter['ZabbixServer'] . " --port " . $pParameter['ZabbixServerPort'] . " --input-file " . $pParameter['CacheFile'] . " --with-timestamps";
		if ( $pParameter['LogLevel'] >= INFO ) {
			$cmd .= ' -vv';
			mylog($pParameter['LogFile'], INFO, "      $cmd");
		}

		list($ret, $output, $stdout, $stderr) = my_exec($cmd);

		if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, "      (ret=$ret / rc=$rc)."); }
		if ( $pParameter['LogLevel'] >= INFO ) {
			foreach ( $stdout as $line ) {
				mylog($pParameter['LogFile'], INFO, '      ' . $line);
			}
		}
		if ( ($ret != 0) && ($ret != 2) ) {
			foreach ( $stderr as $line ) {
				mylog($pParameter['LogFile'], ERR, '      ' . $line);
			}
		}

		if ( (($ret >> 8) == 0)
			|| (($ret >> 8) == 2) ) {
			// Do not delete Cache File but just set it back to zero again
			clearCacheFile($pParameter, $pParameter['CacheFile']);
		}
		else {
			$rc = 1776;
			mylog($pParameter['LogFile'], ERR, "      Load of cache file " . $pParameter['CacheFile'] . " failed (rc=$rc / ret=$ret).");
			// This is a safety measure in case the cache file is corrupted!
			clearCacheFile($pParameter, $pParameter['CacheFile']);
		}
	}
	else {
		if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, "      No cache file " . $pParameter['CacheFile'] . " found."); }
	}

	return $rc;
}

?>
