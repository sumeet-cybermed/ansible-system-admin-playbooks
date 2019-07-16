# mount from host
mkdir -p /mnt/soft
mkdir -p /mnt/temp
mkdir -p /mnt/sumeet
mkdir -p /mnt/cms-applications
mkdir -p /mnt/vagrant

mount -t nfs 10.10.10.1:/soft /mnt/soft
mount -t nfs 10.10.10.1:/temp /mnt/temp
mount -t nfs 10.10.10.1:/sumeet /mnt/sumeet
mount -t nfs 10.10.10.1:/cms-applications /mnt/cms-applications
mount -t nfs 10.10.10.1:/vagrant /mnt/vagrant