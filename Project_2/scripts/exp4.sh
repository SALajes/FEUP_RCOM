#!/bin/bash

echo "Numero da bancada:"

read Y

echo "Numero do pc:"

read PC

PC1="1"
SUB="0."

if [ $PC -eq 4 ]
then
  PC1=254
elif [ $PC -eq 2 ]
then
  SUB="1."
fi

IP="172.16."
IP+="${Y}" 
IP+="${SUB}"
IP+="${PC1}"
IP+="/24"

/etc/init.d/networking restart

ifconfig eth0 up
ifconfig eth0 $IP

if [ $PC -eq 4 ]
then 
  IP2="172.16."
  IP2+="${Y}"
  IP2+="1.253/24"
  ROUTERIP="172.16.${Y}1.254"
  ifconfig eth1 up
  ifconfig eth1 $IP2
  echo 1 > /proc/sys/net/ipv4/ip_forward
  echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts
  route add default gw $ROUTERIP eth1
elif [ $PC -eq 2 ]
then 
  IP="172.16."
  IP+="${Y}0.0/24"
  ROUTERIP="172.16.${Y}${SUB}253"
  route add default gw $ROUTERIP eth0
  echo 0 > /proc/sys/net/ipv4/conf/eth0/accept_redirects
  echo 0 > /proc/sys/net/ipv4/conf/all/accept_redirects
  route del -net "172.16.${Y}0.254"
elif [ $PC -eq 1 ]
then 
  IP="172.16."
  IP+="${Y}1.0/24"
  ROUTERIP="172.16.${Y}${SUB}254"
  route add default gw $ROUTERIP eth0
fi

