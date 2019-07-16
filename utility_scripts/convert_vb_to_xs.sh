#!/bin/bash

set -x #echo on

# # - see: https://github.com/jonludlam/vagrant-xenserver

cp -r  virtualbox xenserver
cd xenserver
qemu-img convert *.vmdk -O vpc box.vhd
rm -f Vagrantfile box.ovf metadata.json *.vmdk
echo "{\"provider\": \"xenserver\"}" > metadata.json
