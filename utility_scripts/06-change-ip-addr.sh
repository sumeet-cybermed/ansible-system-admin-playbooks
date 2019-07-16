#!/bin/bash

set -e
set -x

sed -i "s/\(.*\)$1\(.*\)/\1$2\2/g" /etc/sysconfig/network-scripts/ifcfg-eth0;cat /etc/sysconfig/network-scripts/ifcfg-eth0
sed -i "s/\(.*\)$1\(.*\)/\1$2\2/g" /etc/hostname;cat /etc/hostname

#repeat few times to ensure all occurrences in line are replaced - not sure if /g works
sed -i "s/\(.*\)$1\(.*\)/\1$2\2/g" /etc/hosts;cat /etc/hosts
sed -i "s/\(.*\)$1\(.*\)/\1$2\2/g" /etc/hosts;cat /etc/hosts
sed -i "s/\(.*\)$1\(.*\)/\1$2\2/g" /etc/hosts;cat /etc/hosts
