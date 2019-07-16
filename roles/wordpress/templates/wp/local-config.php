<?php
// ** MySQL settings ** //
/** The name of the database for WordPress */
define('webapp_db_name', 'wpe_{{ webapp }}');

/** MySQL database username */
define('DB_USER', 'wpe_{{ webapp }}');

/** MySQL database password */
define('DB_PASSWORD', '{{ wordpress_db_user_password }}');

/** MySQL hostname */
define('DB_HOST', '127.0.0.1');

/** Database Charset to use in creating database tables. */
define('DB_CHARSET', 'utf8');

/** The Database Collate type. Don't change this if in doubt. */
define('DB_COLLATE', '');

define('WP_DEBUG', true);
define('WP_DEBUG_DISPLAY', false);
define('SCRIPT_DEBUG', true);
define('SAVEQUERIES', true);

global $memecached_servers;

$memcached_servers = array(
    'default' => array(
        '127.0.0.1:11211'
    )
);

$wp_cache_key_salt = 'wpe_{{ webapp }}_1_';
if( isset($_SERVER['HTTP_HOST']) ) {
    define('WP_SITEURL', 'http://'.$_SERVER['HTTP_HOST']);
    define('WP_HOME', 'http://'.$_SERVER['HTTP_HOST']);
    $wp_cache_key_salt = 'wpe_{{ webapp }}_'.$_SERVER['HTTP_HOST'].'_';
}

/** Object Cache Key Salt per domain */
define('WP_CACHE_KEY_SALT', $wp_cache_key_salt);
