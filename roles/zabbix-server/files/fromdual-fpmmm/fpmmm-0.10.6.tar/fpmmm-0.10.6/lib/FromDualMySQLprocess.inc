<?php

#
# Copyright (C) 2010-2016 FromDual GmbH
#


/*

	getMysqlPid($pid_file)
	getNumberOfFileDescriptors($pid)
	getStat($pParameter, $pid)
	processProcessInformation()

*/

// ---------------------------------------------------------------------
function getMysqlPid($pPidFile)
// ---------------------------------------------------------------------
{
	$pid = 0;
	$pid = trim(file_get_contents($pPidFile));
	return $pid;
}

/*
// ---------------------------------------------------------------------
function getMysqlClusterPid($pid_file)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '    ' . __FUNCTION__); }

	$ppid = 0;
	$ppid = trim(file_get_contents($pid_file));

	list($ret, $stdout, $output, $stderr) = my_exec("ps -ef | grep $ppid");

	// root      5388  9857  0 15:35 pts/1    00:00:00 grep 25353
	// root     25353     1  0 06:52 ?        00:00:00 ./ndbd
	// root     25354 25353  0 06:52 ?        00:01:35 ./ndbd

	$pid = 0;
	foreach ( $stdout as $line ) {
		if ( preg_match('/^\w+?\s+(\d+)\s+(\d+).*$/', $line, $matches) ) {
			if ( $matches[2] == $ppid ) {
				$pid = $matches[1];
				break;
			}
		}
	}
	return $pid;
}
*/

// ---------------------------------------------------------------------
function getNumberOfFileDescriptors($pid)
// ---------------------------------------------------------------------
{
	$count = count(glob("/proc/$pid/fd/*"));
	return $count;
}

// ---------------------------------------------------------------------
function getStat($pParameter, $pid)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '    ' . __FUNCTION__); }

	$hProcessStatistics = array();
	$hProcessStatistics['check'] = 1;

	// status = stat + statm
	// Get heap/stack size of process
	// see man proc
	$cmd = "cat /proc/$pid/stat";
	list($ret, $out, $output, $stderr) = my_exec($cmd);
	
	if ( $ret != OK ) {
		$rc = 1480;
		mylog($pParameter['LogFile'], ERR, "    Error in command $cmd (rc=$rc/ret=$ret).");
		foreach ( $stderr as $line ) {
			mylog($pParameter['LogFile'], ERR, '    ' . $line);
		}
		return array($rc, null);
	}

	$stdout = explode(" ", $out);


	// 2524 (mysqld) S 2441 2438 2319 34817 24274 4202496 4583 0 162 0 1268 100 0 0 20 0 10 0 16938 112553984 580 4294967295
	// 0                         5                             10               15          20
	// 134512640 141534372 3214400624 3214399800 5821474 0 552967 4102 26345 4294967295 0 0 17 0 0 0 0 0 0
	// 25                                                30                             35         40

	//  0 pid $d                      The process ID.
	//  1 comm $s                     The filename of the executable, in parentheses.
	//  2 state $c                    One  character from the string.
	//  3 ppid $d                     The PID of the parent.
	//  4 pgrp $d                     The process group ID of the process.
	//  5 session $d                  The session ID of the process.
	//  6 tty_nr $d                   The controlling terminal of the process.
	//  7 tpgid $d                    The ID of the foreground process group of the controlling terminal of the process.
	//  8 flags $u                    The kernel flags word of the process.
	//  9 minflt $lu                  The number of minor faults the process has made which have not required loading a memory page from disk.
	$hProcessStatistics['minor_faults'] = $stdout[9];
	// 10 cminflt $lu                 The number of minor faults that the process's waited-for children have made.
	$hProcessStatistics['children_minor_faults'] = $stdout[10];
	// 11 majflt $lu                  The number of major faults the process has made which have required loading a memory page from disk.
	$hProcessStatistics['major_faults'] = $stdout[11];
	// 12 cmajflt $lu                 The number of major faults that the process's waited-for children have made.
	$hProcessStatistics['children_major_faults'] = $stdout[12];
	// 13 utime $lu                   Amount of time that this process has been scheduled in user mode (in clock ticks).
	$hProcessStatistics['user_time'] = $stdout[13];
	// 14 stime $lu                   Amount of time that this process has been scheduled in kernel mode (in clock ticks).
	$hProcessStatistics['system_time'] = $stdout[14];
	// 15 cutime $ld                  Amount of time that this process's waited-for children have been scheduled in user mode (in clock ticks).
	$hProcessStatistics['children_user_time'] = $stdout[15];
	// 16 cstime $ld                  Amount of time that this process's waited-for children have been scheduled in kernel mode (in clock ticks).
	$hProcessStatistics['children_system_time'] = $stdout[16];
	// 17 priority $ld                Scheduling priority.
	$hProcessStatistics['scheduling_priortiy'] = $stdout[17];
	// 18 nice $ld                    The nice value, a value in the range 19 (low priority) to -20 (high priority).
	$hProcessStatistics['nice_value'] = $stdout[18];
	// 19 num_threads $ld             Number of threads in this process.
	$hProcessStatistics['number_of_threads'] = $stdout[19];
	// 20 itrealvalue $ld
	// 21 starttime $llu
	// 22 vsize $lu                   Virtual memory size in bytes.
	$hProcessStatistics['virtual_memory_size'] = $stdout[22];
	// 23 rss $ld                     Resident Set Size: number of pages the process has in real memory (text, data, stack). NOT not demand-loaded or swapped out.
	$hProcessStatistics['resident_memory_size'] = $stdout[23];
	// 24 rsslim $lu                  Current soft limit in bytes on the rss of the process.
	//$hProcessStatistics['rss_soft_limit'] = $stdout[24];
	// 25 startcode $lu               The address above which program text can run.
	// 26 endcode $lu                 The address below which program text can run.
	// 27 startstack $lu              The address of the start of the stack.
	// 28 kstkesp $lu                 The current value of ESP (stack pointer).
	// 29 kstkeip $lu                 The current EIP (instruction pointer).
	// 30 signal $lu                  The bitmap of pending signals, displayed as a decimal number
	// 31 blocked $lu                 The  bitmap  of  blocked  signals, displayed as a decimal number.
	// 32 sigignore $lu               The bitmap of ignored signals, displayed as a decimal number.
	// 33 sigcatch $lu                The  bitmap  of  caught  signals,  displayed as a decimal number.
	// 34 wchan $lu                   This is the "channel" in which the process is waiting.
	// 35 nswap $lu                   Number of pages swapped (not maintained).
	// 36 cnswap $lu                  Cumulative nswap for child processes (not maintained).
	// 37 exit_signal $d              Signal to be sent to parent when we die.
	// 38 processor $d                CPU number last executed on.
	$hProcessStatistics['last_cpu_number'] = $stdout[38];
	// 39 rt_priority $u              Real-time  scheduling  priority,  a  number  in  the  range  1  to  99  for  processes scheduled under a real-time policy, or 0, for non-real-time processes.
	$hProcessStatistics['rt_priority'] = $stdout[39];
	// 40 policy $u                   Scheduling policy.
	$hProcessStatistics['scheduling_policy'] = $stdout[40];
	// 41 delayacct_blkio_ticks $llu  Aggregated block I/O delays, measured in clock ticks (centiseconds).
	$hProcessStatistics['block_io_delays'] = $stdout[41];
	// 42 guest_time $lu              Guest time of the process (time spent running a virtual CPU for a guest operating system), (in clock ticks).
	$hProcessStatistics['guest_time'] = isset($stdout[42]) ? intval($stdout[42]) : 0;
	// 43 cguest_time $ld             Guest time of the process's children, (in clock ticks).
	$hProcessStatistics['children_guest_time'] = isset($stdout[43]) ? intval($stdout[43]) : 0;

	list($ret, $stdout, $output, $stderr) = my_exec("cat /proc/$pid/status");

	foreach ( $output as $line ) {
		if ( preg_match('/^(\w+):\s+(.*)$/', $line, $matches) ) {

			if ( $matches[1] == 'FDSize' ) {
				$hProcessStatistics['FDSize'] = $matches[2];
			}
			if ( $matches[1] == 'VmPeak' ) {
				$hProcessStatistics['VmPeak'] = $matches[2];
				$hProcessStatistics['VmPeak'] = str_replace(' kB', '', $hProcessStatistics['VmPeak']);
				$hProcessStatistics['VmPeak'] *= 1024;
			}
			if ( $matches[1] == 'VmSize' ) {
				$hProcessStatistics['VmSize'] = $matches[2];
				$hProcessStatistics['VmSize'] = str_replace(' kB', '', $hProcessStatistics['VmSize']);
				$hProcessStatistics['VmSize'] *= 1024;
			}
			if ( $matches[1] == 'VmLck' ) {
				$hProcessStatistics['VmLck'] = $matches[2];
				$hProcessStatistics['VmLck'] = str_replace(' kB', '', $hProcessStatistics['VmLck']);
				$hProcessStatistics['VmLck'] *= 1024;
			}
			if ( $matches[1] == 'VmHWM' ) {
				$hProcessStatistics['VmHWM'] = $matches[2];
				$hProcessStatistics['VmHWM'] = str_replace(' kB', '', $hProcessStatistics['VmHWM']);
				$hProcessStatistics['VmHWM'] *= 1024;
			}
			if ( $matches[1] == 'VmRSS' ) {
				$hProcessStatistics['VmRSS'] = $matches[2];
				$hProcessStatistics['VmRSS'] = str_replace(' kB', '', $hProcessStatistics['VmRSS']);
				$hProcessStatistics['VmRSS'] *= 1024;
			}
			if ( $matches[1] == 'VmData' ) {
				$hProcessStatistics['VmData'] = $matches[2];
				$hProcessStatistics['VmData'] = str_replace(' kB', '', $hProcessStatistics['VmData']);
				$hProcessStatistics['VmData'] *= 1024;
			}
			if ( $matches[1] == 'VmStk' ) {
				$hProcessStatistics['VmStk'] = $matches[2];
				$hProcessStatistics['VmStk'] = str_replace(' kB', '', $hProcessStatistics['VmStk']);
				$hProcessStatistics['VmStk'] *= 1024;
			}
			if ( $matches[1] == 'VmExe' ) {
				$hProcessStatistics['VmExe'] = $matches[2];
				$hProcessStatistics['VmExe'] = str_replace(' kB', '', $hProcessStatistics['VmExe']);
				$hProcessStatistics['VmExe'] *= 1024;
			}
			if ( $matches[1] == 'VmLib' ) {
				$hProcessStatistics['VmLib'] = $matches[2];
				$hProcessStatistics['VmLib'] = str_replace(' kB', '', $hProcessStatistics['VmLib']);
				$hProcessStatistics['VmLib'] *= 1024;
			}
			if ( $matches[1] == 'VmPTE' ) {
				$hProcessStatistics['VmPTE'] = $matches[2];
				$hProcessStatistics['VmPTE'] = str_replace(' kB', '', $hProcessStatistics['VmPTE']);
				$hProcessStatistics['VmPTE'] *= 1024;
			}
			if ( $matches[1] == 'Threads' ) {
				$hProcessStatistics['Threads'] = $matches[2];
			}
			if ( $matches[1] == 'voluntary_ctxt_switches' ) {
				$hProcessStatistics['voluntary_ctxt_switches'] = $matches[2];
			}
			if ( $matches[1] == 'nonvoluntary_ctxt_switches' ) {
				$hProcessStatistics['nonvoluntary_ctxt_switches'] = $matches[2];
			}
		}
	}

	$hProcessStatistics['number_of_file_descriptors'] = getNumberOfFileDescriptors($pid);
	return array($rc, $hProcessStatistics);
}

// ---------------------------------------------------------------------
function processProcessInformation($pParameter)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '    ' . __FUNCTION__); }

	$hProcess['check'] = 0;

	if ( ( ! isset($pParameter['PidFile'])) || ($pParameter['PidFile'] == '') ) {
		$rc = 1475;
		mylog($pParameter['LogFile'], ERR, "    PID file is not defined or empty (rc=$rc).");
		return $rc;
	}

	if ( (! is_file($pParameter['PidFile'])) || (! is_readable($pParameter['PidFile'])) ) {
		$rc = 1476;
		mylog($pParameter['LogFile'], ERR, "    Cannot read PID file " . $pParameter['PidFile'] . " (rc=$rc).");
		mylog($pParameter['LogFile'], ERR, "    Either file does not exist or I have no read permissions.");
		mylog($pParameter['LogFile'], ERR, "    Are you sure the process is running?");
		mylog($pParameter['LogFile'], ERR, "    Adding the zabbix user to the mysql group might help...");
		return $rc;
	}

	$pid = 0;
	if ( $pParameter['Type'] == 'mysqld' ) {

		// SHOW GLOBAL VARIABLES LIKE 'pid_file'
		// The PID is the one of the mysqld (not mysqld_safe)!

		$pid = getMysqlPid($pParameter['PidFile']);
	}
	/*
	elseif ( $pParameter['Type'] == 'ndbd' ) {

		// It is located in the DataDir (config.ini)
		// The PID is the one of the ndbd angel process, NOT of the ndbd itself!

		$pid = getMysqlClusterPid($pParameter['PidFile']);
	}
	*/
	else {
		mylog($pParameter['LogFile'], WARN, "    Unknown section type " . $pParameter['Type']);
	}

	if ( $pid == 0 ) {
		$rc = 1477;
		mylog($pParameter['LogFile'], ERR, '    PID of ' . $pParameter['PidFile'] . " is $pid. Something went wrong (rc=$rc).");
		return $rc;
	}

	list($ret, $hProcess) = getStat($pParameter, $pid);

	if ( count($hProcess) == 0 ) {
		$rc = 1478;
		mylog($pParameter['LogFile'], ERR, "    Error getting process information (rc=$rc).");
		return $rc;
	}

	$aProcessToSend = array(
		'minor_faults'
	, 'children_minor_faults'
	, 'major_faults'
	, 'children_major_faults'
	, 'user_time'
	, 'children_user_time'
	, 'system_time'
	, 'children_system_time'
	, 'guest_time'
	, 'children_guest_time'
	, 'virtual_memory_size'
	, 'resident_memory_size'
	, 'scheduling_priortiy'
	, 'nice_value'
	, 'number_of_threads'
	, 'last_cpu_number'
	, 'rt_priority'
	, 'scheduling_policy'
	, 'block_io_delays'
	, 'Threads'
	, 'voluntary_ctxt_switches'
	, 'nonvoluntary_ctxt_switches'
	, 'number_of_file_descriptors'
	, 'FDSize'
	, 'VmPeak'
	, 'VmSize'
	, 'VmHWM'
	, 'VmRSS'
	, 'check'
);

	// Get memory for RSS/VZS graph

	$lOs = getOs();
	if ( $lOs == 'Linux' ) {
		list($rc, $aMemory) = getMemoryInformationLinux($pParameter);
		// print $rc, $aMemory;

		array_push($aProcessToSend, 'vm_memory_total_kb');
		$hProcess['vm_memory_total_kb'] = $aMemory['vm_memory_total_kb'];
	}
	else {
		$rc = 1479;
		mylog($pParameter['LogFile'], ERR, "    O/S $lOs is not supported yet (rc=$rc).");
		// no exit here
	}

	sendData($pParameter, $hProcess, $aProcessToSend);
	return $rc;
}

?>
