<?php

#
# Copyright (C) 2010-2016 FromDual GmbH
#


/*

	uploadData($pParameter)

*/

// ---------------------------------------------------------------------
function uploadData($pParameter)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '      ' . __FUNCTION__); }

	if ( ! is_readable($pParameter['CacheFile']) ) {
		$rc = 1825;
		mylog($pParameter['LogFile'], ERR, "Cannot read file " . $pParameter['CacheFile'] . " (rc=$rc).");
		return $rc;
	}

	// We had some cases on systems where a too big cache files let
	// the system collapse or at least it was not sending data.
	// Thus check for size (> 10M) and shrink file before

	$a = stat($pParameter['CacheFile']);
	$lFilesize = $a['size'];
	$max_size = 10 * 1024 * 1024;
	if ( $lFilesize > $max_size ) {
		$rc = 1826;
		// truncate did not help. Because we had good experience with rm we
		// use unlink here
		unlink($pParameter['CacheFile']);
		$msg = sprintf("Cache file %s is too big (%d byte). Removing it.", $pParameter['CacheFile'], $lFilesize);
		mylog($pParameter['LogFile'], WARN, "    $msg (rc=$rc).");
	}

	// Comment from old Perl times:
	// The following command does sometimes abort
	// thus we do not remove the lock file!!!
	// no clue why! It can happen at least in the modules slave and fpmmm
	// Could be caused by zabbix_server timeout after 10 seconds because
	// it happens only with zabbix_server call but not with crontab!

	// cURL headers for file uploading
	$aHttpHeader  = array('Content-Type: multipart/form-data');

	// For PHP < 5.5
	if ( ! function_exists('curl_file_create') ) {

		function curl_file_create($filename, $mimetype = '', $postname = '') {
			return "@$filename;filename="
					. ($postname ?: basename($filename))
					. ($mimetype ? ";type=$mimetype" : '');
		}
	}

	$cfile = curl_file_create($pParameter['CacheFile'], 'text/plain', 'testdata');
	$data = array('filename' => $cfile, 'hash' => $pParameter['Hash']);
	$aCurlOptions = array(
	  CURLOPT_URL            => $pParameter['Url']
	, CURLOPT_HEADER         => true
	, CURLOPT_POST           => 1
	, CURLOPT_CUSTOMREQUEST  => 'POST'
	, CURLOPT_RETURNTRANSFER => true
	, CURLOPT_HTTPHEADER     => $aHttpHeader
	, CURLOPT_POSTFIELDS     => $data
	, CURLOPT_INFILESIZE     => $lFilesize
	);

	if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, "    " . print_r($aCurlOptions, true)); }

	$ch = curl_init();
	curl_setopt_array($ch, $aCurlOptions);
	$result = curl_exec($ch);
	if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, "      CURL exec result: $result."); }
	if( curl_errno($ch) == OK ) {

		$info = curl_getinfo($ch);
		if ($info['http_code'] == 200) {
			if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], INFO, "      File uploaded successfully."); }
		}
		if ( preg_match("/error/i", $result) ) {
			if ( $pParameter['LogLevel'] >= ERR ) { mylog($pParameter['LogFile'], ERR, "      CURL exec result: $result."); }
		}
	}
	else {
		$rc = 1827;
		$errmsg = curl_error($ch);
		mylog($pParameter['LogFile'], ERR, "      error: $errmsg (rc=$rc)");
		return $rc;
	}
	curl_close($ch);

	/*
	Old Perl stuff
	$response = result?

	if ( ! $response->is_success ) {
		$rc = 1828;
		mylog($pParameter['LogFile'], ERR, '      ' . $pParameter['Url'] . " error: " . $response->status_line . " (rc=$rc)");
		return $rc;
	}

	if ( $response->content_type != 'text/html' ) {
		$rc = 1829;
		mylog($pParameter['LogFile'], ERR, "      Weird content type at " . $pParameter['Url'] . ": " . $response->content_type . " (rc=$rc)");
		return $rc;
	}

	// Data loaded
	if ( preg_match('/Data loaded successful/', $response->content) ) {
		if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, "        Data loaded."); }
	}
	// Data load failed
	else {
		$rc = 1830;
		mylog($pParameter['LogFile'], ERR, "      Data upload failed. (rc=$rc)");
		mylog($pParameter['LogFile'], ERR, '      ' . $response->content);
		// Do clean-up already here because of problem from 2012-03-14
		clearCacheFile($pParameter, $pParameter['CacheFile']);
		return $rc;
	}
	*/

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, "      Data uploaded successfully."); }
	clearCacheFile($pParameter, $pParameter['CacheFile']);

	return $rc;
}

?>
