#!/bin/bash

# su
# sudo yum install -y nano cifs-utils samba-client
# mkdir -p ~/utility_scripts
# nano ~/utility_scripts/mount_all_host_folders.sh
# chmod +x ~/utility_scripts/mount_all_host_folders.sh
# ~/utility_scripts/mount_all_host_folders.sh
# smbclient -L 59.99.176.128 --user cifs_server 
# 1.cms.skwin7x64

set -e
set -x #echo on

#source ../bin/init.sh

function mountfolder() {

    remote_host=$1 
    remote_folder=$2 
    local_folder=$3 
    copy_to_local_folder=$4 

    #
    mount_command="mount -t cifs -o credentials=/root/cifs_server_password.txt,file_mode=0777,dir_mode=0777,gid=501,uid=501,rw,sec=ntlm,nounix,noperm  $remote_host$remote_folder $local_folder"

    echo $mount_command

    #mount
    mkdir -p $local_folder;
    $mount_command

    #copy
    if [ "$copy_to_local_folder" != "" ]; then
        mkdir -p $copy_to_local_folder;
        /bin/cp -rf $local_folder/* $copy_to_local_folder/
    fi    
}

function mountallfolders() {

    mount_type=$2
       
    echo "username=cifs_server
password=1.cms.skwin7x64
" > /root/cifs_server_password.txt;

    chown root:root /root/cifs_server_password.txt;

    #
    mountfolder $1 "vagrant" "/vagrant" "/vagrant2"
    
    if [ "$mount_type" != "scripts" ]; then
        mountfolder $1 ".vagrant.d/boxes" "~/.vagrant.d/boxes/"
        mountfolder $1 ".vagrant.d/_cache" "~/.vagrant.d/_cache/"
        mountfolder $1 ".vagrant.d/downloads" "~/.vagrant.d/downloads/"

        #system folders
        mountfolder $1 ".vagrant.d/downloads" "/var/downloads/"
        mountfolder $1 ".vagrant.d/_cache/apt/archives" "/var/cache/apt/archives/"
        mountfolder $1 ".vagrant.d/_cache/yum" "/var/cache/yum/"
        mountfolder $1 ".vagrant.d/_cache/yum/x86_64/7" "/var/cache/yum/x86_64/7Server"
    fi    
}

#for local vms
#mountallfolders "//192.168.153.1/" "all"

#for bl openvpn cms
mountallfolders "//59.99.176.128/" "scripts"

#for rsdh vpn
mountallfolders "//172.16.72.73/" "scripts"

