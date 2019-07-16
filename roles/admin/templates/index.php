<!doctype html>
<html>
<head>
    <meta charset='utf-8'>
    <meta http-equiv="X-UA-Compatible" content="chrome=1" />
    <meta name="viewport" content="width=device-width">

    <title>Admin - WP Engine Mercury Vagrant</title>
</head>
<body>

    <h1>Administrative Area</h1>
    <h3>WP Engine Mercury Vagrant</h3>

    <p>
        This domain contains developer tools outside of WordPress itself.
    </p>

    <ul>
        <li><a href="https://ajenti.{{external_ip_address}}/ajenti">ajenti</a> - web UI for managing server</li>
        <li><a href="http://munin.{{external_ip_address}}">munin</a> - web UI for monitoring server</li>
        <li><a href="http://admin.{{external_ip_address}}/phpmyadmin">phpmyadmin</a> - web UI for accessing the MySQL database</li>
        <li><a href="http://admin.{{external_ip_address}}/phpmemcachedadmin">Memcached Admin</a> - web UI for viewing the current status of the Memcached daemon</li>
        <li><a href="http://admin.{{external_ip_address}}/redisadmin">Redis Admin</a> - web UI for viewing the current status of the Redis daemon</li>
        <li><a href="http://admin.{{external_ip_address}}/redisadmin/redis-basic-stats.php">Redis Basic Stats</a> - web UI for viewing the current hit rate of the Redis daemon</li>
        <li><a href="http://admin.{{external_ip_address}}:63790/">Redis Advanced Stats</a> - start the stats app using redis-stat --server</li>
        <li><a href="http://admin.{{external_ip_address}}/logs">PML</a> - web log viewer for examining web server access and error logs</li>
    </ul>

</body>
</html>
