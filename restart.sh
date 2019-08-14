#!/bin/sh 
#===================== 
while : 
do 
echo "Current DIR is " $PWD

processname='Serd'
PROCESS=`ps -ef|grep $processname|grep -v grep|grep -v PPID|awk '{ print $2}'`

stillRunning=$(ps -ef |grep "$PWD/Serd" |grep -v "grep") 
if [ "$stillRunning" ] ; then 
echo "TWS service was already started by another way" 
echo "Kill it and then startup by this shell, other wise this shell will loop out this message annoyingly" 
#kill -9 $pidof $PWD/Serd 

for i in $PROCESS
   do
   echo "Kill the $1 process [ $i ]"
   kill -9 $i
 done
 
else 
echo "The service was not started" 
echo "Starting service ..." 
$PWD/Serd 
echo "Finish" 
fi 
sleep 10 
done 
