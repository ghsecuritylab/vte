#Copyright (C) 2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#!/bin/sh
###################################################################################################
#
#    @file   <TODO_module_NAME>_test.sh
#
#    @brief  shell script template for testcase design usb remote wake up is where to modify block.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#hake Huang/-----             20090609         N/A          Initial version
#Andy Tian                    20120702         N/A        Add USB remote wakeup stress test 
# 
###################################################################################################



# Function:     setup
#
# Description:  - Check if required commands exits
#               - Export global variables
#               - Check if required config files exits
#               - Create temporary files and directories
#
# Return        - zero on success
#               - non zero on failure. return value from commands ($RC)

setup()
{
#TODO Total test case
export TST_TOTAL=4

export TCID="setup"
export TST_COUNT=0
RC=0

modprobe fsl_otg_arc
modprobe ehci_hcd
sleep 2


trap "cleanup" 0

#TODO add setup scripts

return $RC
}

# Function:     cleanup
#
# Description   - remove temporary files and directories.
#
# Return        - zero on success
#               - non zero on failure. return value from commands ($RC)
cleanup()
{
RC=0

#TODO add cleanup code here

modprobe -r fsl_otg_arc
modprobe -r ehci_hcd
return $RC
}


# Function:     test_case_01
# Description   - Test if usb key stroke wakeup ok
#  
test_case_01()
{
#TODO give TCID 
TCID="usb_keyboard_stroke_wakeup_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
echo
echo
echo =============================================================================
echo "TEST:   Wakeup via usb Keyboard stroke."
echo ACTION: When the system is in standby mode, hit a button on the usb Keyboard stroke.
echo RESULT: The system should wake up.

echo "plese insert the USB key board then hit enter"
read x

echo '--- Hit the Enter key on the console to switch to standby mode ---'
read x
echo -n standby > /sys/power/state
sleep 2

RC=0

return $RC

}

# Function:     test_case_02
# Description   - Test if usb connect wakeup ok
#  
test_case_02()
{
#TODO give TCID 
TCID="usb_device_connect_wakeup_test"
#TODO give TST_COUNT
TST_COUNT=2
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

echo "plese unplug the USB key board first then hit enter"
read x

echo
echo
echo =============================================================================
echo "TEST:   Wakeup when a USB device is connected."
echo ACTION: When the system is in standby mode, connect the USB keyboard.
echo RESULT: The system should wake up and see that the keyboard was connected.

echo '--- Hit the Enter key on the console to switch to standby mode ---'
read x
echo -n standby > /sys/power/state
sleep 2

RC=0

return $RC

}

# Function:     test_case_03
# Description   - Test if usb disconnect wakeup ok
#  
test_case_03()
{
#TODO give TCID 
TCID="test_demo3_test"
#TODO give TST_COUNT
TST_COUNT=3
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

echo "plese insert the USB key board first then hit enter"
read x

sleep 2

echo
echo =============================================================================
echo "TEST:   Wakeup when a USB device is disconnected."
echo ACTION: When the system is in standby mode, disconnect the USB keyboard.
echo RESULT: The system should wake up and see that the keyboard was disconnected.n $RC

echo '--- Hit the Enter key on the console to switch to standby mode ---'
read x
echo -n standby > /sys/power/state
sleep 2

RC=0
return $RC

}

# Function:     test_case_04
# Description   - usb remote wakeup stress test
#  
test_case_04()
{
#TODO give TCID 
TCID="USB remote wakeup stress test"
#TODO give TST_COUNT
TST_COUNT=4
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
if [ -z $skip_port ];then
	echo "Need usb port number as argument. Setup FAIL"
	exit 1
elif [ ! -e "/sys/bus/usb/devices/$skip_port" ]; then
	echo "Can not find the usb port of $skip_port, please check it. Setup FAIL"
	exit 1
fi
#Enable remote wakeup for all usb ports
low_power_usb.sh $skip_port || exit $?

#suspend 3000 times
i=0
while [ $i -lt 3000 ];do
	echo mem > /sys/power/state
	sleep 1
	let i=i+1
	echo $i
	echo $i
	echo $i
done
RC=0
return $RC
}

# Function:     test_case_05
# Description   - Test if <TODO test function> ok
#  
test_case_05()
{
#TODO give TCID 
TCID="test_demo5_test"
#TODO give TST_COUNT
TST_COUNT=5
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

return $RC

}

usage()
{
echo "$0 [case ID]"
echo "1: "
echo "2: "
echo "3: "
echo "4 skip_port: USB remote wake up stress test"
echo "5: "
}

# main function

RC=0

#TODO check parameter
if [ $# -gt 2 ]
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
  skip_port=$2
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







