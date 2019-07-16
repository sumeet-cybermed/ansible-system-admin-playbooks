<?php

#
# Copyright (C) 2010-2016 FromDual GmbH
#


/*

	handleDeadlockMessage($pParameter, $lDeadlockMessage)
	handleForeignKeyMessage($pParameter, $lForeignKeyMessage)
	getInnoDbStatus($pParameter, $dbh, &$aEngineInnodbStatus)
	parseInnoDbStatus($pParameter, $dbh, $lInnodbStatus, &$aEngineInnodbStatus)

*/

// ---------------------------------------------------------------------
function handleDeadlockMessage($pParameter, $lDeadlockMessage)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '      ' . __FUNCTION__); }

	$lTimestamp = time();   // Seems to be not needed at all???

	// Cleanup leading and trailing ----
	$lDeadlockMessage = preg_replace('/-+\n/', '', $lDeadlockMessage);

	$lSendFile = 0;
	// Does an old Deadlock message exist?
	$lDeadlockFile = dirname($pParameter['CacheFileBase']) . '/fpmmm_DeadLockMessage.' . $pParameter['Hostname'];

	if ( is_file($lDeadlockFile) ) {

		if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, "      Old Deadlock message file $lDeadlockFile exists."); }

		// Write temporary Deadlock file
		$lTmpDeadlockFile = dirname($pParameter['CacheFileBase']) . '/fpmmm_DeadlockMessage.tmp.' . getmypid();
		if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, "      Write temporary Deadlock file to $lTmpDeadlockFile."); }
		if ( ($fh = fopen($lTmpDeadlockFile, 'w')) === false ) {
			$rc = 1655;
			$a = error_get_last();
			mylog($pParameter['LogFile'], ERR, "    Cannot open file $lTmpDeadlockFile (rc=$rc).");
			mylog($pParameter['LogFile'], ERR, "    Reason: " . $a['message']);
			return $rc;
		}
		fprintf($fh, "%s", $lDeadlockMessage);
		fclose($fh);
		$hash1 = sha1_file($lTmpDeadlockFile);
		if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, "      Hash of Deadlock file is $hash1."); }
		$hash2 = sha1_file($lDeadlockFile);
		if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, "      Hash of old Deadlock file is $hash2."); }

		// Both Deadlocks are qual
		if ( $hash1 == $hash2 ) {
			if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, "      Hashes are equal. Clean up new Deadlock."); }
			unlink($lTmpDeadlockFile);
		}
		// the 2 Deadlocks differ
		else {
			if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, "      Hashes are different. Propagate new Deadlock."); }
			if ( ! rename($lTmpDeadlockFile, $lDeadlockFile) ) {
				$rc = 1660;
				$a = error_get_last();
				mylog($pParameter['LogFile'], ERR, "      Cannot move file $lTmpDeadlockFile to $lDeadlockFile. Please check permissions of both of them (rc=$rc).");
				mylog($pParameter['LogFile'], ERR, "      Error message: " . $a['message']);
				return $rc;
			}
			else {
				$lSendFile = 1;
			}
		}

	}
	// no, old Deadlock message does NOT exists: then write it
	else {

		if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, "      Old Deadlock message files does NOT exist."); }
		if ( ! $fh = fopen($lDeadlockFile, 'w') ) {
			$rc = 1652;
			$a = error_get_last();
			mylog($pParameter['LogFile'], ERR, "    Cannot open file $lDeadlockFile (rc=$rc).");
			mylog($pParameter['LogFile'], ERR, "    Reason: " . $a['message']);
			return $rc;
		}
		fprintf($fh, "%s", $lDeadlockMessage);
		fclose($fh);
		$lSendFile = 1;
	}   // old Deadlock message file exists?

	// --------

	$hGlobalStatus = array();

	$aGlobalStatusToSend = array(
		'Deadlock_new'
	// , 'Deadlock_message'   # only push message if there is one!
	);

	// Only push message if there is one!
	if ( $lSendFile == 1 ) {
		array_push($aGlobalStatusToSend, 'Deadlock_message');
	}

	initValues($hGlobalStatus, $aGlobalStatusToSend);
	$hGlobalStatus['Deadlock_message'] = 'none';

	// fpmmm can be run in 2 modes now: locally or MaaS
	// We run fpmmm locally
	if ( $pParameter['MaaS'] != 'on' ) {

		// Deadlock does not exist yet or is a new Deadlock, so send it
		if ( $lSendFile == 1 ) {

			if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, "      New Deadlock found."); }

			$hGlobalStatus['Deadlock_new'] = 1;
			$rc = checkConnectionToZabbixServer($pParameter);

			// Connection to zabbix server seems OK
			if ( $rc == OK ) {

				if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, "Connection to zabbix server seems OK."); }

				$key = 'Deadlock_message';

				// This should work on CentOS/Ubuntu but also on SuSE
				$prg = 'zabbix_sender zabbix-sender';
				// Caution: Returns 1 because one of both does NOT exist...
				list($ret, $exe, $output, $stderr) = my_exec("which $prg");

				if ( ! isset($exe) ) {
					$rc = 1650;
					mylog($pParameter['LogFile'], ERR, "Cannot find program $exe.");
				}

				// ;; and some other causes troubles in my_exec thus we replace them...
				$dlm = $lDeadlockMessage;
				$dlm = preg_replace('/"/', '', $dlm);
				$dlm = preg_replace("/'/", '', $dlm);
				// $dlm = escapeshellcmd($dlm);

				// This is an exception because with cache file we have not found a way
				// how to send the data with CR/LF
				$cmd = "$exe --zabbix-server " . $pParameter['ZabbixServer'] . " --port " . $pParameter['ZabbixServerPort'] . " --host '" . $pParameter['Hostname']. "' --key " . $pParameter['Domain'] . '.' . $key . " --value '" . $dlm . "'";

				if ( $pParameter['LogLevel'] >= DBG ) {
					$cmd .= ' -vv';
					mylog($pParameter['LogFile'], DBG, $cmd);
				}

				list($ret, $output, $stdout, $stderr) = my_exec($cmd);

				$rc = 1658;
				if ( $pParameter['LogLevel'] >= DBG ) {
					mylog($pParameter['LogFile'], DBG, "      (ret=$ret / rc=$rc).");
					foreach ( $stdout as $line ) {
						mylog($pParameter['LogFile'], DBG, '      ' . $line);
					}
				}
				if ( ($ret != 0) && ($ret != 2) ) {
					foreach ( $stderr as $line ) {
						mylog($pParameter['LogFile'], ERR, '      ' . $line);
					}
				}

				// If zabbix_sender fails we should roll back the stuff to get all Deadlocks!
			}
			// No success connecting to zabbix server write to Cache File
			else {
				$rc = 1659;
				mylog($pParameter['LogFile'], WARN, "Connection to zabbix server failed (rc=$rc).");

				// Transform \n into marker 'xxx' because it will cause an error otherwise!
				// We could think about if it is sufficient to send just a Deadlock without the timestamp
				// this would cause only the first time a wrong information
				// not needed when we run direct and not as MaaS
				// $lDeadlockMessage = preg_replace('/\n/', 'xxx', $lDeadlockMessage);
				$hGlobalStatus['Deadlock_message']  = $lDeadlockMessage;
			}   // Connection to zabbix_server ok/nok
		}
		// There was NO new Deadlock
		else {

			if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, "      NO new Deadlock found."); }
			$hGlobalStatus['Deadlock_new'] = 0;
		} // Deadlock does not exist or is new, so send it...
	}
	// We run fpmmm as MaaS
	else {

		// Deadlock does not exist yet or is a new Deadlock, so send it
		if ( $lSendFile == 1 ) {
			if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, "      New Deadlock found."); }

			$hGlobalStatus['Deadlock_new'] = 1;
			// Transform \n into marker 'xxx' because it will cause an error otherwise!
			// We could think about if it is sufficient to send just a Deadlock without the timestamp
			// this would cause only the first time a wrong information
			$lDeadlockMessage = preg_replace("/\n/", 'xxx', $lDeadlockMessage);
			$hGlobalStatus['Deadlock_message']  = $lDeadlockMessage;
		}
		// There was NO new Deadlock
		else {
			if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, "      NO new Deadlock found."); }
			$hGlobalStatus['Deadlock_new'] = 0;
		} // Deadlock does not exist or is new, so send it...
	}

	writeDataToCacheFile($pParameter, $hGlobalStatus, $aGlobalStatusToSend);
	return $rc;
}

// ---------------------------------------------------------------------
function handleForeignKeyMessage($pParameter, $lForeignKeyMessage)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '    ' . __FUNCTION__); }

	$lTimestamp = time();

	// Cleanup leading and trailing ----
	$lForeignKeyMessage = preg_replace('/-+\n/', '', $lForeignKeyMessage);

	$lSendFile = 0;
	// Does an old Foreign Key message exist?
	$lForeignKeyFile = dirname($pParameter['CacheFileBase']) . '/fpmmm_ForeignKeyMessage.' . $pParameter['Hostname'];
	if ( is_file($lForeignKeyFile) ) {

		if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, "      Old FK message files exists."); }

		// Write temporary Foreign Key file
		$lTmpForeignKeyFile = dirname($pParameter['CacheFileBase']) . '/fpmmm_ForeignKeyMessage.tmp.' . getmypid();
		if ( ! $fh = fopen($lTmpForeignKeyFile, 'w') ) {
			$rc = 1656;
			$a = error_get_last();
			mylog($pParameter['LogFile'], ERR, "    Cannot open file $lTmpForeignKeyFile (rc=$rc).");
			mylog($pParameter['LogFile'], ERR, "    Reason: " . $a['message']);
			return $rc;
		}
		fprintf($fh, "%s", $lForeignKeyMessage);
		fclose($fh);

		$hash1 = sha1_file($lTmpForeignKeyFile);
		$hash2 = sha1_file($lForeignKeyFile);

		// Both Foreign Key messages are qual
		if ( $hash1 == $hash2 ) {
			unlink($lTmpForeignKeyFile);
		}
		// the 2 Foreign Key messagess differ
		else {
			rename($lTmpForeignKeyFile, $lForeignKeyFile);
			$lSendFile = 1;
		}

	}
	// no, old Foreign Key message does NOT exist: then write it
	else {

		if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, "      Old FK message files does NOT exist."); }
		if ( ! $fh = fopen($lForeignKeyFile, 'w') ) {
			$rc = 1657;
			$a = error_get_last();
			mylog($pParameter['LogFile'], ERR, "    Cannot open file $lForeignKeyFile (rc=$rc).");
			mylog($pParameter['LogFile'], ERR, "    Reason: " . $a['message']);
			return $rc;
		}
		fprintf($fh, $lForeignKeyMessage);
		fclose($fh);
		$lSendFile = 1;
	}   // old Foreign Key message file exists?

	// --------

	$hGlobalStatus = array();

	$aGlobalStatusToSend = array (
		'ForeignKey_new'
	// , 'ForeignKey_message'   # only push messages if there is one!
	);

	// Only push message if there is one!
	if ( $lSendFile == 1 ) {
		array_push($aGlobalStatusToSend, 'ForeignKey_message');
	}

	initValues($hGlobalStatus, $aGlobalStatusToSend);
	$hGlobalStatus['ForeignKey_message'] = 'none';

	// rpmmm can be run in 2 modes now: locally or MaaS
	// We run fpmmm locally
	if ( $pParameter['MaaS'] != 'on' ) {

		// Deadlock does not exist yet or is a new Deadlock, so send it
		if ( $lSendFile == 1 ) {
			if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, "      New foreign key message found."); }

			$hGlobalStatus['ForeignKey_new'] = 1;
			$rc = checkConnectionToZabbixServer($pParameter);

			// Connection to zabbix server seems OK
			if ( $rc == OK ) {

				if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, "Connection to zabbix server seems OK."); }

				$key = 'ForeignKey_message';

				// This should work on CentOS/Ubuntu but also on SuSE
				$prg = 'zabbix_sender zabbix-sender';
				// Caution: Returns 1 because one of both does NOT exist...
				list($ret, $exe, $output, $stderr) = my_exec("which $prg");

				if ( ! isset($exe) ) {
					$rc = 1651;
					mylog($pParameter['LogFile'], ERR, "Cannot find program $exe.");
				}

				// This is an exception because with cache file we have not found a way
				// how to send the data with CR/LF
				$cmd = "$exe --zabbix-server " . $pParameter['ZabbixServer'] . " --port " . $pParameter['ZabbixServerPort'] . " --host '" . $pParameter['Hostname']. "' --key " . $pParameter['Domain'] . '.' . $key . " --value '" . $lForeignKeyMessage . "'";

				if ( $pParameter['LogLevel'] >= DBG ) {
					$cmd .= ' -vv';
					mylog($pParameter['LogFile'], DBG, $cmd);
				}

				list($ret, $output, $stdout, $stderr) = my_exec($cmd);

				$rc = 1654;
				if ( $pParameter['LogLevel'] >= DBG ) {
					mylog($pParameter['LogFile'], DBG, "      (ret=$ret / rc=$rc).");
					foreach ( $stdout as $line ) {
						mylog($pParameter['LogFile'], DBG, '      ' . $line);
					}
				}
				if ( ($ret != 0) && ($ret != 2) ) {
					foreach ( $stderr as $line ) {
						mylog($pParameter['LogFile'], ERR, '      ' . $line);
					}
				}

				// If zabbix_sender fails we should roll back the stuff to get all foreign key errors!
			}
			// No success connecting to zabbix server write to Cache File
			else {
				$rc = 1653;
				mylog($pParameter['LogFile'], WARN, "Connection to zabbix server failed (rc=$rc).");

				// Transform \n into marker 'xxx' because it will cause an error otherwise!
				// We could think about if it is sufficient to send just a Deadlock without the timestamp
				// this would cause only the first time a wrong information
				// not needed when we run direct and not as MaaS
				// $lForeignKeyMessage = preg_replace("/\n/", 'xxx', $lForeignKeyMessage);
				$hGlobalStatus['ForeignKey_message']  = $lForeignKeyMessage;
			}   // Connection to zabbix_server ok/nok
		}
		// There was NO new Foreign Key message
		else {

			if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, "      NO new foreign key found."); }
			$hGlobalStatus['ForeignKey_new'] = 0;
		} // Foreign Key message does not exist or is new, so send it...
	}
	// We run fpmmm as MaaS
	else {
		if ( $lSendFile == 1 ) {
			if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, "      New foreign key message found."); }

			$hGlobalStatus['ForeignKey_new'] = 1;
			// Transform \n into marker 'xxx' because it will cause an error otherwise!
			// We could think about if it is sufficient to send just a Deadlock without the timestamp
			// this would cause only the first time a wrong information
			$lForeignKeyMessage = preg_replace("/\n/", 'xxx', $lForeignKeyMessage);
			$hGlobalStatus['ForeignKey_message']  = $lForeignKeyMessage;
		}
		// There was NO new Foreign Key message
		else {
			if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, "      NO new foreign key found."); }
			$hGlobalStatus['ForeignKey_new'] = 0;
		} // Foreign Key message does not exist or is new, so send it...
	}

	writeDataToCacheFile($pParameter, $hGlobalStatus, $aGlobalStatusToSend);
	return $rc;
}

// ---------------------------------------------------------------------
function getInnoDbStatus($pParameter, $dbh, &$aEngineInnodbStatus)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '    ' . __FUNCTION__); }

	$sql = 'SHOW /*!50000 ENGINE */ INNODB STATUS';
	$lInnodbStatus = '';

	if ( $result = $dbh->query($sql) ) {

		if ( $row = $result->fetch_assoc() ) {
			$lInnodbStatus = $row['Status'];
		}
		else {
			$rc = 1661;
			mylog($pParameter['LogFile'], ERR, "    Cannot fetch (rc=$rc).");
		}
	}
	else {
		$rc = 1662;
		mylog($pParameter['LogFile'], ERR, "    Invalid query: $sql, " . $dbh->error . " (rc=$rc).");
		return $rc;
	}

	if ( $lInnodbStatus != '' ) {
		parseInnoDbStatus($pParameter, $dbh, $lInnodbStatus, $aEngineInnodbStatus);
	}

	return $rc;
}

// ---------------------------------------------------------------------
function parseInnoDbStatus($pParameter, $dbh, $lInnodbStatus, &$aEngineInnodbStatus)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '    ' . __FUNCTION__); }

	// Loop over all lines of output
	$section = '';
	$lDeadlockMessage   = '';
	$lForeignKeyMessage = '';
	foreach ( explode("\n", $lInnodbStatus) as $line ) {

		// print $line . "\n";

		if ( preg_match("/^BACKGROUND THREAD$/", $line, $matches) ) {
			$section = 'BACKGROUND THREAD';
			continue;
		}
		elseif ( preg_match("/^DEAD LOCK ERRORS$/", $line, $matches)
				||  preg_match("/^LATEST DETECTED DEADLOCK$/", $line, $matches)
					) {
			$section = 'DEAD LOCK ERRORS';
			continue;
		}
		elseif ( preg_match("/^FOREIGN KEY CONSTRAINT ERRORS$/", $line, $matches)
				|| preg_match("/^LATEST FOREIGN KEY ERROR$/", $line, $matches)
					) {
			$section = 'FOREIGN KEY CONSTRAINT ERRORS';
			continue;
		}
		elseif ( preg_match("/^SEMAPHORES$/", $line, $matches) ) {
			$section = 'SEMAPHORES';
			continue;
		}
		elseif ( preg_match("/^TRANSACTIONS$/", $line, $matches) ) {
			$section = 'TRANSACTIONS';
			continue;
		}
		elseif ( preg_match("/^FILE I\/O$/", $line, $matches) ) {
			$section = 'FILE I/O';
			continue;
		}
		elseif ( preg_match("/^INSERT BUFFER AND ADAPTIVE HASH INDEX$/", $line, $matches) ) {
			$section = 'INSERT BUFFER AND ADAPTIVE HASH INDEX';
			continue;
		}
		elseif ( preg_match("/^LOG$/", $line, $matches) ) {
			$section = 'LOG';
			continue;
		}
		elseif ( preg_match("/^BUFFER POOL AND MEMORY$/", $line, $matches) ) {
			$section = 'BUFFER POOL AND MEMORY';
			continue;
		}
		elseif ( preg_match("/^ROW OPERATIONS$/", $line, $matches) ) {
			$section = 'ROW OPERATIONS';
			continue;
		}

		// --------

		if ( $section == 'INSERT BUFFER AND ADAPTIVE HASH INDEX' ) {

			// Hash table size 276671, node heap has 810 buffer(s)
			if ( preg_match("/node heap has (\d+) buffer/", $line, $matches) ) {
				$aEngineInnodbStatus['Innodb_hash_node_size'] = 16384 * $matches[1];
				continue;
			}

			// 0.00 hash searches/s, 0.00 non-hash searches/s
			if ( preg_match("/^(\d+\.\d+)\s+hash searches\/s,\s+(\d+.\d+)\s+non-hash searches\/s/", $line, $matches) ) {
				$aEngineInnodbStatus['Innodb_hash_searches'] = $matches[1];
				$aEngineInnodbStatus['Innodb_non_hash_searches'] = $matches[2];
				continue;
			}
		}

		if ( $section == 'SEMAPHORES' ) {

			// Mutex spin waits 27329, rounds 60032, OS waits 929
			if ( preg_match("/^Mutex spin waits\s+(\d+),\s+rounds\s+(\d+),\s+OS waits\s+(\d+)/", $line, $matches) ) {
				$aEngineInnodbStatus['Innodb_mutex_spin_waits'] = $matches[1];
				$aEngineInnodbStatus['Innodb_mutex_spin_rounds'] = $matches[2];
				$aEngineInnodbStatus['Innodb_mutex_os_waits'] = $matches[3];
				continue;
			}
		}

		if ( $section == 'TRANSACTIONS' ) {

			// History list length 177
			if ( preg_match("/^History list length\s+(\d+)/", $line, $matches) ) {
				$aEngineInnodbStatus['Innodb_trx_history_list_length'] = $matches[1];
				continue;
			}
		}

		if ( $section == 'LOG' ) {

			// Log sequence number 13392740025
			// Log flushed up to   13392740025
			// Pages flushed up to 13392679774
			// Last checkpoint at  13392679671
			if ( preg_match("/^Log sequence number\s+(\d+)/", $line, $matches) ) {
				$aEngineInnodbStatus['Innodb_log_sequence_number'] = $matches[1];
				continue;
			}
			if ( preg_match("/^Log flushed up to\s+(\d+)/", $line, $matches) ) {
				$aEngineInnodbStatus['Innodb_log_flushed_up_to'] = $matches[1];
				continue;
			}
			// 5.6 feature?
			if ( preg_match("/^Pages flushed up to\s+(\d+)/", $line, $matches) ) {
				$aEngineInnodbStatus['Innodb_pages_flushed_up_to'] = $matches[1];
				continue;
			}
			if ( preg_match("/^Last checkpoint at\s+(\d+)/", $line, $matches) ) {
				$aEngineInnodbStatus['Innodb_last_checkpoint_at'] = $matches[1];
				continue;
			}
		}

		if ( $section == 'DEAD LOCK ERRORS' ) {
			$lDeadlockMessage .= $line . "\n";
		}

		if ( $section == 'FOREIGN KEY CONSTRAINT ERRORS' ) {
			$lForeignKeyMessage .= $line . "\n";
		}

		if ( $section == 'ROW OPERATIONS' ) {

			// 0 queries inside InnoDB, 0 queries in queue
			if ( preg_match("/^(\d+)\s+queries inside InnoDB,\s+(\d+)\s+queries in queue/", $line, $matches) ) {
				$aEngineInnodbStatus['Innodb_queries_inside_innodb'] = $matches[1];
				$aEngineInnodbStatus['Innodb_queries_in_queue'] = $matches[2];
				continue;
			}

			// 1 read views open inside InnoDB
			if ( preg_match("/^(\d+)\s+read views open inside InnoDB/", $line, $matches) ) {
				$aEngineInnodbStatus['Innodb_open_read_views_inside_innodb'] = $matches[1];
				continue;
			}
		}
	}   // loop over InnoDB status

	// Do we have a Deadlock Message?
	if ( $lDeadlockMessage != '' ) {
		handleDeadlockMessage($pParameter, $lDeadlockMessage);
	}
	else {
		if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, '      No Deadlock message.'); }
	}

	// Do we have a ForeignKey Message?
	if ( $lForeignKeyMessage != '' ) {
		handleForeignKeyMessage($pParameter, $lForeignKeyMessage);
	}
	else {
		if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, '      No Deadlock message.'); }
	}

	// Get InnoDB Compression Time
	$sql = 'SELECT SUM(compress_time) AS compress_time, SUM(uncompress_time) AS uncompress_time FROM information_schema.INNODB_CMP';

	if ( $result = $dbh->query($sql) ) {
		if ( $row = $result->fetch_assoc() ) {
				$aEngineInnodbStatus['Innodb_compress_time']   = intval($row['compress_time']);
				$aEngineInnodbStatus['Innodb_uncompress_time'] = intval($row['uncompress_time']);
		}
	}
	else {
		$rc = 1663;
		$msg = "Invalid query: $sql, Errno: " . $dbh->errno . ': ' .  $dbh->error;
		mylog($pParameter['LogFile'], ERR, '    ' . $msg . " (rc=$rc).");
	}

	// Get InnoDB Row Lock Details and InnoDB Transcation Lock Memory
	$sql = 'SELECT SUM(trx_rows_locked) AS rows_locked, SUM(trx_rows_modified) AS rows_modified, SUM(trx_lock_memory_bytes) AS lock_memory FROM information_schema.INNODB_TRX';

	if ( $result = $dbh->query($sql) ) {
		while ( $row = $result->fetch_assoc() ) {
				$aEngineInnodbStatus['Innodb_rows_locked']     = isset($row['rows_locked']) ? $row['rows_locked'] : 0;
				$aEngineInnodbStatus['Innodb_rows_modified']   = isset($row['rows_modified']) ? $row['rows_modified'] : 0;
				$aEngineInnodbStatus['Innodb_trx_lock_memory'] = isset($row['lock_memory']) ? $row['lock_memory'] : 0;
		}
	}
	else {
		$rc = 1664;
		$msg = "Invalid query: $sql, Errno: " . $dbh->errno . ': ' .  $dbh->error;
		mylog($pParameter['LogFile'], ERR, '    ' . $msg . " (rc=$rc).");
	}

	// Get InnoDB Transaction states
	$sql = 'SELECT LOWER(REPLACE(trx_state, " ", "_")) AS state, count(*) AS cnt from information_schema.INNODB_TRX GROUP BY state ORDER BY state';

	if ( $result = $dbh->query($sql) ) {
		if ( $row = $result->fetch_assoc() ) {
				$aEngineInnodbStatus['Innodb_trx_' . $row['state']] = $row['cnt'];
		}
	}
	else {
		$rc = 1665;
		$msg = "Invalid query: $sql, Errno: " . $dbh->errno . ': ' .  $dbh->error;
		mylog($pParameter['LogFile'], ERR, '    ' . $msg . " (rc=$rc).");
	}


	if ( $pParameter['LogLevel'] >= DBG ) {
		mylog($pParameter['LogFile'], DBG, '    InnoDB Status:');
		foreach ( array_keys($aEngineInnodbStatus) as $key ) {
			mylog($pParameter['LogFile'], DBG, "      key: $key, value: " . $aEngineInnodbStatus[$key]);
		}
	}

	return $rc;
}

?>
