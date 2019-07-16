#!/bin/bash

#in vmware, select reinstall vmware tools

#Open a terminal again and login as the super user.
#Run this command and verify if the update is complete:
uname -rs

echo "answer AUTO_KMODS_ENABLED yes" | sudo tee -a /etc/vmware-tools/locations

# After mounting the ISO on `/dev/cdrom` with 'Reinstall VMware tools' menu item.
mkdir /mnt/cdrom
mount /dev/cdrom /mnt/cdrom
cd /tmp
tar zxpf /mnt/cdrom/VMwareTools-10.0.0-2977863.tar.gz
cd /tmp/vmware-tools-distrib

#remove previous install - else can fail!
rm -rf  /usr/lib/vmware-*
rm -rf  /var/lib/vmware-*
rm -rf /etc/vmware*

# Watch the output of this command very closely. No decent error detection :P.
./vmware-install.pl

#--default

#umount /dev/cdrom
#modprobe vmhgfs
