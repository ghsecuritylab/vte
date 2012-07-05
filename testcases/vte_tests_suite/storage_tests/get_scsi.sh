#!/bin/bash
##############################################################################
#Copyright (C) 2012 Freescale Semiconductor, Inc.
#All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
##############################################################################
#
# Revision History:
# Author                   Date     Description of Changes
#-------------------   ------------ -----------------------
# Andy Tian             07/04/2012  Initial version


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
    #check sd* device
	scsi_dir=/sys/class/scsi_device
	scsi_device=`ls $scsi_dir`
	if [ -z "$scsi_device" ];then
		#no scsi device, exit
		exit 1
    fi

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
    return $RC
}

# Function:     test_case_01
# Description   - get the first ata device
#
test_case_01()
{
    #TODO give TCID
    TCID="get the first ata device"
    #TODO give TST_COUNT
    TST_COUNT=1
    RC=1


    #TODO add function test script here
	ata_node=`ls -l $scsi_dir | grep -m 1 ahci | awk '{print $9}'`
	if [ -n "$ata_node" ];then
		ls $scsi_dir/$ata_node/device/block
		RC=0
	fi
	return $RC
}

# Function:     test_case_02
# Description   - get all ata devices
#
test_case_02()
{
    #TODO give TCID
    TCID="get all ata devices"
    #TODO give TST_COUNT
    TST_COUNT=2
    RC=2

    #TODO add function test script here
	for i in $scsi_device; do
		ata_node=`readlink $scsi_dir/$i | grep ahci`
		if [ -n "$ata_node" ];then
			ata_dev=`ls $scsi_dir/$i/device/block`
			ata_devices="$ata_devices $ata_dev"
		fi
	done
	if [ -n "$ata_devices" ]; then
		echo $ata_devices
		RC=0
	fi
	return $RC
}

# Function:     test_case_03
# Description   - get the first usb scsi device
#
test_case_03()
{
    #TODO give TCID
    TCID="get the first usb scsi device"
    #TODO give TST_COUNT
    TST_COUNT=3
    RC=3


    #TODO add function test script here
	ata_node=`ls -l $scsi_dir | grep -m 1 usb | awk '{print $9}'`
	if [ -n "$ata_node" ];then
		ls $scsi_dir/$ata_node/device/block
		RC=0
	fi
	return $RC
}

# Function:     test_case_04
# Description   - get all usb scsi devices
#
test_case_04()
{
    #TODO give TCID
    TCID="get all usb scsi devices"
    #TODO give TST_COUNT
    TST_COUNT=4
    RC=4

    #TODO add function test script here
	for i in $scsi_device; do
		usb_node=`readlink $scsi_dir/$i | grep usb`
		if [ -n "$usb_node" ];then
			usb_dev=`ls $scsi_dir/$i/device/block`
			usb_devices="$usb_devices $usb_dev"
		fi
	done
	if [ -n "$usb_devices" ]; then
		echo $usb_devices
		RC=0
	fi
	return $RC
}

usage()
{
    cat <<-EOF

Get scsi devices name for ata or usb:
`basename $0` [case ID]
    1: Get the first ata device name
    2: Get all ata devices name
    3: Get the first usb scsi device name
    4: Get all usb scsi devices name
EOF

    exit 1
}

# main function

RC=0

#TODO check parameter
if [ $# -ne 1 ]
then
    usage
fi

setup || exit $RC

case "$1" in
1|"ata")
    test_case_01 || exit $RC
    ;;
2|"atas")
    test_case_02 || exit $RC
    ;;
3|"usb")
    test_case_03 || exit $RC
    ;;
4|"usbs")
    test_case_04 || exit $RC
    ;;
*)
    usage
    ;;
esac
