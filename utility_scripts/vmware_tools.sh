mkdir /mnt/cdrom

mount /dev/cdrom /mnt/cdrom

cp /mnt/cdrom/VMwareTools-*.tar.gz /tmp/
cd /tmp
tar -zxvf VMwareTools-*.tar.gz
cd vmware-tools-distrib
./vmware-install.pl


mkdir -p /mnt/shares
echo "
.host:// /mnt/shares vmhgfs defaults,ttl=5,uid=1000,gid=1000   0 0
" > /etc/fstab

mount -a

ls -al /mnt/shares