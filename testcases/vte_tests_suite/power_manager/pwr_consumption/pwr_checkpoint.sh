#!/bin/sh
###############################################################################
# Copyright (C) 2011 Freescale Semiconductor, Inc. All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
###############################################################################
#Revision History:
#                       Modification     Tracking
# Author                    Date          Number    Description of Changes
#--------------------   ------------    ----------  ----------------------
# Spring Zhang           Apr.13,2010       n/a      Initial version
# Spring Zhang           Apr.13,2010       n/a      Add voltage check

# Note:
# You can generate usage and clk config file by:
# cat /proc/cpu/clocks| grep -v " 0 " |awk '{$2=""; $4=""; print }'|sed 's/(//'|sed 's/)//' > cfg_file

setup()
{
	#general setting
	VDDGP=/sys/class/regulator/regulator.10/microvolts
	VCC=/sys/class/regulator/regulator.11/microvolts
	CPU_CTRL=/sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed
	CUR_FREQ_GETTER=/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq
	DVFSCORE_DIR=/sys/devices/platform/mxc_dvfs_core.0
	DVFSPER_DIR=/sys/devices/platform/mxc_dvfs_per.0
	BUSFREQ_DIR=/sys/devices/platform/busfreq.0
	CLK_GETTER=/proc/cpu/clocks
	#display setting
	fb0=/sys/class/graphics/fb0/blank
	fb1=/sys/class/graphics/fb1/blank
	#TVout
	fb2=/sys/class/graphics/fb2/blank

    #clk_name, usage, clk_freq column index in /proc/cpu/clocks
    col_clkname=1
    col_usage=3
    col_freq=5

    echo "stop gdm..."
    stop gdm
	#platform related
	if [ $platfm -eq 53 ]; then
		echo
	fi

    trap "cleanup" 0
}

cleanup()
{
    #TODO: restore old value
    RC=0
    return $RC
}

usage()
{
    cat << EOF
    usage1: ./${0##*/} voltage config_file
        config_file: the checking point config file
    usage2: ./${0##*/} clock idle_mode config_file
        idle_mode: useridle, sysidle
        config_file: the checking point config file
EOF
}

# @params:
# $1-platform code
sys_idle()
{
    ifconfig eth0 down
	echo 1 > $fb0
	enable_dvfs_$platfm
}

# @params:
# $1-platform code
user_idle()
{
	echo 0 > $fb0
    ifconfig eth0 down
	enable_dvfs_$platfm
}

enable_dvfs_53()
{
    echo 1 > $DVFSCORE_DIR/enable
    echo 1 > $BUSFREQ_DIR/enable 
}

enable_dvfs_51()
{
    echo 1 > $DVFSCORE_DIR/enable
    echo 1 > $DVFSPER_DIR/enable 
}

disable_dvfs_53()
{
    echo 0 > $DVFSCORE_DIR/enable
    echo 0 > $BUSFREQ_DIR/enable 
}

disable_dvfs_51()
{
    echo 0 > $DVFSCORE_DIR/enable
    echo 0 > $DVFSPER_DIR/enable 
}

# @params:
# $1-clocks check points file
# check points file format:
# clkname	usage	frequency
# ddr_clk-0	1		133MHz
check_clks()
{
    RC=0
	clk_matrix_file="$1"

    # check active clocks number
    active_clk=`cat $clk_matrix_file| grep -v " 0 "| wc -l`
    cur_active_clk=`cat $CLK_GETTER | grep -v " 0 "| wc -l`
    if [ $active_clk -ne $cur_active_clk ]; then
        echo "FATAL ERROR: active clocks number doesn't align, now it is $cur_active_clk"
        RC=21
    fi
	
	cat $clk_matrix_file | while read line; do
        #ignore comments line
        if [ -z "$line" ]; then
            continue
        fi
        if echo $line |grep "^#" >/dev/null 2>&1; then
            continue
        fi
		clk_name=`echo $line | awk '{ print $1 }'`
		clk_usage=`echo $line | awk '{ print $2 }'`
		clk_freq=`echo $line | awk '{ print $3 }'`
		if cat $CLK_GETTER | grep $clk_name >/dev/null 2>&1; then
			cur_clk_usage=`cat $CLK_GETTER| grep $clk_name | awk -v col="$col_usage" '{print $col}'`
			cur_clk_freq=`cat $CLK_GETTER| grep $clk_name| sed 's/(//' |sed 's/)//'| awk -v col="$col_freq" '{print $col}'`
			if [ "$clk_usage" != "$cur_clk_usage" ]; then
				echo "WARNING $clk_name: usage not aligned, current usage is $cur_clk_usage"
                RC=21
			fi
			if [ "$cur_clk_freq" != "$clk_freq" ]; then
				echo "FATAL ERROR $clk_name: frequency different, current freq is $cur_clk_freq"
                RC=21
			fi
		else
			echo "WARNING $clk_name: no such clock"
		fi		
	done

    return $RC
}

# Check voltage in different working points
# @params:
# $1-voltage check points file
# check points file format:
# wp		gp_voltage
# 400000	135000
check_voltage()
{
    RC=0

	#need to disable DVFS first
    check_file="$1"
    cat $check_file | while read line; do
        if "$line" == ""; then
            continue
        fi
        if echo $line |grep "^#" >/dev/null 2>&1; then
            continue
        fi
		wp=`echo $line | awk '{ print $1 }'`
		vol=`echo $line | awk '{ print $2 }'`
		echo $wp > $CPU_CTRL
		cur_vol=`cat $VDDGP`
		if [ "$vol" != "$cur_vol" ]; then
			echo "FATAL ERROR WP-$wp: voltage different, current vol is $cur_vol, reference is $vol"
            RC=22
		fi
	done

    return $RC
}

#main
platfm.sh || platfm=$?
if [ $platfm -eq 67 ]; then
	RC=$platfm
	echo "SoC Platform doesn't support"
	return $RC
fi

setup

check_type=$1

#TODO: use getopts to handle params
case $check_type in
	"clock")
		if [ $# -lt 3 ]; then
            usage
			exit 3
		fi
		mode=$2
		clk_points_file=$3
		case $mode in
		"useridle")
		user_idle
		;;
		"sysidle")
		sys_idle
		;;
		*)
        usage
        exit 1
		;;
		esac
		check_clks $clk_points_file || exit $RC
	;;
	"voltage")
		if [ $# -lt 2 ]; then
            usage
			exit 2
		fi
		clk_points_file=$2
		#Disable DVFS first
		disable_dvfs_$platfm
		check_voltage $clk_points_file || exit $RC
	;;
	*)
        usage
        exit 1
	;;
esac