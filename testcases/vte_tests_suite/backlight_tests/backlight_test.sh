#!/bin/sh
#######################################################################
#Copyright 2008-2011 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#######################################################################
#
# File :        backlight_test.sh
#
# Description:  This is a test to set/get backlight functions.
#
#======================================================================
#Revision History:
#                       Modification     Tracking
# Author                    Date          Number    Description of Changes
#--------------------   ------------    ----------  -----------------------
# Blake                   20081015
# Spring.Zhang/b17931     20100323         n/a      Add MX53 support
# Spring.Zhang/b17931     20111216         n/a      Add MX6Q support

#Set path variable to add vte binaries
#export TESTCASES_HOME= `pwd`
#export PATH=${PATH}:${TESTCASES_HOME}

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
    # Total number of test cases in this file. 
    export TST_TOTAL=1

    # The TCID and TST_COUNT variables are required by the LTP 
    # command line harness APIs, these variables are not local to this program.

    # Test case identifier
    export TCID="BACKLIGHT_TEST"
    # Set up is initialized as test 0
    export TST_COUNT=0
    # Initialize cleanup function to execute on program exit.
    # This function will be called before the test program exits.
    trap "cleanup" 0
    RC=0
    return $RC
}

cleanup()
{
    echo "CLEANUP "
}

env_test()
{
    RC=0

    # get cpu info
    info=`cat /proc/cpuinfo | grep "Revision" | grep "370.*" | wc -l`
    if [ $info -eq 1 ]
    then 
        BL_DIR=/sys/class/backlight/wm8350-bl.2
        RGB_DIR=0
    fi

    info=`cat /proc/cpuinfo | grep "Revision" | grep "371.*" | wc -l`
    if [ $info -eq 1 ]
    then
        BL_DIR=/sys/class/backlight/mxc_mc13892_bl.0
        RGB_DIR=0
    fi

    platStr=`platfm.sh`
    platfm=$?

    if [ $platfm -eq 51 ] || [ $platfm -eq 41 ]; then
        BL_DIR=/sys/class/backlight/pwm-backlight.0
        RGB_DIR=/sys/class/leds/pmic_leds
    fi

    if [ $platfm -eq 53 ]; then  #WVGA
        BL_DIR=/sys/class/backlight/pwm-backlight.0
    fi

    if [ $platfm -eq 61 ] || [ $platfm -eq 63 ]; then  #LVDS
	BL_DIR=`find /sys/class/backlight -name "*backlight*" | sed -n '$p'`
    fi

    if [ ! -d $BL_DIR ]
    then
        tst_resm TFAIL "There is no such directory: $BL_DIR "
        RC=1
        return $RC
    fi

    echo "backlight path is $BL_DIR"
    return $RC
}

brightness_test()
{
    RC=0

    max=`cat $BL_DIR/max_brightness`
    default=`cat $BL_DIR/brightness`
    echo "MAX brightness is $max"
    # Set brightness from 0~max
    myvar=0
    while [ $myvar -le $max ]
    do 
        echo $myvar > $BL_DIR/brightness
        val=`cat $BL_DIR/brightness`
        if [ $myvar -eq $val ]
        then
            tst_resm TINFO "Set brightness to $val "
        else
            tst_resm TFAIL "Fail to set brightness to $val"
            RC=1
        fi
        myvar=$((myvar + 1))
	if [ $max -le 20 ]; then
	    sleep 1
	fi
    done
    myvar=$((myvar - 1))
    while [ $myvar -ge 0 ]
    do
        echo $myvar > $BL_DIR/brightness
        val=`cat $BL_DIR/brightness`
        if [ $myvar -eq $val ]
        then
            tst_resm TINFO "Set brightness to $val "
        else
            tst_resm TFAIL "Fail to set brightness to $val"
            RC=1
        fi
        myvar=$((myvar - 1))
	if [ $max -le 20 ]; then
	    sleep 1
	fi
    done
  
    if [ $RC -eq 0 ]
    then
        tst_resm TPASS "Pass to test brightness"
    else 
        tst_resm TFAIL "Fail to test brightness"
    fi
    
    echo $default > $BL_DIR/brightness
    return $RC
}

rgb_test()
{
    RC=0
    myvar=0
    default=`cat ${RGB_DIR}$1/brightness`
    while [ $myvar -le 255 ]
    do 
        echo $myvar > ${RGB_DIR}$1/brightness
        val=`cat ${RGB_DIR}$1/brightness`
        if [ $myvar -eq $val ]
        then
            tst_resm TINFO "Set brightness to $val "
        else
            tst_resm TFAIL "Fail to set brightness to $val"
            RC=1
            break
        fi
        myvar=$((myvar + 1))
    done
    myvar=255
    while [ $myvar -ge 0 ]
    do
        echo $myvar > ${RGB_DIR}$1/brightness
        val=`cat ${RGB_DIR}$1/brightness`
        if [ $myvar -eq $val ]
        then
            tst_resm TINFO "Set brightness to $val "
        else
            tst_resm TFAIL "Fail to set brightness to $val"
            RC=1
            break
        fi
        myvar=$((myvar - 1))
    done
  
    if [ $RC -eq 0 ]
    then
        tst_resm TPASS "Pass to test brightness"
    else 
        tst_resm TFAIL "Fail to test brightness"
    fi
    
    echo $default > ${RGB_DIR}$1/brightness
    return $RC

}

help()
{ 
    echo "***************************************"
    echo Pls run this testcase with parameter:
    echo LCD ----- change main lcd brightness
    echo LED ----- change led rgb brightness
    echo i.e  backlight_test.sh LED
    echo "***************************************"
}

# Function:     main
#
# Description:  - Execute all tests, exit with test status.
#
# Exit:         - zero on success
#               - non-zero on failure.
#
# Return value from setup, and test functions.
RC=0

setup  || exit $RC
env_test || exit $RC
if [ "$1" = "LCD" ]; then
    brightness_test || exit $RC
else if [ "$1" = "LED" ] && [ "$RGB_DIR" != "0" ]; then
        rgb_test r 
        rgb_test g
        rgb_test b
        exit $RC
    else 
        help
    fi
fi
