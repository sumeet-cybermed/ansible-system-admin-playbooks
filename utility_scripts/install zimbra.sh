# http://www.unixmen.com/install-zimbra-collaboration-suite-8-6-0-centos-7/
# 1. Configure /etc/hosts and hostname

# 2. Allow iptables to by-pass all zimbra ports.

# 3.  Disabled SELINUX

nano /etc/sysconfig/selinux

# Change enforcing to disabled :

# This file controls the state of SELinux on the system.
# SELINUX= can take one of these three values:
#     enforcing - SELinux security policy is enforced.
#     permissive - SELinux prints warnings instead of enforcing.
#     disabled - No SELinux policy is loaded.
SELINUX=disabled
# SELINUXTYPE= can take one of these two values:
#     targeted - Targeted processes are protected,
#     minimum - Modification of targeted policy. Only selected processes are protected.
#     mls - Multi Level Security protection.
SELINUXTYPE=targeted

# Stop any MTA services installed in the server
systemctl stop postfix
systemctl disable postfix
systemctl stop sendmail
systemctl disable sendmail

# 5.Update the OS

yum update -y

# 6.Install the required packages and libraries by issuing the following command :

yum install perl perl-core ntpl nmap sudo libidn gmp libaio libstdc++ unzip sysstat sqlite -y

# Now, the server is ready for install Zimbra 8.6.0.
# Download Zimbra Open Source Edition 8.6.0

# Issue the following command to download ZCS 8.6.0

cd /tmp
mkdir -p zimbra
cd zimbra

wget https://files.zimbra.com/downloads/8.6.0_GA/zcs-8.6.0_GA_1153.RHEL7_64.20141215151110.tgz

# Extract the downloaded tar file :

# Using the following command you can extract the tar file, We downloaded in previous step

tar xzf zcs-8.6.0_GA_1153.RHEL7_64.20141215151110.tgz

# Go to extracted ZCS Open Source Edition :

cd zcs-8.6.0_GA_1153.RHEL7_64.20141215151110.tgz

# Start the installation with the following command :

# Now, we are going to install the ZCS package using the script

./install.sh --platform-override

# Start zimbra services :

su - zimbra
zmcontrol start

# Access admin panel via browser :

https://<your_zimbra_domain.com>:7071

#http://serverfault.com/questions/448340/nginx-rewrite-rule-for-zimbra
server {
    server_name mail.hostname;
    location / {
        proxy_redirect                   off;
        proxy_set_header Host            $host;
        proxy_set_header X-Real-IP       $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_pass                       http://127.0.0.1:7071/;
        if ($args !~* at=long\-encrypted\-user\-id) {
            rewrite ^/(.*)$ /$1?at=long-encrypted-user-id last;
        }
    }
}

# https://forums.zextras.com/zimbra-howto/485-setting-up-dnsmasq-instead-bind-bulletproof-internal-dns-resolution-splitdns.html

# install dnsmasq
yum install -y dnsmasq bind-utils NetworkManager-tui

#set fqdn
nano /etc/hostname

test.cxlm.ramsaysimedarbyhealth.com

nano /etc/sysconfig/network

HOSTNAME=test.cxlm.ramsaysimedarbyhealth.com

#test hostname -f
nano /etc/default/dhcpcd

#SET_HOSTNAME='yes'


# configure hosts
nano /etc/hosts
172.16.70.189 test.cxlm.ramsaysimedarbyhealth.com mail.test.cxlm.ramsaysimedarbyhealth.com 
172.16.70.189 test.cxlm.rsdh.com mail.test.cxlm.rsdh.com

#must also set this to avoid resolve conf being overwritten
nano /etc/sysconfig/network-scripts/ifcfg-ens160

DNS1="172.16.70.189"
DNS2="172.16.70.225"

#
nano /etc/sysconfig/network

test.cxlm.ramsaysimedarbyhealth.com

# resolve
nano /etc/resolv.conf
#
search test.cxlm.ramsaysimedarbyhealth.com
nameserver 172.16.70.189

#resolv.dnsmasq
nano /etc/resolv.dnsmasq

nameserver 172.16.70.225
nameserver 8.8.8.8

# dnsmasq.conf
nano /etc/dnsmasq.conf

address=/test.cxlm.ramsaysimedarbyhealth.com/172.16.70.189
address=/myhcsel990134s.simedarbygroup.com/172.16.70.189
address=/mail.test.cxlm.ramsaysimedarbyhealth.com/172.16.70.189
address=/test.cxlm.rsdh.com/172.16.70.189
resolv-file=/etc/resolv.dnsmasq
except-interface=lo
listen-address=127.0.0.1,172.16.70.189
bind-interfaces
domain=cxlm.ramsaysimedarbyhealth.com
mx-host=test.cxlm.ramsaysimedarbyhealth.com,mail.test.cxlm.ramsaysimedarbyhealth.com,10
# mx-host=test.cxlm.ramsaysimedarbyhealth.com,test.cxlm.ramsaysimedarbyhealth.com,10
# mx-host=myhcsel990134s.simedarbygroup.com,myhcsel990134s.simedarbygroup.com,10
# mx-host=test.cxlm.rsdh.com,test.cxlm.rsdh.com,10
#mx-host=test.cxlm.rsdh.com,mail.test.cxlm.rsdh.com,10
server=/test.cxlm.ramsaysimedarbyhealth.com/172.16.70.189
server=/mail.test.cxlm.ramsaysimedarbyhealth.com/172.16.70.189

# restart
service dnsmasq restart

#test
dig mx test.cxlm.ramsaysimedarbyhealth.com
dig mx mail.test.cxlm.ramsaysimedarbyhealth.com

host -t mx test.cxlm.ramsaysimedarbyhealth.com

#check zimbra log
tail -f -n 25 /opt/zimbra/log/mailbox.log /var/log/zimbra.log
 
