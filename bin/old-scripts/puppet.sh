#!/bin/bash

set -x #echo on
set -e #exit if any cpommand fails

# echo "-------------setting proxy ----------------"
# HTTP_PROXY=http://192.168.160.1:3128
# export http_proxy=$HTTP_PROXY
# echo 

set -e
# LSB=`lsb_release -r | awk {'print $2'}`

if [ -f /etc/debian_version ]; then
    echo "This is debian based distro"
    ospackager_debian="debian"
elif [ -f /etc/redhat-release ]; then
    echo "This is yum based distro"
    ospackager_rpm="rpm"
    
    if grep "CentOS" /etc/redhat-release; then
       osfamily_centos='CentOS'
       echo "This is CentOS"
    else
       osfamily_redhat='RedHat'
       echo "This is RedHat"
    fi
    
else
    echo "This is something else"
    ospackager_other="other"
fi

#install puppet
if ! which puppet > /dev/null; then
##https://www.digitalocean.com/community/tutorials/how-to-install-puppet-in-standalone-mode-on-centos-7

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
        echo
        echo "Installing for puppet."
        echo
        #essential packages
        wget http://apt.puppetlabs.com/puppetlabs-release-trusty.deb 
        dpkg -i puppetlabs-release-trusty.deb | true
        apt-get update 
        apt-get install -y puppet
        
    elif [ "$ospackager_rpm" != ""  ]; then
        export ospackager=rpm
        
        #        
        rpm -ivh http://yum.puppetlabs.com/puppetlabs-release-el-7.noarch.rpm | true
        yes | yum -y install puppet
        puppet --version
        
    else
       echo " neither debian nor rhel based distro!"
    fi
    
  
    
fi

puppet_version=`dpkg -s puppet 2>&1 | grep Version | cut -f2 -d' '`
echo
echo "puppet installed ($puppet_version)"

PUP_BIN=`which puppet`

if [[ -z $PUP_BIN ]]
    then
    echo "Whoops, can't find puppet anywhere. Aborting run."
    echo
    exit
fi

# To see whether your node is correctly configured for Puppet, execute the following two commands:
facter | grep hostname
facter | grep fqdn

# the relevant output, which should look like this:
# hostname=mynode
# fqdn=mynode.example.com


#If the domain value in fqdn is incorrect, you can fix it by appending a single
#line to the system configuration file /etc/resolv.conf.
# cat >>/etc/resolv.conf
# domain example.com
# ^D

###
puppet module install puppetlabs-stdlib
puppet module install duritong-sysctl #for rhel
###

echo "
 :logger: console
" > /etc/puppet/hiera.yaml

mkdir -p /etc/puppet
mkdir -p /etc/puppet/manifests

#create test project and run it
touch /etc/puppet/manifests/test.pp
echo "
node default  {

file { '/root/example_file.txt':
    ensure => 'file',
    owner  => 'root',
    group  => 'root',
    mode   => '700',
    content => 'Congratulations! Puppet has created this file.
',}

} # End node mynode.example.com
" > /etc/puppet/manifests/test.pp

echo "
-------------------
"
puppet apply /etc/puppet/manifests/test.pp

###########################
#ansible/cis
###########################
##module is not uptodate - use git instead

cd /etc/puppet/modules
rm -rf cis-puppet
rm -rf cis
git clone https://github.com/arildjensen/cis-puppet.git
mv cis-puppet cis

##create run manifest
touch /etc/puppet/manifests/cis7.pp
echo "
node default {
  include cis::el7all
  
}
" > /etc/puppet/manifests/cis7.pp

echo "
-------------------
"
puppet apply  /etc/puppet/manifests/cis7.pp

#https://github.com/CSD-Public/stonix
#https://highon.coffee/blog/security-harden-centos-7/


