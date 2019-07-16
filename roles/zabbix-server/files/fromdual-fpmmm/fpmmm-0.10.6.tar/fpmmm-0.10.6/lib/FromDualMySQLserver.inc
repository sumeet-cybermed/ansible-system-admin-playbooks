<?php

#
# Copyright (C) 2010-2016 FromDual GmbH
#


/*

	getNuma($pParameter, &$aCpuInfo)
	getVirtualization($pParameter, &$aCpuInfo)
	getNetworkInformationLinux($pParameter)
	getCpuInformationLinux($pParameter)
	getDiskSpaceInformationLinux($pParameter)
	getMemoryInformationLinux($pParameter)
	getDiskStatsLinux($pParameter)
	processServerInformation($pParameter, $pDomain)

*/

// ---------------------------------------------------------------------
function getNuma($pParameter, &$aCpuInfo)
// ---------------------------------------------------------------------
{
	$rc = OK;

	$aCpuInfo['numa'] = 'unknown';

	$c = '/usr/bin/numactl';
	if ( ! is_executable($c) ) {

		$rc = 1534;
		$msg = "Please install program $c.";
		mylog($pParameter['LogFile'], WARN, '    ' . "$msg (rc=$rc).");
		$rc = OK;
		return $rc;
	}

	$cmd = "$c --hardware";
	list($ret, $output, $stdout, $stderr) = my_exec($cmd);
	if ( $ret != OK ) {
		$rc = $ret;
		return $rc;
	}

	$aCpuInfo['numa'] = implode(' ', $stdout);

	return $rc;
}

// ---------------------------------------------------------------------
function getVirtualization($pParameter, &$aCpuInfo)
// ---------------------------------------------------------------------
{
	$rc = OK;

	$aCpuInfo['virtualization'] = 'unknown';

	// http://unix.stackexchange.com/questions/89714/easy-way-to-determine-virtualization-technology

	// crw-r----- 1 root kmem 1, 1 Jul 10 08:16 /dev/mem
	if ( ! is_readable('/dev/mem') ) {

		$rc = 1532;
		$msg = 'Cannot read /dev/mem. Please add user ' . getCurrentUser() . ' to group kmem.';
		mylog($pParameter['LogFile'], WARN, '    ' . "$msg (rc=$rc).");
		$rc = OK;
		return $rc;
	}

	$c = '/usr/sbin/dmidecode';
	if ( ! is_executable($c) ) {

		$rc = 1533;
		$msg = "Please install program $c.";
		mylog($pParameter['LogFile'], WARN, '    ' . "$msg (rc=$rc).");
		$rc = OK;
		return $rc;
	}

	$cmd = "$c -s system-product-name";
	list($ret, $output, $stdout, $stderr) = my_exec($cmd);
	if ( $ret != OK ) {
		$rc = $ret;
		return $rc;
	}

	$aCpuInfo['virtualization'] = $output;

	return $rc;
}

// ---------------------------------------------------------------------
function getNetworkInformationLinux($pParameter)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '      ' . __FUNCTION__); }

	$aNetworkInfo = array();

	// Inter-|   Receive                                                |  Transmit
	//  face |bytes    packets errs drop fifo frame compressed multicast|bytes    packets errs drop fifo colls carrier compressed
	//     lo: 2776770   11307    0    0    0     0          0         0  2776770   11307    0    0    0     0       0          0
	//   eth0: 1215645    2751    0    0    0     0          0         0  1782404    4324    0    0    0   427       0          0
	// or
	//     lo:138923130045 107515369    0    0    0     0          0         0 138923130045 107515369    0    0    0     0       0          0
	//   eth0:960408648343 3436113164  267  267    0   267          0    257958 6771695068 14193375    0    0    0     0       0          0


	$file = '/proc/net/dev';
	if ( ! $NET = fopen($file, 'r') ) {
		$rc = 1525;
		$a = error_get_last();
		mylog($pParameter['LogFile'], ERR, "    Cannot open file $file (rc=$rc).");
		mylog($pParameter['LogFile'], ERR, "    Reason: " . $a['message']);
		return array($rc, $aNetworkInfo);
	}

	$net = '';
	while ( $line = fgets($NET) ) {

		trim($line);

		$pattern = "/^\s*(\w+)\s*:\s*(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)$/";
		if ( preg_match($pattern, $line, $matches) ) {

			$aNetworkInfo['network[' . $matches[1] . ',bytes_rx]'] = $matches[2];
			$aNetworkInfo['network[' . $matches[1] . ',pckts_rx]'] = $matches[3];
			$aNetworkInfo['network[' . $matches[1] . ',err_rx]'] = $matches[4];
			$aNetworkInfo['network[' . $matches[1] . ',drop_rx]'] = $matches[5];
			$aNetworkInfo['network[' . $matches[1] . ',bytes_tx]'] = $matches[10];
			$aNetworkInfo['network[' . $matches[1] . ',pckts_tx]'] = $matches[11];
			$aNetworkInfo['network[' . $matches[1] . ',err_tx]'] = $matches[12];
			$aNetworkInfo['network[' . $matches[1] . ',drop_tx]'] = $matches[13];
		}
	}
	fclose($NET);

	return array($rc, $aNetworkInfo);
}

// ---------------------------------------------------------------------
function getCpuInformationLinux($pParameter)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '      ' . __FUNCTION__); }

	$aCpuInfo = array();

	// Which devices we have data from
	$aCpuInfo['cpu.cpus']  = '';
	$aCpuInfo['cpu.count'] = 0;

	// in USER_HZ (0.01 s)
	// user mode
	// nice mode
	// system
	// idle
	// iowait
	// irq
	// softirq
	// steal
	// guest

	// This is probably wrong! We have to take 2 snapshots?

	$file = '/proc/stat';
	if ( ! $CPU = fopen($file, 'r') ) {
		$rc = 1526;
		$a = error_get_last();
		mylog($pParameter['LogFile'], ERR, "   Cannot open file $file (rc=$rc).");
		mylog($pParameter['LogFile'], ERR, "   Reason: " . $a['message']);
		return array($rc, $aCpuInfo);
	}
	$cpu = '';
	while ( $line = fgets($CPU) ) {

		trim($line);

		$pattern = "/^cpu(\d*)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s*(\d+)?\s*(\d+)?\s*(\d+)?\s*(\d+)?\s*(\d+)?\s*(\d+)?$/";
		if ( preg_match($pattern, $line, $matches) ) {

			if ( $matches[1] == '' ) {
				$cpu = 'total';
			}
			else {
				$cpu = $matches[1];
				$aCpuInfo['cpu.count']++;
				if ( $aCpuInfo['cpu.cpus'] == '' ) {
					$aCpuInfo['cpu.cpus'] .= $cpu;
				}
				else {
					$aCpuInfo['cpu.cpus'] .= ' ' . $cpu;
				}
			}
			$v2  = isset($matches[2]) ? $matches[2] : 0;
			$v3  = isset($matches[3]) ? $matches[3] : 0;
			$v4  = isset($matches[4]) ? $matches[4] : 0;
			$v5  = isset($matches[5]) ? $matches[5] : 0;
			$v6  = isset($matches[6]) ? $matches[6] : 0;
			$v7  = isset($matches[7]) ? $matches[7] : 0;
			$v8  = isset($matches[8]) ? $matches[8] : 0;
			$v9  = isset($matches[9]) ? $matches[9] : 0;
			$v10 = isset($matches[10]) ? $matches[10] : 0;
//       $v11 = isset($matches[1]1) ? $matches[1]1 : 0;

			$aCpuInfo['cpu[' . $cpu . ',user]']   = $v2;
			$aCpuInfo['cpu[' . $cpu . ',nice]']   = $v3;
			$aCpuInfo['cpu[' . $cpu . ',system]'] = $v4;
			$aCpuInfo['cpu[' . $cpu . ',idle]']   = $v5;
			$aCpuInfo['cpu[' . $cpu . ',iowait]'] = $v6;
			$aCpuInfo['cpu[' . $cpu . ',irq]']    = $v7;
			$aCpuInfo['cpu[' . $cpu . ',soft]']   = $v8;
			$aCpuInfo['cpu[' . $cpu . ',steal]']  = $v9;
			$aCpuInfo['cpu[' . $cpu . ',guest]']  = $v10;
//       $aCpuInfo['cpu[' . $cpu . ',unknown]'] = $v11;
		}
		elseif ( preg_match("/^ctxt\s*(\d*)/", $line, $matches) ) {
			$aCpuInfo['cpu_ctxt'] = $matches[1];
		}
		elseif ( preg_match("/^processes\s*(\d*)/", $line, $matches) ) {
			$aCpuInfo['cpu_proc'] = $matches[1];
		}
		elseif ( preg_match("/^intr\s*(\d*)/", $line, $matches) ) {
			$aCpuInfo['cpu_intr'] = $matches[1];
		}
		elseif ( preg_match("/^procs_running\s*(\d*)/", $line, $matches) ) {
			$aCpuInfo['cpu_proc_r'] = $matches[1];
		}
		elseif ( preg_match("/^procs_blocked\s*(\d*)/", $line, $matches) ) {
			$aCpuInfo['cpu_proc_b'] = $matches[1];
		}
	}
	fclose($CPU);

	$aCpuInfo['virtualization'] = '';
	$ret = getVirtualization($pParameter, $aCpuInfo);

	$aCpuInfo['numa'] = '';
	$ret = getNuma($pParameter, $aCpuInfo);

	// Only every nth value is sent, this is to simulat frequency to safe space
	if ( (rand() % 360) != 0 ) {
		unset($aCpuInfo['cpu.cpus']);
		unset($aCpuInfo['cpu.count']);
		unset($aCpuInfo['virtualization']);
		unset($aCpuInfo['numa']);
	}

	return array($rc, $aCpuInfo);
}

// ---------------------------------------------------------------------
function getDiskSpaceInformationLinux($pParameter)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '      ' . __FUNCTION__); }

	// todo: higly proprietary...
	$cmd = 'df -k';
	list($ret, $output, $aLines, $stderr) = my_exec($cmd);

	$aDiskSpace  = array();

	for ( $i = 0; $i < count($aLines); $i++ ) {

		$aLines[$i] = trim($aLines[$i]);

		//   Filesystem           1K-blocks      Used Available Use$ Mounted on
		//   none                   8209112       248   8208864   1$ /dev
		//   none                   8215980      4164   8211816   1$ /dev/shm
		//   none                   8215980       224   8215756   1$ /var/run
		//   none                   8215980         0   8215980   0$ /var/lock
		if ( preg_match("/^none\s+\d+/", $aLines[$i], $matches) ) {
			continue;
		}

		//   Filesystem           1K-blocks      Used Available Use$ Mounted on
		if ( preg_match("/^Filesystem\s+/", $aLines[$i], $matches) ) {
			continue;
		}

		// removed, see belov
		if ( preg_match("/^removed$/", $aLines[$i], $matches) ) {
			continue;
		}

		// Concatenate lines
		// /dev/mapper/vg01-root
		//                      121888740  63151980  52445560  55$ /
		// fas2040a:/vol/sv_os/sv_s35_os_root
		//                      5368709120 1601446272 3767262848  30$ /mnt/ossv_backup
		// mgt2:/nfs_exports/s35_home_lun
		//                      123052544  68847936  47852896  59$ /cyon_backup/home-backup

		if ( preg_match("/^\S+$/", $aLines[$i], $matches) ) {

			$aLines[$i] .= $aLines[$i+1];
			$aLines[$i] = trim($aLines[$i]);
			$aLines[$i+1] = 'removed';
		}

		//   Filesystem           1K-blocks      Used Available Use$ Mounted on
		//   /dev/sdb1            461229088 216425820 221374200  50$ /
		//   /dev/sda1             62486392  45949340  16537052  74$ /home/mysql/data

		$pattern = "/(\S+)\s+(\d+)\s+(\d+)\s+(\d+)\s+\d+%\s+(.*)$/";
		if ( preg_match($pattern, $aLines[$i], $matches) ) {

			$aDiskSpace['disk_space[' . $matches[1] . ',total]'] = $matches[2];
			$aDiskSpace['disk_space[' . $matches[1] . ',used]']  = $matches[3];
			$aDiskSpace['disk_space[' . $matches[1] . ',pused]']  = ($matches[3] / $matches[2] * 100);
			$aDiskSpace['disk_space[' . $matches[1] . ',free]']  = $matches[4];
			$aDiskSpace['disk_space[' . $matches[1] . ',pfree]']  = ($matches[4] / $matches[2] * 100);

			$aDiskSpace['disk_space[' . $matches[5] . ',total]'] = $matches[2];
			$aDiskSpace['disk_space[' . $matches[5] . ',used]']  = $matches[3];
			$aDiskSpace['disk_space[' . $matches[5] . ',pused]']  = ($matches[3] / $matches[2] * 100);
			$aDiskSpace['disk_space[' . $matches[5] . ',free]']  = $matches[4];
			$aDiskSpace['disk_space[' . $matches[5] . ',pfree]']  = ($matches[4] / $matches[2] * 100);
			continue;
		}

		// Show all the rest we could not parse
		if ( $pParameter['LogLevel'] >= ERR ) { mylog($pParameter['LogFile'], ERR, '      ' . $aLines[$i]); }
		$rc = 1527;
	}

	return array($rc, $aDiskSpace);
}

// ---------------------------------------------------------------------
function getMemoryInformationLinux($pParameter)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '      ' . __FUNCTION__); }
	$aMemory = array();

	$file = '/proc/meminfo';
	if ( ! is_readable($file) ) {
		$rc = 1528;
		if ( $pParameter['LogLevel'] >= ERR ) { mylog($pParameter['LogFile'], INFO, '    ' . "Cannot read $file (rc=$rc)."); }
		return array($rc, $aMemory);

		// We could try to read free instead
		// free
		//              total       used       free     shared    buffers     cached
		// Mem:      16431968    7775540    8656428          0     392500    2607192
		// -/+ buffers/cache:    4775848   11656120
		// Swap:     19802108          0   19802108
	}

	// man proc
	// http://unixfoo.blogspot.com/2008/02/know-about-procmeminfo.html
	// vmstat -s
	//
	// MemTotal:       16431968 kB *
	// MemFree:         8655484 kB *
	// Buffers:          392512 kB *
	// Cached:          2607312 kB *
	// SwapTotal:      19802108 kB *
	// SwapFree:       19802108 kB *
	$aMemory['vm_memory_total_kb'] = 0;
	$aMemory['vm_memory_free_kb'] = 0;
	$aMemory['vm_buffers_kb'] = 0;
	$aMemory['vm_cached_kb'] = 0;
	$aMemory['vm_swap_total_kb'] = 0;
	$aMemory['vm_swap_free_kb'] = 0;
//   $aMemory['vm_swap_used_kb'] = 0;
//   $aMemory['vm_used_free_kb'] = 0;

	if ( ! $FILE = fopen($file, 'r') ) {
		$rc = 1529;
		$a = error_get_last();
		mylog($pParameter['LogFile'], ERR, "   Cannot open file $file (rc=$rc).");
		mylog($pParameter['LogFile'], ERR, "   Reason: " . $a['message']);
		return array($rc, $aMemory);
	}

	while ( $line = fgets($FILE) ) {

		trim($line);

		if ( preg_match("/^MemTotal:\s+(\d+)\s+kB/", $line, $matches) ) {
			$aMemory['vm_memory_total_kb'] = $matches[1];
		}
		elseif ( preg_match("/^MemFree:\s+(\d+)\s+kB/", $line, $matches) ) {
			$aMemory['vm_memory_free_kb'] = $matches[1];
		}
		elseif ( preg_match("/^Buffers:\s+(\d+)\s+kB/", $line, $matches) ) {
			$aMemory['vm_memory_buffers_kb'] = $matches[1];
		}
		elseif ( preg_match("/^Cached:\s+(\d+)\s+kB/", $line, $matches) ) {
			$aMemory['vm_memory_cached_kb'] = $matches[1];
		}
		elseif ( preg_match("/^SwapTotal:\s+(\d+)\s+kB/", $line, $matches) ) {
			$aMemory['vm_swap_total_kb'] = $matches[1];
		}
		elseif ( preg_match("/^SwapFree:\s+(\d+)\s+kB/", $line, $matches) ) {
			$aMemory['vm_swap_free_kb'] = $matches[1];
		}
	}
	fclose($FILE);

	// Do some calculations
	$aMemory['vm_swap_used_kb'] = $aMemory['vm_swap_total_kb'] - $aMemory['vm_swap_free_kb'];
	$aMemory['vm_memory_used_kb'] = $aMemory['vm_memory_total_kb'] - $aMemory['vm_memory_cached_kb'] - $aMemory['vm_memory_buffers_kb'] - $aMemory['vm_memory_free_kb'];

	return array($rc, $aMemory);
}

// ---------------------------------------------------------------------
function getDiskStatsLinux($pParameter)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '      ' . __FUNCTION__); }

	$aDiskStats = array();

	$file = '/proc/diskstats';
	if ( ! $STAT = fopen($file, 'r') ) {
		$rc = 1530;
		$a = error_get_last();
		mylog($pParameter['LogFile'], ERR, "   Cannot open file $file (rc=$rc).");
		mylog($pParameter['LogFile'], ERR, "  Reason: " . $a['message']);
		return array($rc, $aDiskStats);
	}

	// Which devices we have data from
	$aDiskStats['disk.devices'] = '';

	while ( $line = fgets($STAT) ) {

		trim($line);

		// 8       0 sda  131176 1216 5939494 215496 241947 164663 346315536 2757696 0 383088 2973200
		$pattern = "/^\s+\d+\s+\d+\s+(\w+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)/";

		if ( preg_match($pattern, $line, $matches) ) {

			// skip ram and loop
			if ( preg_match("/^ram/", $matches[1])
				|| preg_match("/^loop/", $matches[1])
				|| preg_match("/^sr/", $matches[1])
				) {
				continue;
			}

			if ( $aDiskStats['disk.devices'] == '' ) {
				$aDiskStats['disk.devices'] .= $matches[1];
			}
			else {
				$aDiskStats['disk.devices'] .= ' ' . $matches[1];
			}

			// rd_ios
			$aDiskStats['disk.reads_completed[' . $matches[1] . ']'] = $matches[2];
			// rd_merges
			$aDiskStats['disk.reads_merged[' . $matches[1] . ']'] = $matches[3];
			// rd_sectors
			$aDiskStats['disk.sectors_read[' . $matches[1] . ']'] = $matches[4];
			// rd_ticks_or_wr_sec or rd_ticks
			$aDiskStats['disk.ms_spent_reading[' . $matches[1] . ']'] = $matches[5];
			// wr_ios
			$aDiskStats['disk.writes_completed[' . $matches[1] . ']'] = $matches[6];
			// wr_merges
			$aDiskStats['disk.writes_merged[' . $matches[1] . ']'] = $matches[7];
			// wr_sectors
			$aDiskStats['disk.sectors_written[' . $matches[1] . ']'] = $matches[8];
			// wr_ticks
			$aDiskStats['disk.ms_spent_writing[' . $matches[1] . ']'] = $matches[9];
			$aDiskStats['disk.io_in_progress[' . $matches[1] . ']'] = $matches[10];
			$aDiskStats['disk.ms_spent_doing_io[' . $matches[1] . ']'] = $matches[11];

			// r_await = (ioi->rd_ios - ioj->rd_ios) ? (ioi->rd_ticks - ioj->rd_ticks) / ((double) (ioi->rd_ios - ioj->rd_ios)) : 0.0;
			// w_await = (ioi->wr_ios - ioj->wr_ios) ? (ioi->wr_ticks - ioj->wr_ticks) / ((double) (ioi->wr_ios - ioj->wr_ios)) : 0.0;


			// This field is incremented at each I/O start, I/O completion, I/O
			// merge, or read of these stats by the number of I/Os in progress
			// (field 9) times the number of milliseconds spent doing I/O since the
			// last update of this field.  This can provide an easy measure of both
			// I/O completion time and the backlog that may be accumulating.
			// rq_ticks
			$aDiskStats['disk.wighted_nbr_ms_spent_doing_io[' . $matches[1] . ']'] = $matches[12];
		}
	}
	fclose($STAT);

	// Only every nth value is sent, this is to simulat frequency to safe space
	if ( (rand() % 360) != 0 ) {
		unset($aDiskStats['disk.devices']);
	}

	return array($rc, $aDiskStats);
}

// ---------------------------------------------------------------------
function processServerInformation($pParameter, $pDomain)
// ---------------------------------------------------------------------
{
	$rc = OK;

	if ( $pParameter['LogLevel'] >= INFO ) { mylog($pParameter['LogFile'], INFO, '    ' . __FUNCTION__); }

	$hServerStatus = array();

	$aServerStatusToSend = array(
	);

	initValues($hServerStatus, $aServerStatusToSend);

	$lOs = getOs();
	if ( $lOs == 'Linux' ) {

		// ----
		// Disk statistics
		// ----
		list($rc, $aDiskStats) = getDiskStatsLinux($pParameter);
		foreach ( $aDiskStats as $key => $value ) {
			array_push($aServerStatusToSend, $key);
			$hServerStatus[$key] = $value;
		}

		// ----
		// Get Virtual Memory information
		// ----
		list($rc, $aMemory) = getMemoryInformationLinux($pParameter);
		// print $rc, $aMemory;

		foreach ( $aMemory as $key => $value ) {
			array_push($aServerStatusToSend, $key);
			$hServerStatus[$key] = $value;
		}

		// ----
		// Get Disk Space information
		// ----
		list($rc, $aDiskSpace) = getDiskSpaceInformationLinux($pParameter);
		// print Dumper($rc, $aDiskSpace);
		foreach ( $aDiskSpace as $key => $value ) {
			array_push($aServerStatusToSend, $key);
			$hServerStatus[$key] = $value;
		}

		// ----
		// CPU Information
		// ----
		list($rc, $aCpuInfo1) = getCpuInformationLinux($pParameter);
		sleep(1);
		list($rc, $aCpuInfo2) = getCpuInformationLinux($pParameter);

		// Build sum to calculate $
		foreach ( array_keys($aCpuInfo2) as $key ) {

			// Filter out all unless cpu[...]
			if ( preg_match("/cpu\[(.*),(.*)\]/", $key, $matches) ) {
				if ( ! isset($aCpuInfo2["cpu[$matches[1],total]"] ) ) {
					$aCpuInfo2["cpu[$matches[1],total]"] = 0.0;
				}
				// Sum of delta of now and one second ago
				$aCpuInfo2["cpu[$matches[1],total]"] += ($aCpuInfo2[$key] - $aCpuInfo1[$key]);
			}
		}

		// Calculate $
		foreach ( array_keys($aCpuInfo2) as $key ) {

			if ( preg_match("/cpu\[(.*),(.*)]/", $key, $matches)  ) {
				if ( $matches[2] != 'total' ) {
					array_push($aServerStatusToSend, $key);
					if ( $aCpuInfo2['cpu[' . $matches[1] . ',total]'] != 0 ) {
						$hServerStatus[$key] = round(($aCpuInfo2[$key] - $aCpuInfo1[$key]) / $aCpuInfo2['cpu[' . $matches[1] . ',total]'] * 100, 2);
					}
					else {
						$aCpuInfo2['cpu[' . $matches[1] . ',total]'] = 0;
					}
				}
			}
			else {
				array_push($aServerStatusToSend, $key);
				$hServerStatus[$key] = $aCpuInfo2[$key];
			}
		}

		// ----
		// Network Information
		// ----
		list($rc, $aNetworkInfo) = getNetworkInformationLinux($pParameter);
		foreach ( $aNetworkInfo as $key => $value ) {
			array_push($aServerStatusToSend, $key);
			$hServerStatus[$key] = $value;
		}
	}
	// Other operating systems than Linux
	else {
		$rc = 1531;
		if ( $pParameter['LogLevel'] >= WARN ) { mylog($pParameter['LogFile'], WARN, '    ' . "Other operating systems than Linux are currently NOT supported (rc=$rc)."); }
	}

	// Do some post calculations

	// $hServerStatus['xxx'] = $hServerStatus['xxx'] == 0 ? 0 : $hServerStatus['xxx'] / $hServerStatus['xxx'];

	// NO domain in this case!
	// Ugly hack
	$domain = $pDomain;
	$pDomain = '';
	sendData($pParameter, $hServerStatus, $aServerStatusToSend);
	$pDomain = $domain;
	return $rc;
}

?>
