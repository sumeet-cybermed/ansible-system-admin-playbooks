<?php

#
# Copyright (C) 2010-2016 FromDual GmbH
#


/* 

	printUsage($pMyName, $pDefaultOptions)
	cleanAndQuit($pParameter, $pRemoveLockFile, $pRc)
	setAgentLock($pParameter, &$pRemoveLockFile)
	clearCacheFile($pCacheFile)
	checkConnectionToZabbixServer($pParameter)
	readConfigurationFile($file)
	getSections($config)
	readSection($config, $section)
	getDefaultParameter($config)
	getParameter($config, $section)
	mylog($logfile, $lvl, $str)
	getDatabaseConnection($pParameter)
	releaseDatabaseConnection($dbh)
	initValues($variables_ref, $variables_to_send_ref)
	getGlobalVariables($dbh, $variables_ref)
	getGlobalStatus($dbh, $status_ref)
	removeAgentLock($pParameter, $gRemoveLockFile)

*/

// ---------------------------------------------------------------------
function printUsage($pMyName, $pDefaultOptions)
// ---------------------------------------------------------------------
{
	print "
Usage: $pMyName --config=<file_name> [--daemon] --interval=<time>
                              [--help] [--version]

Options:
  config          Configuration file to read parameter from (default=" . $pDefaultOptions['config'] . ").
  daemon          Make fpmmm working as a daemon.
  interval        Time in seconds after which the data should be gatheread
                  again (default=" . $pDefaultOptions['interval'] . ").
  help            Prints this help.
  version         Print version.

Examples:

  $pMyName --config=" . $pDefaultOptions['config'] . " --daemon --interval=" . $pDefaultOptions['interval'] . "

";
}

// ---------------------------------------------------------------------
function cleanAndQuit($pParameter, $pRemoveLockFile, $pRc)
// do a proper exit here
// This should be the only exit for the whole fpmmm!
// ---------------------------------------------------------------------
{

	// Some exits have not defined:
	// $pRemoveLockFile
	// $pParameter['LogLevel']
	// This causes some exits in the agent where we do not see the reason
	// todo: Ugly hack should be fixed in the next release
	if ( ! isset($pRemoveLockFile) ) {
		$pRemoveLockFile = 0;
	}
	if ( ! isset($pParameter['LogLevel']) ) {
		$pParameter['LogLevel'] = ERR;
	}

	// Remove Lock file if exists.
	if ( $pRemoveLockFile == 1 ) {

		if ( isset($pParameter['AgentLockFile']) ) {

			if ( file_exists($pParameter['AgentLockFile']) ) {
				$ret = removeAgentLock($pParameter, $pRemoveLockFile);
			}
		}
	}

	// Print exit message to log if exists or to stdout.
	$msg = "FromDual Performance Monitor for MySQL and MariaDB (fpmmm) run finished (rc=$pRc).";
	if ( isset($pParameter['LogFile'])
		&& $pParameter['LogFile'] != ''
		&& file_exists($pParameter['LogFile'])
		&& is_writeable($pParameter['LogFile']) ) {
		mylog($pParameter['LogFile'], INFO, $msg);
	}
	else {
		print "$msg\n";
	}

	print "1\n";
	exit($pRc);
}

// ----------------------------------------------------------------------
function setAgentLock($pParameter, &$pRemoveLockFile)
// ----------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '  ' . __FUNCTION__); }

	// Check if lock file already exists and complain if yes
	if ( file_exists($pParameter['AgentLockFile']) ) {
		$rc = 1100;
		if ( $pParameter['LogLevel'] >= WARN ) { mylog($pParameter['LogFile'], WARN, "  Agent Lock file " . $pParameter['AgentLockFile'] . " already exists."); }

		// Check the mtime of the lock file
		// If agent was running too long, kill it!
		//
		if ( ($a = @stat($pParameter['AgentLockFile'])) === false ) {
			$rc = 1119;
			$msg = 'Lock file ' . $pParameter['AgentLockFile'] . " disappeared while we were reading it (rc=$rc).";
			mylog($pParameter['LogFile'], ERR, '  ' . $msg);
			return $rc;
		}
		$mtime = $a[9];
		$mtime = isset($mtime) ? $mtime : 0;
		$now = time();

		// This is possibly too long for a local installation?
		$timeout = 60;
		// Other agent was running for more than timeout seconds
		if ( ($now - $mtime) > $timeout ) {
			$rc = 1101;
			mylog($pParameter['LogFile'], ERR, "  Agent was running for more than $timeout seconds (now: " . date('Y-m-d H:i:s', $now) . ", mtime: " . date('Y-m-d H:i:s', $mtime) . "). Killing it now! (rc=$rc).");

			// cat $pParameter['AgentLockFile']
			// todo: Could be done in function like getPidFromFile or so. (see also getMySQLPid function)
			if ( $fh = fopen($pParameter['AgentLockFile'], 'r') ) {
				$pid = intval(trim(fgets($fh)));
				fclose($fh);
				// It looks like this situation happens during log rotate!!!
				if ( $pid > 0 ) {
					// Terminate program
					mylog($pParameter['LogFile'], ERR, "  Kill (-TERM) agent with PID $pid (our PID is ". getmypid() . ").");
					$cnt = posix_kill($pid, SIGTERM);
					if ( $cnt == 0 ) {
						mylog($pParameter['LogFile'], ERR, "  Kill (-KILL) agent with PID $pid (our PID is " . getmypid() . ").");
						$cnt = posix_kill($pid, SIGKILL);
					}
					$ret = removeAgentLock($pParameter, 1);
					$ret = setAgentLock($pParameter, $pRemoveLockFile);
				}
				else {
					// It looks like this situation happens during log rotate!!!
					mylog($pParameter['LogFile'], ERR, "  PID file was empty again!");
				}
			}
		}
		// Other agent was running for less than timeout seconds 
		else {
			$rc = 1102;
			$pid = 0;
			if ( $fh = fopen($pParameter['AgentLockFile'], 'r') ) {
				$pid = intval(trim(fgets($fh)));
				fclose($fh);
			}
			mylog($pParameter['LogFile'], WARN, "  Another agent with PID $pid in " . $pParameter['AgentLockFile'] . " (our PID is " . getmypid() . ") is running. We will abort now.");
			// This message seems to be redundant with cleanAndQuit
			// mylog($pParameter['LogFile'], WARN, "FromDual Performance Monitor for MySQL and MariaDB (fpmmm) run finshed (rc=$rc).");
			$pRemoveLockFile = 0;
			// todo: ret should be returned
			if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, '  RemoveLockFile is set to ' . $pRemoveLockFile . '.'); }
			cleanAndQuit($pParameter, $pRemoveLockFile, $rc);
		}
	}
	// Agent lock file does NOT exits!
	else {

		$lAgentLockFileDir = dirname($pParameter['AgentLockFile']);
		if ( ! is_dir($lAgentLockFileDir) ) {
			mylog($pParameter['LogFile'], INFO, "  Directory $lAgentLockFileDir does NOT exist. Creating it now...");
			if ( ! @mkdir($lAgentLockFileDir, 0777, true) ) {
				$rc = 1103;
				$a = error_get_last();
				mylog($pParameter['LogFile'], ERR, "  Could not create directory $lAgentLockFileDir (rc=$rc).");
				mylog($pParameter['LogFile'], ERR, "  Reason: " . $a['message']);
				cleanAndQuit($pParameter, 0, $rc);
			}
		}

		// Create a new agent lock file
		if ( ! $handle = @fopen($pParameter['AgentLockFile'], 'w') ) {

			$rc = 1104;
			$a = error_get_last();
			mylog($pParameter['LogFile'], ERR, "  Cannot write to Lock File " . $pParameter['AgentLockFile'] . " (rc=$rc).");
			mylog($pParameter['LogFile'], ERR, "  Reason: " . $a['message']);
			cleanAndQuit($pParameter, 0, $rc);
		}
		else {
			fprintf($handle, "%d\n", getmypid());
			fclose($handle);
			$pRemoveLockFile = 1;
		}
	}

	if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, '  RemoveLockFile is set to ' . $pRemoveLockFile . '.'); }
	return $rc;
}

// ----------------------------------------------------------------------
function clearCacheFile($pParameter, $pCacheFile)
// ----------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '      ' . __FUNCTION__); }

	// Clear Cache File
	if ( ! $FH = fopen($pCacheFile, 'w') ) {
		$rc = 1105;
		$a = error_get_last();
		mylog($pParameter['LogFile'], ERR, "  Cannot write to Cache File $pCacheFile (rc=$rc).");
		mylog($pParameter['LogFile'], ERR, "  Reason: " . $a['message']);
		return $rc;
	}
	fclose($FH);
	return $rc;
}

// ----------------------------------------------------------------------
function checkConnectionToZabbixServer($pParameter)
// ----------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '      ' . __FUNCTION__); }

	// This tag does NOT exist in templates!!!
	$lKey    = 'FromDual.server.check';
	$lValue  = 1;

	if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, "      Check connection to zabbix server."); }

	// This should work on CentOS/Ubuntu but also on SuSE
	$prg = 'zabbix_sender zabbix-sender';
	list($ret, $exe, $output, $stderr) = my_exec("which $prg");
	// Caution: Returns 1 because one of both does NOT exist...
	if ( $exe == '' ) {
		$rc = 1106;
		mylog($pParameter['LogFile'], ERR, "      Cannot find program $prg (rc=$rc).");
		foreach ( $stderr as $l ) {
			mylog($pParameter['LogFile'], ERR, '      ' . $l);
		}
		return $rc;
	}

	$cmd = "$exe --zabbix-server " . $pParameter['ZabbixServer'] . " --port " . $pParameter['ZabbixServerPort'] . " --host '" . $pParameter['Hostname'] . "' --key $lKey --value '$lValue'";
	if ( $pParameter['LogLevel'] >= DBG ) {
		$cmd .= ' -vv';
		mylog($pParameter['LogFile'], DBG, '      ' . $cmd);
	}

	list($ret, $output, $stdout, $stderr) = my_exec($cmd);

	if ( $pParameter['LogLevel'] >= DBG ) {
		mylog($pParameter['LogFile'], DBG, "      (ret=$ret).");
		foreach ( $stdout as $line ) {
			mylog($pParameter['LogFile'], DBG, '      ' . $line);
		}
	}
	if ( ($ret != 0) && ($ret != 2) ) {
		// STDERR seems to be not enough!
		foreach ( $stdout as $line ) {
			mylog($pParameter['LogFile'], DBG, '      ' . $line);
		}
		foreach ( $stderr as $line ) {
			mylog($pParameter['LogFile'], ERR, '      ' . $line);
		}
	}

	// EXIT STATUS
	// The exit status is 0 if the values were sent and all of them were
	// successfully processed by server. If data was sent, but processing
	// of at least one of the values failed, the exit status is 2. If
	// data sending failed, the exit status is 1.

	if ( $ret == OK ) {
		return $rc;
	}
	elseif ( $ret == 1 ) {
		$rc = 1107;
		mylog($pParameter['LogFile'], ERR, "      Failed to execute $exe (rc=$rc/ret=$ret).");
		foreach ( $stderr as $line ) {
			mylog($pParameter['LogFile'], ERR, '      ' . $line);
		}
		return $rc;
	}
	elseif ( $ret == 2 ) {
		// Change in Zabbix 2.1.7
		// [ZBXNEXT-935] zabbix sender exit status now better reflects the operation result:
		// success: 0
		// partial success: 2
		// failure: 1
		if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], DBG, "      Partial success (ret=$ret / rc=$rc)."); }
	}
	elseif ( $ret & 127 ) {
		$rc = 1108;
		if ( $pParameter['LogLevel'] >= DBG ) { mylog($pParameter['LogFile'], ERR, "      " . sprintf("Child died with signal %d, %s coredump", ($ret & 127), ($ret & 128) ? 'with' : 'without') . " (rc=$rc/ret=$ret)."); }
		return $rc;
	}
	else {
		$rc = 1109;
		if ( $pParameter['LogLevel'] >= ERR ) { mylog($pParameter['LogFile'], ERR, "      " . sprintf("Child exited with value %d", $ret) . " (rc=$rc/ret=$ret)."); }
		return $rc;
	}

	return $rc;
}

// ----------------------------------------------------------------------
function readConfigurationFile($pParameter, $file)
// ----------------------------------------------------------------------
{
	$rc = OK;
	$lRemoveLockFile = 1;

	if ( ! file_exists($file) ) {
		$rc = 1110;
		print "Configuration file $file does not exist (rc=$rc).\n";
		cleanAndQuit($pParameter, $lRemoveLockFile, $rc);
	}

	if ( ! is_readable($file) ) {
		$rc = 1111;
		print "Cannot read configuration file $file (rc=$rc).\n";
		cleanAndQuit($pParameter, $lRemoveLockFile, $rc);
	}

	$config = array();
	if ( ! $fh = fopen($file, 'r') ) {
		$rc = 1112;
		$a = error_get_last();
		print "Cannot open file $file (rc=$rc).";
		print "Reason: " . $a['message'] . "\n";
		cleanAndQuit($pParameter, $lRemoveLockFile, $rc);
	}
	while ( $line = fgets($fh) ) {
		$line = trim($line);                  // remove newlines
		$line = preg_replace('/^\s*\/\/.*/', '', $line);            // remove comment
		$line = preg_replace('/^\s+/', '', $line);               // remove leading white space
		$line = preg_replace('/\s+$/', '', $line);               // remove trailing white space
		if ( $line == '' ) {
			continue;
		}
		array_push($config, $line);
	}
	fclose($fh);

	return $config;
}

// ----------------------------------------------------------------------
function getSections($config)
// ----------------------------------------------------------------------
{
	$aSections = array();
	foreach ( $config as $line ) {

		if ( preg_match('/^\[(.+)\]/', $line, $matches) ) {
			array_push($aSections, $matches[1]);
		}
	}

	return $aSections;
}

// ----------------------------------------------------------------------
function readSection($config, $section)
// ----------------------------------------------------------------------
{
	$aSection = array();

	$in_section = 0;
	foreach ( $config as $line ) {

		if ( preg_match('/^\[(.+)\]/', $line, $matches) ) {

			if ( $matches[1] == $section ) {
				$in_section = 1;
				continue;
			}
			else {
				if ( $in_section == 1 ) {
					break;
				}
				$in_section = 0;
				continue;
			}
		}

		if ( $in_section && ( preg_match('/^(\S+)\s*=\s*(.+)\s*$/', $line, $matches) ) ) {
			$aSection[$matches[1]] = $matches[2];
		}
	}

	return $aSection;
}

// ----------------------------------------------------------------------
function getDefaultParameter($config)
// ----------------------------------------------------------------------
{
	$rc = OK;
	$hDefaults = array();
	$lRemoveLockFile = 1;

	$lOs = getOs();

	if ( $lOs == 'MSWin' ) {
		$hDefaults['Socket']           = 'C:\Program Files\MySQL\MySQL Server 5.5\data\mysql.sock';
		$hDefaults['LogFile']          = 'C:\Temp\fpmmm.log';
		$hDefaults['CacheFileBase']    = 'C:\Temp\fpmmmCache';
		$hDefaults['AgentLockFile']    = 'C:\Temp\fpmmm.lock';
	}
	elseif ( $lOs == 'Linux' ) {
		// Those are not the correct ones but they cause less problems and thus
		// better user experience
		$hDefaults['Socket']           = '/var/lib/mysql/mysql.sock';
		$hDefaults['LogFile']          = '/tmp/fpmmm.log';
		$hDefaults['CacheFileBase']    = '/tmp/fpmmmCache';
		$hDefaults['AgentLockFile']    = '/tmp/fpmmm.lock';
	}
	elseif ( $lOs ==  'Darwin' ) {
		$hDefaults['Socket']           = '/var/lib/mysql/mysql.sock';
		$hDefaults['LogFile']          = '/tmp/fpmmm.log';
		$hDefaults['CacheFileBase']    = '/tmp/fpmmmCache';
		$hDefaults['AgentLockFile']    = '/tmp/fpmmm.lock';
	}
	else {
		$rc = 1113;
		print 'O/S ' . $lOs . " is not supported yet (rc=$rc).\n";
		cleanAndQuit($hDefaults, $lRemoveLockFile, $rc);
	}

	$hDefaults['LogLevel']            = WARN;
	$hDefaults['LogLevel']            = 4;
	$hDefaults['LogLevel']            = ERR;
	$hDefaults['TimeShift']        = 0.0;
	$hDefaults['Disabled']         = 'false';
	$hDefaults['Modules']          = 'mysql myisam process';
	$hDefaults['MysqlHost']        = '127.0.0.1';
	$hDefaults['MysqlPort']        = '3306';
	$hDefaults['Password']         = '';
	$hDefaults['PidFile']          = '';
	$hDefaults['Type']             = 'mysqld';
	$hDefaults['Username']         = 'root';
	$hDefaults['ZabbixServer']     = '';

	$hDefaults['ZabbixServerPort'] = '10051';

	$hDefaults['MaaS']             = 'off';
	$hDefaults['Hash']             = '';
	$hDefaults['Methode']          = 'https';
	$hDefaults['Url']              = 'https://support.fromdual.com/maas/receiver.php';

	// Read default from configuration
	$default = readSection($config, 'default');

	foreach ( $default as $key => $dummy ) {

		if ( $default[$key] != '' ) {
			$hDefaults[$key] = $default[$key];
		}
	}

	return $hDefaults;
}

// ----------------------------------------------------------------------
function getParameter($config, $section)
// ----------------------------------------------------------------------
{
	// The logic is:
	// - Get defaults
	// - Get section parameters
	// - Overwrite defaults by section parameters
	// - Return defaults as section parameters!

	$defaults = getDefaultParameter($config);

	if ( $defaults['LogLevel'] >= DBG ) {
		mylog($defaults['LogFile'], DBG, "The defaults parameters of this configuration file are:");
		foreach ( $defaults as $p => $dummy ) {

			// This would be possibly a security problem...
			if ( $p == 'Password' ) {
				mylog($defaults['LogFile'], DBG, "  $p - ********");
			}
			else {
				mylog($defaults['LogFile'], DBG, "  $p - $defaults[$p]");
			}
		}
	}

	$parameter = readSection($config, $section);

	if ( $defaults['LogLevel'] >= DBG ) {
		mylog($defaults['LogFile'], DBG, "The parameters of section $section are:");
		foreach ( $parameter as $p => $dummy ) {

			// This would be possibly a security problem...
			if ( $p == 'Password' ) {
				mylog($defaults['LogFile'], DBG, "  $p - ********");
			}
			else {
				mylog($defaults['LogFile'], DBG, "  $p - $parameter[$p]");
			}
		}
	}

	foreach ( $parameter as $key => $dummy ) {

		if ( $parameter[$key] != '' ) {
			$defaults[$key] = $parameter[$key];
		}
	}
	// Do some data normalization here...
	$defaults['Hostname'] = $section;
	$defaults['CacheFile'] = $defaults['CacheFileBase'] . '.' . $section . '.cache';
	if ( isset($defaults['MaaS']) ) {
		$defaults['MaaS'] = strtolower($defaults['MaaS']);
	}
	if ( isset($defaults['Methode']) ) {
		$defaults['Methode'] = strtolower($defaults['Methode']);
	}

	return $defaults;
}

// ----------------------------------------------------------------------
function getDatabaseConnection($pParameter)
// ----------------------------------------------------------------------
{
	$rc = OK;

// Has been moved to fpmmmInstaller.inc
// 	if ( extension_loaded('mysqli') === false ) {
// 		$rc = 1114;
// 		mylog($pParameter['LogFile'], WARN, "Warning: No PHP mysqli extension found. Please install php5-mysql (rc=$rc).");
// 		$lRemoveLockFile = 1;
// 		$ret = removeAgentLock($pParameter, $lRemoveLockFile);
// 		cleanAndQuit($pParameter, $lRemoveLockFile, $rc);
// 	}

	// We do not specify a database, it could be we do not have the
	// privileges for
	$dbh = @new mysqli($pParameter['MysqlHost'], $pParameter['Username'], $pParameter['Password'], null, $pParameter['MysqlPort'], null);

	if ( mysqli_connect_errno() ) {
		$lDatabase = 'mysql';
		$msg = "mysqli connect with database=$lDatabase, host=" . $pParameter['MysqlHost'] . ', port=' . $pParameter['MysqlPort'] . ' and user=' . $pParameter['Username'] . ' failed: ' . mysqli_connect_error();
		mylog($pParameter['LogFile'], ERR, '    ' . $msg);
		// print "$msg\n";
		$dbh = false;
	}

	return $dbh;
}

// ----------------------------------------------------------------------
function releaseDatabaseConnection($dbh)
// ----------------------------------------------------------------------
{
	$dbh->close();
}

// ----------------------------------------------------------------------
function initValues(&$aVariables, $aVariablesToSend)
// ----------------------------------------------------------------------
{
	foreach ( $aVariablesToSend as $key ) {
		$aVariables[$key] = 0;
	}
}

// ----------------------------------------------------------------------
function getGlobalVariables($pParameter, $dbh, &$aGlobalVariables)
// ----------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '    ' . __FUNCTION__); }

	$sql = 'SHOW GLOBAL VARIABLES';
	if ( $result = $dbh->query($sql) ) {
		while ( $record = $result->fetch_assoc() ) {
				$aGlobalVariables[$record['Variable_name']] = $record['Value'];
		}
	}
	else {
		$rc = 1117;
		$msg = "Invalid query: $sql, Errno: " . $dbh->errno . ': ' .  $dbh->error;
		mylog($pParameter['LogFile'], ERR, '    ' . $msg . " (rc=$rc).");
	}

	if ( $pParameter['LogLevel'] >= DBG ) {
		mylog($pParameter['LogFile'], DBG, '    Global Variables:');
		foreach ( $aGlobalVariables as $variable => $value ) {
			mylog($pParameter['LogFile'], DBG, "      variable: $variable, value: " . $value);
		}
	}
	return $rc;
}

// ----------------------------------------------------------------------
function getGlobalStatus($pParameter, $dbh, &$aGlobalStatus)
// ----------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '    ' . __FUNCTION__); }

	$sql = 'SHOW /*!50000 GLOBAL */ STATUS';
	
	if ( $result = $dbh->query($sql) ) {
		while ( $record = $result->fetch_assoc() ) {
				$aGlobalStatus[$record['Variable_name']] = $record['Value'];
		}
	}
	else {
		$rc = 1118;
		$msg = "Invalid query: $sql, Errno: " . $dbh->errno . ': ' .  $dbh->error;
		mylog($pParameter['LogFile'], ERR, '    ' . $msg . " (rc=$rc).");
	}

	if ( $pParameter['LogLevel'] >= DBG ) {
		mylog($pParameter['LogFile'], DBG, '    Global Status:');
		foreach ( $aGlobalStatus as $status => $value ) {
			mylog($pParameter['LogFile'], DBG, "      status: $status, value: " . $value);
		}
	}
	return $rc;
}

// ----------------------------------------------------------------------
function removeAgentLock($pParameter, $pRemoveLockFile)
// ----------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '  ' . __FUNCTION__); }

	if ( $pRemoveLockFile == 1 ) {
		if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, "    gRemoveLockFile IS set."); }

		$lAgentLockFile = $pParameter['AgentLockFile'];
		if ( ! is_file($lAgentLockFile) ) {
			$rc = 1115;
			if ( $pParameter['LogLevel'] >= ERR ) {
				mylog($pParameter['LogFile'], ERR, "    Lock file $lAgentLockFile does not exist (rc=$rc).");
			}
		}
		else {
			if ( ! @unlink($lAgentLockFile) ) {
				$rc = 1116;
				if ( $pParameter['LogLevel'] >= ERR ) {
					$a = error_get_last();
					mylog($pParameter['LogFile'], ERR, "    Cannot delete lock file $lAgentLockFile (rc=$rc).");
					mylog($pParameter['LogFile'], ERR, "    Reason: " . $a['message']);
				}
			}
		}
	}
	// $pRemoveLockFile == 0
	else {
		if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, "    gRemoveLockFile is NOT set."); }
		if ( $pParameter['LogLevel'] >= DBG ) {
			mylog($pParameter['LogFile'], DBG, "    Stack Trace:");
			mylog($pParameter['LogFile'], DBG, print_r(debug_backtrace(), true));
		}
	}
	return $rc;
}

?>
