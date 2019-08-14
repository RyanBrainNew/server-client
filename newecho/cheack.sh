#!/bin/sh
ps -fe|grep Serd |grep -v grep
if [ $? -ne 0 ]
then
echo "start process....."
cd /home/bs/project/Servertest3
./Serd
else
echo "runing....."
fi
