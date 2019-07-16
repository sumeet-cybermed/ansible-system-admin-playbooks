#!/bin/bash

set -e
set -x #echo on

remotehost="172.16.70.190"
remoteuser="backupuser"

# su backupuser
mkdir -p ~/.ssh
ssh-keygen -t rsa

ssh backupuser@172.16.70.190 mkdir -p .ssh

# copy public ssh key
cat ~/.ssh/id_rsa.pub | ssh backupuser@172.16.70.190 'cat >> .ssh/authorized_keys'

ssh backupuser@172.16.70.190 "chmod 700 .ssh; chmod 640 .ssh/authorized_keys"

#check login
ssh backupuser@172.16.70.190 "ls  .ssh/authorized_keys"

# create backup dir
ssh backupuser@172.16.70.190 'mkdir -p /var/backups/uploaded/172.16.70.189/rdiff'

