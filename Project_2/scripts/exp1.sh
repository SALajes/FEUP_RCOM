#!/bin/bash

echo "Numero da bancada:"

read Y

echo "Numero do pc:"

read PC

if [ $PC -eq 4 ]
then
  PC=254
fi

IP="172.16."
IP+="${Y}" 
IP+="0."
IP+="${PC}"
IP+="/24"

/etc/init.d/networking restart

ifconfig eth0 $IP


