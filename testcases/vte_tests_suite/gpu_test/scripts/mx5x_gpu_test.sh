#!/bin/sh
#Copyright (C) 2009-2011 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#
###############################################################################
#
#    @file   gpu_test.sh
#
#    @brief  shell script template for testcase design "gpu" is where to modify block.
#
################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#------------------------   ------------    ----------  -----------------------
#Hake Huang/-----             20090512     N/A          Initial version
#Spring Zhang                 20100706     N/A          Use common module installer
#Spring Zhang                 20110321     N/A          use fifo to end tiger
# 
################################################################################

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

    trap "cleanup" 0

    #TODO add setup scripts
    #gpu-install install
    #gpu_maj=`grep "gsl_kmod" /proc/devices | cut -b1,2,3`
    #mknod /dev/gsl_kmod c "$gpu_maj" 0
    modprobe gpu
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
    #modprobe gpu -r
    #rm -f /dev/gsl_kmod
    return $RC
}


# Function:     test_case_01
# Description   - Test if GSL ok
#  
test_case_01()
{
    #TODO give TCID 
    TCID="GSL_test"
    #TODO give TST_COUNT
    TST_COUNT=1
    RC=0

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    #TODO add function test script here

    gsl_sanity_app || RC=1

    return $RC
}

# Function:     test_case_02
# Description   - Test if openvg ok
#  
test_case_02()
{
    #TODO give TCID 
    TCID="openvg_test"
    #TODO give TST_COUNT
    TST_COUNT=2
    RC=0

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    #TODO add function test script here

    tmpdir=$(mktemp -d)
    mkdir $tmpdir
    mkfifo $tmpdir/tiger_fifo
    sh -c "cat $tmpdir/tiger_fifo | tiger" &
    sleep 20
    echo "" > $tmpdir/tiger_fifo

    rm -rf $tmpdir

    echo "TEST PASS"
    return $RC
}

# Function:     test_case_03
# Description   - Test if gles ok
#  
test_case_03()
{
    #TODO give TCID 
    TCID="gles_test"
    #TODO give TST_COUNT
    TST_COUNT=3
    RC=0

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    #TODO add function test script here
    simple_draw -f 100 && torusknot 100 

    if [ $? -eq 0 ]; then
    echo "TEST PASS"
    else
    RC=1
    echo "TEST FAIL"
    fi

    return $RC
}

# Function:     test_case_04
# Description   - Test if stress gles ok
#  
test_case_04()
{
    #TODO give TCID 
    TCID="stress_gles_test"
    #TODO give TST_COUNT
    TST_COUNT=4
    RC=0

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    #TODO add function test script here
    echo "######################"
    echo "run this case overnight and check program still ok"

    es11ex

    if [ $? -eq 0 ]; then
    echo "TEST PASS"
    else
    RC=1
    echo "TEST FAIL"
    fi

    return $RC
}

# Function:     test_case_05
# Description   - Test if power manager ok
#  
test_case_05()
{
    #TODO give TCID 
    TCID="gles_pm_test"
    #TODO give TST_COUNT
    TST_COUNT=5
    RC=0

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    #TODO add function test script here
    tmpdir=$(mktemp -d)
    mkfifo $tmpdir/tiger_fifo
    mkfifo $tmpdir/es11ex_fifo
    sh -c "cat $tmpdir/tiger_fifo | tiger -f 10000" &
    sh -c "cat $tmpdir/es11ex_fifo | es11ex" &
    sleep 2

    rtc_testapp_6 -T 50

    echo y > $tmpdir/tiger_fifo
    echo y > $tmpdir/es11ex_fifo

    #try again
    sh -c "cat $tmpdir/tiger_fifo | tiger -f 10000" &
    sh -c "cat $tmpdir/es11ex_fifo | es11ex" &
    sleep 2

    rtc_testapp_6 -T 50
    sleep 1
    rtc_testapp_6 -T 50
    sleep 1
    rtc_testapp_6 -T 50
    sleep 1
    rtc_testapp_6 -T 50

    echo y > $tmpdir/tiger_fifo
    echo y > $tmpdir/es11ex_fifo

    echo "test PASS"
    echo "now reboot system!"
    reboot

    return $RC
}

usage()
{
    echo "$0 [case ID]"
    echo "1: GSL test"
    echo "2: openvg test"
    echo "3: gles test"
    echo "4: gles stress test"
    echo "5: power managerment test"
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
