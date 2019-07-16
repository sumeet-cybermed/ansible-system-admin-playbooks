<?php

#
# Copyright (C) 2010-2016 FromDual GmbH
#


/*

	printHiResTime()
	mylog2($pSeverity, $pMessage)
	mylog($logfile, $lvl, $str)

*/

// ---------------------------------------------------------------------
function printHiResTime()
// ---------------------------------------------------------------------
{
	list($usec, $sec) = explode(' ', microtime());
	return (float)$usec + (float)$sec;
}

// ---------------------------------------------------------------------
function mylog2($pSeverity, $pMessage)
// ---------------------------------------------------------------------
{
	global $gLogFile, $gLogLevel;

	if ( $pSeverity <= $gLogLevel ) {
		$date = date('Y-m-d H:i:s');
		error_log($date . substr(sprintf("%.3f", printHiResTime()), 10) . " - $pMessage\n", 3, $gLogFile);
	}
}

// ----------------------------------------------------------------------
function mylog($logfile, $lvl, $str)
// ----------------------------------------------------------------------
{
	$rc = OK;
	global $gParameter;

	if ( (! isset($logfile)) || ($logfile == '') ) {
		$rc = 1675;

		$a = gettimeofday();
		$seconds = $a['sec'];
		$microseconds = $a['usec'];

		list($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst) = localtime($seconds);
		printf("%5d:%4d-%02d-%02d %02d:%02d:%02d.%03d - %s: %s\n", getmypid(), $year+1900,$mon+1,$mday,$hour,$min,$sec, $microseconds / 1000, 'ERR', "Logfile is not defined or not specified (rc=$rc).");
		cleanAndQuit($gParameter, 1, $rc);
	}

	// Logfile does not exist try to create it
	// This has to be done before the -w check because otherwiese -w would fail the
	// time!
	if ( ! file_exists($logfile) ) {
		$rc = 1676;
		if ( ! $LOG = fopen($logfile, 'a+') ) {
			$a = error_get_last();
			print "Cannot open file $logfile (rc=$rc).";
			print "Reason: " . $a['message'] . "\n";
			cleanAndQuit($gParameter, 0, $rc);
		}
		fclose($LOG);
	}

	if ( ! is_writeable($logfile) ) {
		$rc = 1677;
		print "Cannot write to logfile $logfile. Please check permissions (rc=$rc).\n";
		cleanAndQuit($gParameter, 0, $rc);
	}

	$severity = '';
	if ( $lvl == INFO ) {
		$severity = 'INFO';
	}
	elseif ( $lvl == WARN ) {
		$severity = 'WARN';
	}
	elseif ( $lvl == ERR ) {
		$severity = 'ERR ';
	}
	elseif ( $lvl == DBG ) {
		$severity = 'DBG ';
	}
	else {
		$severity = 'UNKN';
	}

	$PID = getmypid();

	if ( ! $LOG = fopen($logfile, 'a+') ) {
		$rc = 1678;
		$a = error_get_last();
		print "ERROR: Cannot open file $logfile";
		print "       Reason: " . $a['message'] . "\n";
		print "ERROR: FromDual Performance Monitor for MySQL and MariaDB (fpmmm) run finshed (rc=$rc).";
		cleanAndQuit($gParameter, 0, $rc);
	}

	$a = gettimeofday();
	$seconds = $a['sec'];
	$microseconds= $a['usec'];
	list($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst) = localtime($seconds);

	fprintf($LOG, "%5d:%4d-%02d-%02d %02d:%02d:%02d.%03d - %s: %s\n", $PID, $year+1900,$mon+1,$mday,$hour,$min,$sec, $microseconds / 1000, $severity, $str);
	fclose($LOG);

	// Do a log rotate if necessary if logfile is > 10M
	$filesize = filesize($logfile);
	if ( $filesize > 10*1024*1024 ) {
		$new = $logfile . '.rotated';
		rename($logfile, $new);
	}
}

?>
