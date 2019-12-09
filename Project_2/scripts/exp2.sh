#!/bin/bash

echo "Numero da bancada:"

read Y

echo "Numero do pc:"

read PC

SUB="0."

if [ $PC -eq 4 ]
then
  PC=254
elif [ $PC -eq 2 ]
then
  PC="1"
  SUB="1."
fi

IP="172.16."
IP+="${Y}" 
IP+="${SUB}"
IP+="${PC}"
IP+="/24"

/etc/init.d/networking restart

ifconfig eth0 $IP


