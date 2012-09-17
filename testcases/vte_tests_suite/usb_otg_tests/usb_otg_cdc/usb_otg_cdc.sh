#!/bin/sh
#Copyright (C) 2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
###################################################################################################
#
#    @file   usb_otg_cdc.sh
#
#    @brief  shell script template for testcase design "TODO" is where to modify block.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#Hake Huang/-----             02/12/2009     N/A          Initial version
# 

#Andy Tian/-----              11/15/2011     N/A          The old script may return 0 even there 
#                                                          is package lost. Fix this error
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
export TST_TOTAL=1

export TCID="setup"
export TST_COUNT=0
RC=1

trap "cleanup" 0

#TODO add setup scripts



modprobe g_ether || return 1

ifconfig usb0 $CLIENTIP up || return 1

echo "please connect the board to host with usb cable"

sleep 3

echo "Target board IP is $CLIENTIP"
echo "please set up the host usb0 to $HOSTIP"
echo "plese run following command on host"
echo "modprobe usbnet;modprobe cdc_ether;modprobe g_ether"
echo "ifconfig usb0 $HOSTIP up"

echo "press enter when ready"
read -p tt

RC=0
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

#TODO add cleanup code here

modprobe -r g_ether || return 1
#modprobe -r cdc_ether  #no such device
#modprobe -r usbnet    #no such module

return $RC
}


usage()
{
echo "$0 <testcase ID>"
echo "1: icmp test"
}


# Function:     test_case_01
# Description   - Test if cdc icmp ok
#  
test_case_01()
{
#TODO give TCID 
TCID="cdc_icmp_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

PACKSIZE="64 128 256 512 1024 2048 4096 8192 16384"

for i in $PACKSIZE
do
loss=$(ping $HOSTIP -c 4 -s $i | grep ", 0% packet loss")
if [ -z "$loss" ]
then
return 1
fi 
done



RC=0
return $RC

}

# Function:     test_case_02
# Description   - Test if <TODO test function> ok
#  
test_case_02()
{
#TODO give TCID 
TCID="test_demo2_test"
#TODO give TST_COUNT
TST_COUNT=2
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

return $RC

}

# Function:     test_case_03
# Description   - Test if <TODO test function> ok
#  
test_case_03()
{
#TODO give TCID 
TCID="test_demo3_test"
#TODO give TST_COUNT
TST_COUNT=3
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

return $RC

}

# Function:     test_case_04
# Description   - Test if <TODO test function> ok
#  
test_case_04()
{
#TODO give TCID 
TCID="test_demo4_test"
#TODO give TST_COUNT
TST_COUNT=4
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

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

# main function

RC=0

#TODO check parameter
if [ $# -ne 1 ]
then
usage
exit 1 
fi

HOSTIP=11.11.11.11
CLIENTIP=11.11.11.12

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
#TODO check parameter
  usage
  ;;
esac

tst_resm TINFO "Test PASS"








