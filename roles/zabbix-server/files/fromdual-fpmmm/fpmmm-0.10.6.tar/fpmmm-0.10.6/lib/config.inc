<?php

define('RELEASE', '0.10.6');

$gLogLevel         = WARN;

$lHostname = gethostname();

// Seems to be not used anymore
// $gDataDir          = '/tmp';
$gLogDir           = '/var/log';

$gDatabase         = 'fpmmm';
$gDbUser           = 'fpmmm';
$gDbPassword       = 'fpmmm123';

if ( $lHostname == 'master' ) {
  $gDbHost           = '127.0.0.1';
  $gDbPort           = '35154';
}
else {
//   print $lHostname . "\n";
  $gDbHost           = 'localhost';
  $gDbSocket         = '/var/lib/mysql/mysql.sock';
  // $gDbPort           = '33006';
}

$gZabbixServer     = '127.0.0.1';
$gZabbixServerPort = '10051';

?>
