#!/bin/bash
#Copyright 2008-2011 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#
#
# File :        dvfs.sh
#
# Description:  DVFS test
#    
#===============================================================================
#Revision History:
#                            Modification     Tracking
# Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -----------------------
# Blake                      20081015
# Spring Zhang               20100108            n/a      Reduce code
# Spring Zhang               Jan.11,2010         n/a      Add dvfs suspend test
# Spring Zhang               May.10,2010         n/a      Add mx53 support
# Spring Zhang               Jun.13,2010         n/a      Add dead loop detection test


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
    RC=0

    # Total number of test cases in this file. 
    export TST_TOTAL=3

    # The TCID and TST_COUNT variables are required by the LTP 
    # command line harness APIs, these variables are not local to this program.

    # Test case identifier
    export TCID="TGE_LV_DVFS_TEST"
    # Set up is initialized as test 0
    export TST_COUNT=0
    # Initialize cleanup function to execute on program exit.
    # This function will be called before the test program exits.
    BIN_DIR=`dirname $0`
    export PATH=$PATH:$BIN_DIR

    LTPTMP=${TMP}        # Temporary directory to create files, etc.
    if [ -z $LTPTMP ]
    then
        LTPTMP=/tmp
    fi

    platfm.sh || PLATFORM=$?
    if [ $PLATFORM -eq 67 ]
    then
        RC=$PLATFORM
        return $RC
    fi

    #store current dvfs status. cur_status=1 - enabled, =0 - disabled
    cur_status=`cat ${DVFS_DIR[$PLATFORM]}/${status[$PLATFORM]} | grep "enabled" | wc -l`

    trap "cleanup" 0

    return $RC
}

cleanup()
{
    #resume to old status
    tst_resm TINFO "Resume to old dvfs status"
    echo $cur_status > ${DVFS_DIR[$PLATFORM]}/${status[$PLATFORM]} 
}

# Function:     usage
#
# Description:  - display help info.
#
# Return        - none
usage()
{
cat <<-EOF

    Use this command to test DVFS functions.
    usage: ./${0##*/} 1  -- basic test 
           ./${0##*/} 2  -- suspend test 
    e.g.: ./${0##*/} 2

EOF
}


dvfs_dir_set()
{
    #use bash array
    DVFS_DIR[31]=/sys/devices/system/dvfs/dvfs0
    DVFS_DIR[35]=/sys/devices/system/dvfs/dvfs0
    DVFS_DIR[37]=/sys/devices/platform/mxc_dvfs_core.0
	# modified dvfs dir, developer restructure DVFS CORE to common driver. 
    # added by blake , 2009-02-17
	DVFS_DIR[51]=/sys/devices/platform/mxc_dvfs_core.0
    #i.MX51 BBG
	DVFS_DIR[41]=/sys/devices/platform/mxc_dvfs_core.0
	DVFS_DIR[53]=/sys/devices/platform/mxc_dvfs_core.0
	#imx50 arm2
	DVFS_DIR[50]=/sys/devices/platform/mxc_dvfs_core.0

    #dvfs status query
    status[31]=status
    status[35]=status
    status[37]=enable
    status[51]=enable
    status[41]=enable
    status[53]=enable
    status[50]=enable
}

dvfs_test()
{
    RC=0
    export TCID="TGE_LV_DVFS_BASIC"
    export TST_COUNT=1

    # For imx31/35/37/51/53/50 
    echo 1 > ${DVFS_DIR[$PLATFORM]}/enable
    res=`cat ${DVFS_DIR[$PLATFORM]}/${status[$PLATFORM]} | grep "enabled" | wc -l`
    if [ $res -eq 1 ];then
        tst_resm TPASS "dvfs is enabled"
    else
        tst_resm TFAIL "fail to enable dvfs"
        RC=1
        return $RC
    fi
    sleep 3 
    
    echo 0 > ${DVFS_DIR[$PLATFORM]}/enable
    res=`cat ${DVFS_DIR[$PLATFORM]}/${status[$PLATFORM]} | grep "disabled" | wc -l`
    if [ $res -eq 1 ];then
        tst_resm TPASS "dvfs is disabled"
    else
        tst_resm TFAIL "fail to disable dvfs"
        RC=1
    fi
	
    return $RC
}

dvfs_suspend()
{
    export TCID="TGE_LV_DVFS_SUSPEND"
    export TST_COUNT=2
    RC=0

    # For imx31/35/37/51 
    echo 1 > ${DVFS_DIR[$PLATFORM]}/enable
    res=`cat ${DVFS_DIR[$PLATFORM]}/${status[$PLATFORM]} | grep "enabled" | wc -l`
    if [ $res -eq 1 ];then
        tst_resm TINFO "dvfs is enabled"
    else
        tst_resm TFAIL "fail to enable dvfs"
        RC=1
        return $RC
    fi
    sleep 3 
    
    echo "*************************************"
    echo "* please press key to resume system *"
    echo "*************************************"
    rtc_testapp_6 -T 50 -m standby
		tst_resm TPASS "Resume from suspend..."

    sleep 5
    echo "*************************************"
    echo "* please press key to resume system *"
    echo "*************************************"
    rtc_testapp_6 -T 50 -m mem
    tst_resm TPASS "Resume from mem..."
    
    echo 0 > ${DVFS_DIR[$PLATFORM]}/enable
    res=`cat ${DVFS_DIR[$PLATFORM]}/${status[$PLATFORM]} | grep "disabled" | wc -l`
    if [ $res -eq 1 ];then
        tst_resm TINFO "dvfs is disabled"
    else
        tst_resm TFAIL "fail to disable dvfs"
        RC=1
    fi
	
    return $RC
}

#detect dead loop: arm_podf_busy bit
dvfs_deadloop()
{
    RC=0

    export TCID="TGE_LV_DVFS_DEADLOOP"
    export TST_COUNT=3

    old_printk=`echo /proc/sys/kernel/printk`
    echo 8 > /proc/sys/kernel/printk
    echo 1 > /sys/class/graphics/fb0/blank
    ifconfig eth0 down
    ifconfig eth1 down 2>/dev/null
    echo 1 > ${DVFS_DIR[$PLATFORM]}/enable

    sleep 5
    dmesg |tail |grep -i "ARM_PODF still in busy"
    RC=$?

    ifconfig eth0 up
    ifconfig eth1 up
    echo 0 > /sys/class/graphics/fb0/blank
    echo $old_printk > /proc/sys/kernel/printk

    return $RC
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

# bash specified script, using array, not dash-compatibility
dvfs_dir_set || exit $RC
setup  || exit $RC

case "$1" in
    1)
    dvfs_test || exit $RC
    ;;
    2)
    dvfs_suspend || exit $RC
    ;;
    3)
    dvfs_deadloop || exit $RC
    ;;
    *)
    usage
    exit 67
    ;;
esac


