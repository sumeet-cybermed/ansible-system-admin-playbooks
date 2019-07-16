#!/bin/bash


echo "
	SYNTAX:
	${0} -c <value> -s <value>
	-s|--site <site_name> #  site_vars sub directory to load global additional SITE-SPECIFIC vars
	-c|--config <config_name> #  config_vars sub directory to load global additional CONFIG-SPECIFIC vars
	-i|--inventory <inventory_file_name.txt> # which inventory file to use
	-r|--run # adhoc command to run for adhoc scripts only
	-h|--help <blank | yes> # show this syntax help
	-p|--playbook <playbook_name.yml> # which playbook to run
	-ir|--initial-run <yes | no> # set var is_initial_run to true
	-ss|--site-salt <blank | hard_to_guess_site_salt> # allow unique passwords for site - must be correctly passed on command line to script on every subsequent invoke
	-l|--limit <blank to run for all | group_pattern | host_pattern> # limit run to host/group pattern
	-a|--app <blank | app_name > #  app_vars sub directory to load global additional APPLICATION-SPECIFIC vars
	-e|--extra-vars < 'var1=value1 var2=value2 '> # Extra vars to pass to run
	-v|--verbosity <v | vv | vvv> # verbosity for run
	-ip|--install-packages <yes | no> # run install packages first serially, then all other tags in parallel
	-t|--tags <blank | tag1,tag2> # run only these tags
	-sk|--skip-tags <blank | tag1,tag2> # skip these tags
	"
