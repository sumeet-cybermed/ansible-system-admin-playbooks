#!/usr/bin/php
<?php

#
# Copyright (C) 2010-2016 FromDual GmbH
#


$lMyName = basename(__FILE__, '.php');

// ---- Functions ----

// ---------------------------------------------------------------------
function Usage()
// ---------------------------------------------------------------------
{
	print("
SYNOPSIS

  $MyName flags

DESCRIPTION

  Sends MySQL Performance Monitor Agent cache file to remote Server via https

FLAGS

  file, f  Data file to send.
  hash     Hash value to authenticate.
  help, h  Print this help.
  url, u   URL where to upload.

PARAMETERS

  none

EXAMPLES

  $MyName --url=https://localhost/maas/receiver.php --hash=5499c0397021d002e0e3150944e53459 \\
          --file=/home/oli/fromdual_devel/support/test.cache

");
}

// ---------------------------------------------------------------------
// Start
// ---------------------------------------------------------------------

$rc = OK;

// Option defaults

$aOptions['url']  = 'https://support.fromdual.com/maas/receiver.php';
$aOptions['hash'] = '';
$aOptions['file'] = '';

$shortopts  = 'hf:u:?';

$longopts  = array(
  'hash:'
, 'file:'
, 'help'
, 'url:'
);

if ( $argc == 0 ) {
  Usage();
  exit($rc);
}

$aOptions = getopt($shortopts, $longopts);

if ( isset($aOptions['help']) ) {
  $rc = OK;
  Usage();
  exit($rc);
}

if( $argc != 0 ) {
  $rc = 1906;
  Usage();
  exit($rc);
}

if ( ($aOptions['hash'] == '') || (strlen($aOptions['hash']) != 32) ) {
  $rc = 1908;
  print "ERROR: Hash value (" . $aOptions['hash'] . ") is empty or wrong (rc=$rc).\n";
  exit($rc);
}

if ( ($aOptions['file'] == '') || (! is_readable($aOptions['file'])) ) {
  $rc = 1907;
  print "ERROR: File (" . $aOptions['file'] . ") does NOT exist or is not readable (rc=$rc).\n";
  exit($rc);
}

// ---------------------------------------------------------------------
// MAIN
// ---------------------------------------------------------------------

if ( ! is_readable($aOptions['file']) ) {
  $rc = 1904;
  print "ERROR: Cannot read file " . $aOptions['file'] . " (rc=$rc).\n";
  exit($rc);
}

// todo: Replace my CURLFile later...

// cURL headers for file uploading
$aHttpHeader  = array('Content-Type: multipart/form-data');
$aPostFields  = array('hash' => $pParameter['Hash'], 'data' => $pParameter['CacheFile']);
$aCurlOptions = array(
	CURLOPT_URL            => $pParameter['Url']
, CURLOPT_HEADER         => true
, CURLOPT_POST           => 1
, CURLOPT_CUSTOMREQUEST  => 'POST'
, CURLOPT_RETURNTRANSFER => true
, CURLOPT_HTTPHEADER     => $aHttpHeader,
, CURLOPT_POSTFIELDS     => $aPostFields
, CURLOPT_INFILESIZE     => $lFilesize,
);

$ch = curl_init();
curl_setopt_array($ch, $aCurlOptions);
$result = curl_exec($ch);
mylog($pParameter['LogFile'], DBG, "      CURL exec result: $result.");
if( curl_errno($ch) == OK ) {

	$info = curl_getinfo($ch);
	if ($info['http_code'] == 200) {
		mylog($pParameter['LogFile'], ERR, "      File uploaded successfully.");
	}
}
else {
	$rc = 1901;
	$errmsg = curl_error($ch);
	mylog($pParameter['LogFile'], ERR, "      error: $errmsg (rc=$rc)");
	exit($rc);
}
curl_close($ch);

/*

Old Perl Stuff

$response = $browser->request(POST $aOptions['url'],
  Content_Type => 'form-data',
  Content      => [ hash  => $aOptions['hash'],
                    data  => [$aOptions['file']],
                  ]); // ->as_string()

if ( ! $response->is_success ) {
  $rc = 1905;
  print "ERROR: ".$aOptions['url']." error: " . $response->status_line . " (rc=$rc)\n";
  exit($rc);
}
if ( $response->content_type != 'text/html' ) {
  $rc = 1902;
  print "ERROR: Weird content type at ".$aOptions['url'].": " . $response->content_type . " (rc=$rc)\n";
  exit($rc);
}

// Data loaded
if ( $response->content =~ /Data loaded successfully/ ) {
  // print "Data loaded.\n";
}
// Data load failed
else {
  $rc = 1903;
  print "ERROR: Data load failed. (rc=$rc)\n";
  print $response->content . "\n";
  exit($rc);
}

*/

print "INFO: Data loaded successfully (rc=$rc).\n";
exit($rc);

?>
