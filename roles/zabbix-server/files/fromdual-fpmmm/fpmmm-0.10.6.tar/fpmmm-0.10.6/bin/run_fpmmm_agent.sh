#/bin/bash

sleep=10
conf="/etc/zabbix/fpmmm.conf"
fpmmm_agent="/opt/fpmmm/bin/fpmmm"

# This is a daemon script
while [ 1 ] ; do

  $fpmmm_agent $conf
  if [ $sleep $lt 10 ] ; then
    sleep 10
  else
    sleep $sleep
  fi
done
# We never exit here!
