# http://software.opensuse.org/download?package=webcit&project=home:homueller:citadel

#citadel
cd /etc/yum.repos.d/
wget http://download.opensuse.org/repositories/home:homueller:citadel/CentOS_7/home:homueller:citadel.repo
yum install citadel

#webcit
cd /etc/yum.repos.d/
wget http://download.opensuse.org/repositories/home:homueller:citadel/CentOS_7/home:homueller:citadel.repo
yum install webcit

# http://www.citadel.org/doku.php/installation:easyinstall:start
#easy install
curl http://easyinstall.citadel.org/install | sh

#setup
/usr/sbin/citadel-setup 

# SysVinit and systemd

# The older RPM based distributions, like SLES and CENTOS, use sysvinit as init system. With these distributions the citadel and webcit daemons will be started through the scripts /etc/init.d/citadel and /etc/init.d/webcit. So, you would use

/etc/init.d/citadel start

# to start the citserver daemon.

# The newer distributions, like openSUSE, fedora and Mandriva, use systemd as init system. So, you can would use

systemctl start citadel.service

# to start the citserver daemon. The FAQ article about systemd describes how to use systemd.

# The service command will work in either case, e.g.:

service citadel start

