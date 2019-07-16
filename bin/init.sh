#!/bin/bash

set +e #dont exit if any command fails
set -x #echo on

# More continuous scroll of the ansible standard output buffer
export PYTHONUNBUFFERED=1
export ANSIBLE_FORCE_COLOR=true
export ANSIBLE_KEEP_REMOTE_FILES=1
export ANSIBLE_CONFIG="/vagrant/provisioning/ansible/ansible.cfg"
export ara_location=$(python -c "import os,ara; print(os.path.dirname(ara.__file__))")
#
now=$(date +"%Y_%m_%d")
export ANSIBLE_LOG_PATH="/var/log/ansible-${now}.log"

#used by failure_log.py callback plugin
export ANSIBLE_FAILURE_LOG="/var/log/ansible-FAILURES-${now}.log"

set -e #exit if any command fails
set +x #echo off

#include common script
source "$(dirname "$0")/../functions.sh"

# echo "-------------setting proxy ----------------"
# HTTP_PROXY=http://192.168.160.1:3128
# export http_proxy=$HTTP_PROXY
# echo 

if [ -f /etc/debian_version ]; then
    # echo "This is debian based distro"
    ospackager_debian="debian"
elif [ -f /etc/redhat-release ]; then

    # echo "This is yum based distro"
    ospackager_rpm="rpm"
	
	if grep "CentOS" /etc/redhat-release; then
       osfamily_centos='CentOS'
       # echo "This is CentOS"
    else
       osfamily_redhat='RedHat'
       # echo "This is RedHat"
    fi
	
	# # cleanup any pending transactions
    # yum install -y yum-utils; yum-complete-transaction --cleanup-only
	
else
    # echo "This is something else"
    ospackager_other="other"
fi


#install ansible
if ! which ansible > /dev/null; then
	set -x #echo on
	
    if [ "$ospackager_debian" != "" ]; then
        export ospackager=debian
        
        #kill existing apt-get instances
        killall -9 apt-get | true
        # yum -y remove PackageKit | true
                
        # echo
        # echo "Updating APT sources."
        # echo
        # apt-get update 
        # #sumeet - good to see output > /dev/null
        # echo
        # echo "Installing for Ansible."
        # echo
        #essential packages
        apt-get update
        apt-get -y --no-install-recommends install software-properties-common python-software-properties python-setuptools python-dev cifs-utils make git build-essential g++ htop libssl-dev sshpass python-netaddr
        
        mkdir -p /var/downloads
        cd /var/downloads
       
        # if did not install from apt
        if ! which ansible > /dev/null; then
            easy_install pip
            pip install ansible
        fi

    elif [ "$ospackager_rpm" != ""  ]; then
        export ospackager=rpm
		
        #alter yum.conf to cache rpms
        sed -i "s/^keepcache=.*/keepcache=1/" /etc/yum.conf #| true
        sed -i "s/^metadata_expire=.*/metadata_expire=90d/" /etc/yum.conf  #| true

        if [ "$osfamily_redhat" != "" ]; then
            SUBS_STATUS=$(subscription-manager list | grep 'Status:         Subscribed')

            echo "Current Subscription Status= ${SUBS_STATUS}"

            if [ -z "${SUBS_STATUS}" ]; then
                # echo "subscription-manager registration"
                # NOTE:RHEL_USERNAME and RHEL_PASSWORD must be exported as environment variables in calling svript/terminal using "export RHEL_USERNAME='xyz'" and "export RHEL_PASSWORD='123'"
                subscription-manager register --username RHEL_USERNAME --password RHEL_PASSWORD --auto-attach --force
                #
                # echo "enable rhel-7-server-supplementary-rpms"
                subscription-manager repos --enable rhel-7-server-supplementary-rpms

                # echo "enable rhel-7-server-optional-rpms "
                subscription-manager repos --enable rhel-7-server-optional-rpms

                # echo "enable rhel-7-server-extras-rpms "
                subscription-manager repos --enable rhel-7-server-extras-rpms
            fi

        fi
        #kill existing yum instances
        killall -9 yum | true
        killall -9 PackageKit | true
        rm -rf /var/run/yum.pid  | true
        yum -y remove PackageKit | true
        
        yum install -y python-setuptools  python-devel gcc g++ make openssl-devel yum-utils python-passlib
		
        # echo  "install epel repo"
        # #get major version
        RELEASEVER=$(rpm -q --qf "%{VERSION}" $(rpm -q --whatprovides redhat-release) |cut -d'.' -f1 )
        
        if [ ! -f /etc/yum.repos.d/epel.repo ]; then
            # echo  "add epel repo, ignoring errors"
            rpm -ivh http://download.fedoraproject.org/pub/epel/epel-release-latest-$RELEASEVER.noarch.rpm
            # if [ "$RELEASEVER" != "7"  ]; then
               # rpm -ivh http://fedora.ip-connect.vn.ua/fedora-epel/6/$(arch)/epel-release-6-8.noarch.rpm
            # else
               # rpm -ivh http://fedora.ip-connect.vn.ua/fedora-epel/7/$(arch)/e/epel-release-7-5.noarch.rpm
            # fi
        else
            echo  "epel repo is already installed"
        fi
		
		yum install -y sshpass
        
        mkdir -p /var/downloads
        cd /var/downloads
        
        # if did not install from apt
        if ! which ansible > /dev/null; then
            easy_install pip
            pip install ansible
            pip install netaddr #for using jinja2 filter | ipaddr()
            pip install passlib #for using jinja2 filter | ipaddr()
        fi

    else
       echo " neither debian nor rhel based distro!"
    fi


fi

ansible_version=`dpkg -s ansible 2>&1 | grep Version | cut -f2 -d' '`
# echo "Ansible installed ($ansible_version)"

ANS_BIN=`which ansible-playbook`

if [[ -z $ANS_BIN ]]
    then
    echo "Whoops, can't find Ansible anywhere. Aborting run."
    # echo
    exit
fi

shopt -s nullglob

# cd to root ansible provisioning directory so cfg and hosts files are properly fetched

set +e #dont exit if any command fails - else we get kicked out of remote linux terminals - need to check exit status in scripts
set +x #echo off

# we presume this script was called with source from a nested subdir and ansible playbooks are one dir down
cd "$(dirname "$0")/../.."

################################
##guess vars from command line

# we presume actually calling script files in a nested subdirectory - if not, calling script will have to do an explicit cd

###defaults
PLAYBOOK="default.yml"
INVENTORY=""
SITE=""
CONFIG=""
APP=""
EXTRA_VARS=""
VERBOSITY="-vv"
PACKAGES=""
TAGS="all"
SKIP_TAGS=""
STRATEGY="linear"
DEFAULT=""
LIMIT="all"
SHOW_HELP=""
IS_INITIAL_RUN="yes"
SITE_SALT=""
RUN_COMMAND=""

##parse arguments
# http://stackoverflow.com/questions/192249/how-do-i-parse-command-line-arguments-in-bash
# Straight Bash Space Separated
# Usage  ./myscript.sh -e conf -s /etc -l /usr/lib /etc/hosts

# Use -gt 1 to consume two arguments per pass in the loop (e.g. each
# argument has a corresponding value to go with it).
# Use -gt 0 to consume one or more arguments per pass in the loop (e.g.
# some arguments don't have a corresponding value to go with it such
# as in the --default example).
# note: if this is set to -gt 0 the /etc/hosts part is not recognized ( may be a bug )
while [[ $# -gt 1 ]]
do
	key="$1"

	case $key in
		-p|--playbook)
		PLAYBOOK="$2"
		shift # past argument
		;;
		-ir|--initial-run)
		IS_INITIAL_RUN="$2"
		shift # past argument
		;;
		-r|--run)
		RUN_COMMAND="$2"
		shift # past argument
		;;
		-ss|--site-salt)
		SITE_SALT="$2"
		shift # past argument
		;;
		-i|--inventory)
		INVENTORY="$2"
		shift # past argument
		;;
		-l|--limit)
		LIMIT="$2"
		shift # past argument
		;;
		-s|--site)
		SITE="$2"
		shift # past argument
		;;
		-c|--config)
		CONFIG="$2"
		shift # past argument
		;;
		-a|--app)
		APP="$2"
		shift # past argument
		;;
		-e|--extra-vars)
		EXTRA_VARS="$2"
		shift # past argument
		;;
		-v|--verbosity)
		VERBOSITY="$2"
		shift # past argument
		;;
		-ip|--install-packages)
		PACKAGES="$2"
		shift # past argument
		;;
		-t|--tags)
		TAGS="$2"
		shift # past argument
		;;
		-st|--skip-tags)
		SKIP_TAGS="$2"
		shift # past argument
		;;
		-h|--help)
		SHOW_HELP="$2"
		shift # past argument
		;;
		*)
		# unknown option
		# ignore
		##SHOW_HELP="yes"
		shift # past argument
		;;
	esac

	shift # past argument or value
done

if [ "" != "${SHOW_HELP}" ]; then
	source ./show_syntax.sh;
	exit
fi

##########
if [[ "" = "${SKIP_TAGS}" ]]; then
	/bin/true
else
	SKIP_TAGS=",${SKIP_TAGS}"
fi

if [[ "" = "${CONFIG}" ]]; then
	CONFIG_VARS_FILE="config_vars/dummy.yml"
else
	CONFIG_VARS_FILE="config_vars/${CONFIG}/vars.yml"
fi

if [[ "" = "${APP}" ]]; then
	APP_VARS_FILE="app_vars/dummy.yml"
else
	APP_VARS_FILE="app_vars/${APP}/vars.yml"
fi

if [[ "" = "${SITE}" ]]; then
	SITE_VARS_FILE="site_vars/dummy.yml"
else
	SITE_VARS_FILE="site_vars/${SITE}/vars.yml"
fi


######
if [[ "" = "${INVENTORY}" ]]; then
    INVENTORY="h_${CONFIG}_${SITE}.txt"

    if [[ "h_.txt" = "${INVENTORY}" ]]; then
        INVENTORY="h_local.txt"
    fi;

    if [[ "h__local.txt" = "${INVENTORY}" ]]; then
        INVENTORY="h_local.txt"
    fi;

    if [ -f "$INVENTORY" ]
    then
        echo "Using Inventory File: $INVENTORY"
    else
        INVENTORY2="h_${CONFIG}.txt"
        echo "
        Inventory File: $INVENTORY does not exist
        Using Inventory INVENTORY: $INVENTORY2 instead
        "
        INVENTORY=$INVENTORY2
    fi
fi


#######################################
#source "$(dirname "$0")/../show_syntax.sh";
######################################
