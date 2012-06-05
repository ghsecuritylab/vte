#!/bin/bash

setup()
{
#TODO Total test case
export TST_TOTAL=4
 
export TCID="setup"
export TST_COUNT=0
RC=0
 
trap "cleanup" 0
 
#TODO add setup scripts
return $RC
}

cleanup()
{
RC=0
 
#TODO add cleanup code here
return $RC
}

test_case_01()
{
#TODO give TCID
TCID="SMP_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

times=500

while [ $times -gt 0 ]
do
echo 0 > /sys/devices/system/cpu/cpu1/online
echo 0 > /sys/devices/system/cpu/cpu2/online
echo 0 > /sys/devices/system/cpu/cpu3/online

sleep 1

echo 1 > /sys/devices/system/cpu/cpu1/online
echo 1 > /sys/devices/system/cpu/cpu2/online
echo 1 > /sys/devices/system/cpu/cpu3/online

sleep 1
times=$(expr $times - 1)
echo $times
done

return $RC
}

test_case_02()
{
#TODO give TCID
TCID=""
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
times=5000

while [ $times -gt 0 ]
do
echo 0 > /sys/devices/system/cpu/cpu1/online
echo 0 > /sys/devices/system/cpu/cpu2/online
echo 0 > /sys/devices/system/cpu/cpu3/online

echo 1 > /sys/devices/system/cpu/cpu1/online
echo 1 > /sys/devices/system/cpu/cpu2/online
echo 1 > /sys/devices/system/cpu/cpu3/online

times=$(expr $times - 1)
echo $times
done


return $RC
}

test_case_03()
{
#TODO give TCID
TCID=""
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
times=5000

mkdir /mnt/mmcblk0p1
mount /dev/mmcblkop1 /mnt/mmcblk0p1

while [ $times -gt 0 ]
do
echo 0 > /sys/devices/system/cpu/cpu1/online
echo 0 > /sys/devices/system/cpu/cpu2/online
echo 0 > /sys/devices/system/cpu/cpu3/online

echo 1 > /sys/devices/system/cpu/cpu1/online
echo 1 > /sys/devices/system/cpu/cpu2/online
echo 1 > /sys/devices/system/cpu/cpu3/online

bonnie\+\+ -d /mnt/mmcblk0p1 -s 32 -r 16 -u 0:0 -m FSL &
cat /proc/interrupts

times=$(expr $times - 1)
echo $times
done

wait
umount /mnt/mmcblk0p1

return $RC
}

test_case_04()
{
#TODO give TCID
TCID="jiffies test"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

jiffes_list=$(cat /proc/timer_list | grep jiffies: | awk '{print $2}')
jiffes_o=0

for i in $jiffes_list
do
  if [ $jiffes_o -eq 0  ];then
	  jiffes_o=$i
  else
	  if [ $jiffes_o -gt $i  ]; then
	  	RC=$(expr $RC + 1)
	  fi
	jiffes_o=$i
  fi
done

return $RC
}

test_case_05()
{
#TODO give TCID
TCID="local timer test"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

interrupts=$(cat /proc/interrupts | grep "Local timer interrupts")
timers=$(echo $interrupts | wc -w)
timercount=0

if [ $timers -gt 4 ]; then
	timercount=$(expr $timers - 2) 
fi

if [ $timercount -eq 0 ];then
	return 5
fi

i=2
while [ $i -lt $timercount ]
do
	value=$(echo $interrupts | cut -d " " -f $i)
	i=$(expr $i + 1)
	if [ $value -eq 0 ]; then
		RC=$(expr $RC + 1)
	fi
done

return $RC
}

useage()
{
echo "1: cpu hotplug 500 times "	
echo "2: cpu hotplug 5000 times "
echo "3: high interrupt test"
echo "4: time jiffies test"
echo "5: local timer test"
}

# main function
 
RC=0
 
#TODO check parameter
if [ $# -ne 1 ]
then
usage
exit 1
fi
 
setup || exit $RC
 
case "$1" in
1)
  test_case_01 || exit $RC
  ;;
2)
  test_case_02 || exit $RC
  ;;
3)
  test_case_03 || exit $RC
  ;;
4)
  test_case_04 || exit $RC
  ;;
5)
  test_case_05 || exit $RC
  ;;
*)
  usage
  ;;
esac
 
tst_resm TINFO "Test PASS"

