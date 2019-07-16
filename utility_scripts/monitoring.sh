#1 /bin/git ls-remote  fails behind firewall
/bin/git ls-remote git://github.com/preinheimer/xhprof.git -h refs/heads/HEAD
#check cron settings
nano /etc/crontab

#check cron jobs
crontab -e

#monitor cron job runs
tail -f -n  100 /var/log/cron

#monitor cpu etc/crontab
htop

#monitor network
iftop

#monitor ccrm
tail -f -n 100 /var/www/ccrm/10SuiteCRM/_ccrm.log

#monitor cms_LogMessage
tail -f -n 100 /var/www/ccrm/10SuiteCRM/_cms_logMessage.log


#check zimbra log
tail -f -n 25 /opt/zimbra/log/mailbox.log

#mount folders on developer machine
umount -a
mount -o fileaccess=777 rsize=32 wsize=32 mtype=soft timeout=2  -u:root  -p:81@CXLM.test \\ccrm.test-cxlm\etc *
mount -o fileaccess=777 rsize=32 wsize=32 mtype=soft timeout=2  -u:root  -p:81@CXLM.test \\ccrm.test-cxlm\var *

#open logfiles
rm -f /var/www/ccrm/10-CCRM-Core/10-WebRoot/*.log
