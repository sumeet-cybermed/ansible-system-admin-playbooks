#!/bin/bash

cd "$(dirname "$0")"

rm -f ./__mappings.txt

for LOOPCNTR in {100..254}
do

PREFIX="5${LOOPCNTR}"
 
 echo "
	####VM ${LOOPCNTR} 
	#ssh
	post-up iptables -t nat -A PREROUTING -i eth0 -p tcp --dport ${PREFIX}2 -j DNAT --to 10.10.10.${LOOPCNTR}:22
	post-down iptables -t nat -D PREROUTING -i eth0 -p tcp --dport ${PREFIX}2 -j DNAT --to 10.10.10.${LOOPCNTR}:22   

	#rdp
	post-up iptables -t nat -A PREROUTING -i eth0 -p tcp --dport ${PREFIX}9 -j DNAT --to 10.10.10.${LOOPCNTR}:3389
	post-down iptables -t nat -D PREROUTING -i eth0 -p tcp --dport ${PREFIX}9 -j DNAT --to 10.10.10.${LOOPCNTR}:3389	 
    
	#http 
	post-up iptables -t nat -A PREROUTING -i eth0 -p tcp --dport ${PREFIX}8 -j DNAT --to 10.10.10.${LOOPCNTR}:80
	post-down iptables -t nat -D PREROUTING -i eth0 -p tcp --dport ${PREFIX}8 -j DNAT --to 10.10.10.${LOOPCNTR}:80
	
	#http 81 - for nginx listening behind haproxy
	post-up iptables -t nat -A PREROUTING -i eth0 -p tcp --dport ${PREFIX}1 -j DNAT --to 10.10.10.${LOOPCNTR}:81
	post-down iptables -t nat -D PREROUTING -i eth0 -p tcp --dport ${PREFIX}1 -j DNAT --to 10.10.10.${LOOPCNTR}:81
	
	#https 
	post-up iptables -t nat -A PREROUTING -i eth0 -p tcp --dport ${PREFIX}3 -j DNAT --to 10.10.10.${LOOPCNTR}:443
	post-down iptables -t nat -D PREROUTING -i eth0 -p tcp --dport ${PREFIX}3 -j DNAT --to 10.10.10.${LOOPCNTR}:443
	
	#mysql 
	post-up iptables -t nat -A PREROUTING -i eth0 -p tcp --dport ${PREFIX}6 -j DNAT --to 10.10.10.${LOOPCNTR}:3600
	post-down iptables -t nat -D PREROUTING -i eth0 -p tcp --dport ${PREFIX}6 -j DNAT --to 10.10.10.${LOOPCNTR}:3600
	
	#ftp
	post-up iptables -t nat -A PREROUTING -i eth0 -p tcp --dport ${PREFIX}1 -j DNAT --to 10.10.10.${LOOPCNTR}:21
	post-down iptables -t nat -D PREROUTING -i eth0 -p tcp --dport ${PREFIX}1 -j DNAT --to 10.10.10.${LOOPCNTR}:21
	
"	>> ./_proxmox_mappings.txt
done