#!/bin/bash

#mkdir -p /root/utility_scripts/
#rm /root/utility_scripts/02-get_vagrant_scripts.sh
#nano /root/utility_scripts/02-get_vagrant_scripts.sh
#source /root/utility_scripts/02-get_vagrant_scripts.sh

set -x #echo on
set -e #exit if any cpommand fails

#
# yum -y install git nano


# configure git
mkdir -p /root/.git;
touch /root/.git/config;
git config --global core.autocrlf false;
git config --global core.safecrlf false;
git config --global color.ui true;
git config --global core.compression 7;
git config --global branch.autosetuprebase always;
git config --global http.postBuffer 524288000;
git config --system pack.threads 4;
git config --global credential.helper 'cache --timeout 720000';
git config --global credential.helper store;
git config credential.helper store | true;


##
mkdir -p /root/scripts
cd /root/scripts
rm -rf ./CMS-SystemAdmin


## outgoing ssh is blocked!
# git clone git@gitlab.com:CyberMed-Solutions/CMS-SystemAdmin.git
git clone https://gitlab.com/CyberMed-Solutions/CMS-SystemAdmin.git --depth 1

# cybermedsolutions
# 1AuroMira

###
rm -Rf /vagrant
mkdir -p /vagrant
/bin/cp -rf ./CMS-SystemAdmin/Vagrant/#vagrant_scripts/* /vagrant
chown -Rf root:root  /vagrant
chmod -x /vagrant/provisioning/ansible/*

#only way to recurse sub dirs with +x ia to us a pipe
cd /vagrant/provisioning/ansible/bin
find . -name '*.sh' -type f | xargs chmod +x

#only way to recurse sub dirs with +x ia to us a pipe
cd /vagrant/provisioning/ansible/utility_scripts
find . -name '*.sh' -type f | xargs chmod +x

##
# bash /vagrant/provisioning/ansible/bin/none.sh

