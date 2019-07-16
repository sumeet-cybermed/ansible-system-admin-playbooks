#!/bin/bash

#/vagrant/utility_scripts/vmware-hgfs-kernel-not-loading.sh

# for yum based
yum install -y gcc gcc-c++ make openssl-devel kernel-devel binutils kernel-headers deltarpm

# for apt based
# apt-get install -y build-essential linux-headers-$(uname -r)

#http://kb.vmware.com/selfservice/microsites/search.do?language=en_US&cmd=displayKC&externalId=2016514
yum -y update kernel
reboot

##then run install-wmware-tools-part2
