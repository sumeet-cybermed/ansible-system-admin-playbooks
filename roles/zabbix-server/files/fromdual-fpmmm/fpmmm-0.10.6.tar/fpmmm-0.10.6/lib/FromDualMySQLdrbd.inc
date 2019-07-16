<?php

#
# Copyright (C) 2010-2016 FromDual GmbH
#


/*

	getDrbdOverview($pParameter, &$drbdDevices, &$drbdData)
	getDrbdDetail($pParameter, &$drbdData)
	processDrbdInformation($pParameter)

*/

// ---------------------------------------------------------------------
function getDrbdOverview($pParameter, &$drbdDevices, &$drbdData)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '    ' . __FUNCTION__); }

	// 0:drbd_r1  Connected Secondary/Secondary UpToDate/UpToDate C r----
	// if not as root
	// 0:??not-found??  Connected Primary/Secondary UpToDate/UpToDate C r---- /mnt ext3 2.0G 350M 1.6G 19$ 

	list($ret, $cmd, $output, $stderr) = my_exec('which drbd-overview');

	if ( $ret != OK ) {
		$rc = 1225;
		mylog($pParameter['LogFile'], ERR, '      ' . "drbd-overview does not exist (rc=$rc).");
		return $rc;
	}
	trim($cmd[0]);

	list($ret, $stdout, $output, $stderr) = my_exec($cmd[0]);

	foreach ( $stdout as $line ) {

		trim($line);
		if ( $pParameter['LogLevel'] >= DBG ) {
			mylog($pParameter['LogFile'], DBG, '      ' . $line);
		}

		if ( preg_match("/^\s+(\d+):(\S+)\s+(\w+)\s+(\w+\/\w+)\s+(\w+\/\w+)\s+(\w)/", $line, $matches)
			|| preg_match("/^\s+(\d+):(\?\?not-found\?\?)\s+(\w+)\s+(\w+\/\w+)\s+(\w+\/\w+)\s+(\w)/", $line, $matches)
			) {

			array_push($drbdDevices, $matches[1]);

			$drbdData[$matches[1]]['name'] = $matches[2];
			$drbdData[$matches[1]]['cstate'] = $matches[3];
			$drbdData[$matches[1]]['role'] = $matches[4];
			$drbdData[$matches[1]]['dstate'] = $matches[5];
			$drbdData[$matches[1]]['protocol'] = $matches[6];
			if ( $pParameter['LogLevel'] >= DBG ) {
				mylog($pParameter['LogFile'], DBG, '      ' . 'name   ' . $drbdData[$matches[1]]['name']);
				mylog($pParameter['LogFile'], DBG, '      ' . 'cstate ' . $drbdData[$matches[1]]['cstate']);
				mylog($pParameter['LogFile'], DBG, '      ' . 'role   ' . $drbdData[$matches[1]]['role']);
				mylog($pParameter['LogFile'], DBG, '      ' . 'dstate ' . $drbdData[$matches[1]]['dstate']);
				mylog($pParameter['LogFile'], DBG, '      ' . 'protocol ' . $drbdData[$matches[1]]['protocol']);
			}
		}
	}
	return $rc;
}

// ---------------------------------------------------------------------
function getDrbdDetail($pParameter, &$drbdData)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '    ' . __FUNCTION__); }

	// version: 8.3.8 (api:88/proto:86-94)GIT-hash: d78846e52224fd00562f7c225bcc25b2d422321d build by mockbuild$builder10.centos.org, 2010-06-04 08:04:09
	//  0: cs:Connected ro:Secondary/Secondary ds:UpToDate/UpToDate C r----
	//     ns:0 nr:0 dw:0 dr:0 al:0 bm:0 lo:0 pe:0 ua:0 ap:0 ep:1 wo:b oos:0

	$file = '/proc/drbd';
	if ( ! is_readable($file) ) {
		$rc = 1226;
		mylog($pParameter['LogFile'], ERR, '      ' . "Cannot read on $file (rc=$rc).");
		return $rc;
	}

	list($ret, $stdout, $output, $stderr) = my_exec("cat $file");

	$line = trim($stdout[0]);
	$drbdData['version'] = $line;

	for ( $i = 1; $i < count($stdout); $i += 2 ) {

		// We are not intersted in 1st line because we got it already
		// with the drbd-overview command

		$line = $stdout[$i+1];
		trim($line);
		if ( preg_match("/^\s+(\d+):/", $line, $matches) ) {
			$device = $matches[1];
		}
		else {
			mylog($pParameter['LogFile'], ERR, '      ' . $line);
		}

		// Parse the 2nd line now
		$line = trim($stdout[$i+2]);

		if ( preg_match("/^\s+ns:(\d+)\s+nr:(\d+)\s+dw:(\d+)\s+dr:(\d+)\s+al:(\d+)\s+bm:(\d+)\s+lo:(\d+)\s+pe:(\d+)\s+ua:(\d+)\s+ap:(\d+)\s+ep:(\d+)\s+wo:(\w+)\s+oos:(\d+)/", $line, $matches) ) {

			$drbdData[$device]['ns']  =  $matches[1];
			$drbdData[$device]['nr']  =  $matches[2];
			$drbdData[$device]['dw']  =  $matches[3];
			$drbdData[$device]['dr']  =  $matches[4];
			$drbdData[$device]['al']  =  $matches[5];
			$drbdData[$device]['bm']  =  $matches[6];
			$drbdData[$device]['lo']  =  $matches[7];
			$drbdData[$device]['pe']  =  $matches[8];
			$drbdData[$device]['ua']  =  $matches[9];
			$drbdData[$device]['ap']  = $matches[10];
			$drbdData[$device]['ep']  = $matches[11];
			$drbdData[$device]['wo']  = $matches[12];
			$drbdData[$device]['oos'] = $matches[13];

			if ( $pParameter['LogLevel'] >= DBG ) {
				mylog($pParameter['LogFile'], DBG, '      ' . 'ns  ' . $drbdData[$device]['ns']);
				mylog($pParameter['LogFile'], DBG, '      ' . 'nr  ' . $drbdData[$device]['nr']);
				mylog($pParameter['LogFile'], DBG, '      ' . 'dw  ' . $drbdData[$device]['dw']);
				mylog($pParameter['LogFile'], DBG, '      ' . 'dr  ' . $drbdData[$device]['dr']);
				mylog($pParameter['LogFile'], DBG, '      ' . 'al  ' . $drbdData[$device]['al']);
				mylog($pParameter['LogFile'], DBG, '      ' . 'bm  ' . $drbdData[$device]['bm']);
				mylog($pParameter['LogFile'], DBG, '      ' . 'lo  ' . $drbdData[$device]['lo']);
				mylog($pParameter['LogFile'], DBG, '      ' . 'pe  ' . $drbdData[$device]['pe']);
				mylog($pParameter['LogFile'], DBG, '      ' . 'ua  ' . $drbdData[$device]['ua']);
				mylog($pParameter['LogFile'], DBG, '      ' . 'ap  ' . $drbdData[$device]['ap']);
				mylog($pParameter['LogFile'], DBG, '      ' . 'ep  ' . $drbdData[$device]['ep']);
				mylog($pParameter['LogFile'], DBG, '      ' . 'wo  ' . $drbdData[$device]['wo']);
				mylog($pParameter['LogFile'], DBG, '      ' . 'oos ' . $drbdData[$device]['oos']);
			}
		}
	}
	return $rc;
}

// ---------------------------------------------------------------------
function processDrbdInformation($pParameter)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '    ' . __FUNCTION__); }

	$ret = 0;
	$ret = getDrbdOverview($pParameter, $drbdDevices, $drbdData);
	if ( $ret != 0 ) {
		$rc = 1227;
		mylog($pParameter['LogFile'], ERR, '      ' . "Error in getDrbdOverview (ret=$ret, rc=$rc).");
		return $rc;
	}
	$ret = getDrbdDetail($pParameter, $drbdData);
	if ( $ret != 0 ) {
		$rc = 1228;
		mylog($pParameter['LogFile'], ERR, '      ' . "Error in getDrbdDetail (rc=$rc).");
		return $rc;
	}

	$aDrbdStatusToSend = (
	  'version'
	);

	$hDrbdStatus['version'] = $drbdData['version'] ;

	foreach ( $drbdDevices as $device ) {

		array_push($aDrbdStatusToSend, 'name[' . $device . ']');
		$hDrbdStatus['name[' . $device . ']'] = $drbdData[$device]['name'];

		array_push($aDrbdStatusToSend, 'cstate[' . $device . ']');
		$hDrbdStatus['cstate[' . $device . ']'] = $drbdData[$device]['cstate'];

		array_push($aDrbdStatusToSend, 'role[' . $device . ']');
		$hDrbdStatus['role[' . $device . ']'] = $drbdData[$device]['role'];

		array_push($aDrbdStatusToSend, 'dstate[' . $device . ']');
		$hDrbdStatus['dstate[' . $device . ']'] = $drbdData[$device]['dstate'];

		array_push($aDrbdStatusToSend, 'protocol[' . $device . ']');
		$hDrbdStatus['protocol[' . $device . ']'] = $drbdData[$device]['protocol'];

		array_push($aDrbdStatusToSend, 'ns[' . $device . ']');
		$hDrbdStatus['ns[' . $device . ']'] = $drbdData[$device]['ns'];

		array_push($aDrbdStatusToSend, 'nr[' . $device . ']');
		$hDrbdStatus['nr[' . $device . ']'] = $drbdData[$device]['nr'];

		array_push($aDrbdStatusToSend, 'dw[' . $device . ']');
		$hDrbdStatus['dw[' . $device . ']'] = $drbdData[$device]['dw'];

		array_push($aDrbdStatusToSend, 'dr[' . $device . ']');
		$hDrbdStatus['dr[' . $device . ']'] = $drbdData[$device]['dr'];

		array_push($aDrbdStatusToSend, 'al[' . $device . ']');
		$hDrbdStatus['al[' . $device . ']'] = $drbdData[$device]['al'];

		array_push($aDrbdStatusToSend, 'bm[' . $device . ']');
		$hDrbdStatus['bm[' . $device . ']'] = $drbdData[$device]['bm'];

		array_push($aDrbdStatusToSend, 'lo[' . $device . ']');
		$hDrbdStatus['lo[' . $device . ']'] = $drbdData[$device]['lo'];

		array_push($aDrbdStatusToSend, 'pe[' . $device . ']');
		$hDrbdStatus['pe[' . $device . ']'] = $drbdData[$device]['pe'];

		array_push($aDrbdStatusToSend, 'ua[' . $device . ']');
		$hDrbdStatus['ua[' . $device . ']'] = $drbdData[$device]['ua'];

		array_push($aDrbdStatusToSend, 'ap[' . $device . ']');
		$hDrbdStatus['ap[' . $device . ']'] = $drbdData[$device]['ap'];

		array_push($aDrbdStatusToSend, 'ep[' . $device . ']');
		$hDrbdStatus['ep[' . $device . ']'] = $drbdData[$device]['ep'];

		array_push($aDrbdStatusToSend, 'wo[' . $device . ']');
		$hDrbdStatus['wo[' . $device . ']'] = $drbdData[$device]['wo'];

		array_push($aDrbdStatusToSend, 'oos[' . $device . ']');
		$hDrbdStatus['oos[' . $device . ']'] = $drbdData[$device]['oos'];
	}

	sendData($pParameter, $hDrbdStatus, $aDrbdStatusToSend);
	return $rc;
}

?>
