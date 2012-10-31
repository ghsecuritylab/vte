#!/bin/bash
# Copyright (C) 2011, 2012 Freescale Semiconductor, Inc. All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
#    @file   display_stress.sh
#
#    @brief  shell script template for testcase design "cpu freq test" is where to modify block.
#
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#Andy Tian                   03/07/2012      N/A         enhance the wait statement, in previous
#                                                        version, wait will always return 0 which 
#                                                        causes case 04&05 always pass
# 



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
export TST_TOTAL=7

export TCID="setup"
export TST_COUNT=0
RC=0

trap "cleanup" 0

#TODO add setup scripts
#export VSALPHA to show normal color in LVDS according to Angolini Daiane
export VSALPHA=1
echo 0 > /sys/class/graphics/fb2/blank
echo 0 > /sys/class/graphics/fb1/blank
echo -e "\033[9;0]" > /dev/tty0
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

run_test()
{
	RC=0
	pCPU=0
	pGPU=0
	pVPU=0
	pIPU=0
	pIO=0
	while [ ! -z "$1" ]; do
		case "$1" in
			'CPU' )
			echo CPU
			core_stress.sh 4 &
			pCPU=$!
			;;
			'GPU' )
			echo GPU
			export FB_FRAMEBUFFER_0=/dev/fb2
			gles_viv.sh 1 &
			pGPU=$!
			;;
			'VPU' )
			echo VPU
			test_case_04 &
			pVPU=$!
			;;
			'IPU' )
			echo IPU
			ipu_test.sh 1 &
			pIPU=$!
			;;
			'IO' )
			echo IO
			storage_all.sh 1 &
			pIO=$!
			;;
			'*' )
			echo "skip"
			;;
		esac
		shift
	done
	if [ $pGPU -ne 0  ]; then
    ret=$(wait $pGPU)
	RC=$(expr $ret + $RC)
	fi
	if [ $pVPU  -ne 0 ]; then
    ret=$(wait $pVPU)
	RC=$(expr $ret + $RC)
	fi
	if [ $pIPU -ne 0 ]; then
    ret=$(wait $pIPU)
	RC=$(expr $ret + $RC)
	fi
	if [ $pIO -ne 0 ]; then
    ret=$(wait $pIO)
	RC=$(expr $ret + $RC)
	fi
	if [  $pCPU -ne 0  ]; then
    kill -9 $pCPU
	fi
	wait
	return $RC
}


run_auto_test_list()
{
   RC=0
   mkfs.vfat /dev/sda1 || return 15
   mkdir -p /media/sda1; mount -t vfat /dev/sda1 /media/sda1 || return $16
	 echo "USB Host test"
	 bonnie\+\+ -d /media/sda1 -u 0:0 -s 500 -r 100 &
	 loop=100
	 while [ $loop -gt 0 ]
		 do
   echo "uart test"
   cat /etc/passwd > /dev/ttymxc0 || return $RC
   echo "ALSA test"
   aplay -vv $STREAM_PATH/alsa_stream/audio44k16M.wav || return 14
	 arecord -D plughw:0 -f S16_LE -r 44100 -c 2 -traw -d 5 | aplay -D plughw:0 -f S16_LE -r 44100 -c 2 || RC='$RC 1'  || return 141
	  loop=$(expr $loop - 1)
	 done
	 wait
	 RC=$?
	 return $RC
}

# Function:     test_case_01
# Description   - Test if <CPU freq> ok
#  
test_case_01()
{
#TODO give TCID 
TCID="test_clock_stress"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

echo 0 > /sys/class/graphics/fb0/blank
echo 0 > /sys/class/graphics/fb1/blank
echo 0 > /sys/class/graphics/fb2/blank

echo -e "\033[9;0]" > /dev/tty0

#test list
run_test GPU VPU IO CPU
RC=$?
return $RC
}

# Function:     test_case_02
# Description   - Test if bus stress test ok
#  
test_case_02()
{
#TODO give TCID 
TCID="bus_test"
#TODO give TST_COUNT
TST_COUNT=2
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
echo 0 > /sys/class/graphics/fb0/blank
echo 0 > /sys/class/graphics/fb2/blank
echo -e "\033[9;0]" > /dev/tty0


run_test GPU VPU IO
RC=$?

return $RC
}

# Function:     test_case_03
# Description   - Test if multi-stress ok
#  
test_case_03()
{
#TODO give TCID 
TCID="multi_stress_test"
#TODO give TST_COUNT
TST_COUNT=3
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
   echo "ADC DAC test"
	 dac_test1.sh -f $STREAM_PATH/alsa_stream/audio16k16M.wav -A
	 sh -c "arecord -D plughw:0 -f S16_LE -r 44100 -c 2 -traw | aplay -D plughw:0 -f S16_LE -r 44100 -c 2 || RC='$RC 1' &" 
	 sleep 2
	 echo "USB Host test"
   sh -c "mkfs.vfat /dev/sda1 && mkdir -p /media/sda1; mount -t vfat /dev/sda1 /media/sda1 && bonnie\+\+ -d /media/sda1 -u 0:0 -s 10 -r 5 && dt of=/media/sda1/test_file bs=4k limit=128m passes=20 || RC='$RC 2' &"
	 echo "SD test"
	 sleep 2
	 sh -c "mkdir -p /mnt/mmc && mkfs.vfat /dev/mmcblk0p1 && mount /dev/mmcblk0p1 /mnt/mmc && bonnie\+\+ -d /mnt/mmc -u 0:0 -s 10 -r 5 && dt of=/mnt/mmc/test_file bs=4k limit=128m passes=20 || RC='$RC 3' &"
	 echo "epdc test"
	 sh -c "epdc_test -T 7 || RC='$RC 4' &"
	 sleep 2
	 echo "wifi test"
	 sh -c "modprobe ath6kl ; sleep 10; ifconfig wlan0 up && iwconfig wlan0 mode managed && sleep 5 &&iwlist wlan0 scanning | grep FSLLBGAP_001 && iwconfig wlan0 key $(echo Happy123 | md5sum | cut -c 1-10) && iwconfig wlan0 essid FSLLBGAP_001 && sleep 5 && udhcpc -i wlan0"
	 export LOCALIP=$(ifconfig wlan0 | grep inet |  cut -d: -f 2 | awk '{print $1}')
   cd ${LTPROOT}/testcases/bin
	 if [ ! -z $LOCALIP ];then
	 sh -c "tcp_stream_2nd_script 10.192.225.222 CPU $LOCALIP || RC='$RC 5' &"
	 fi
   echo "gpu test"
	 modprobe galcore
	 #sh -c "gpu_test.sh 2 || RC='$RC 6' &"
	 sh -c "tiger &"
  read -p "use Ctrl+c to quit"
return $RC
}

# Function:     test_case_04
# Description   - Test if dual 1080P ok
#  
test_case_04()
{
#TODO give TCID 
TCID="test_dual_1080P_test"
#TODO give TST_COUNT
TST_COUNT=4
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

#a_stream_path=/mnt/nfs/test_stream/power_stream/ToyStory3_H264HP_1920x1080_10Mbps_24fps_AAC_48kHz_192kbps_2ch.mp4
#b_stream_path=/mnt/nfs/test_stream/power_stream/V031204_Times_Square_traffic_1920x1088p30_20Mbps_600frm_H264_noaudio.mp4
a_stream_path=/mnt/nfs/test_stream/video/ToyStory3_H264HP_1920x1080_10Mbps_24fps_AAC_48kHz_192kbps_2ch_track1.h264
b_stream_path=/mnt/nfs/test_stream/video/Mpeg4_SP3_1920x1080_23.97fps_9760kbps_AACLC_44KHz_2ch_track1_track1.cmp

/unit_tests/mxc_vpu_test.out -D "-f 2 -i ${a_stream_path} -a 100" &
pid1=$!

if [ -e /sys/devices/platform/mxc_v4l2_output.0/video4linux/video19 ]; then
	/unit_tests/mxc_vpu_test.out -D "-f 0 -i ${b_stream_path} -x 19 -a 100" &
	pid2=$!
elif [ -e /sys/devices/platform/mxc_v4l2_output.0/video4linux/video18 ]; then
	/unit_tests/mxc_vpu_test.out -D "-f 0 -i ${b_stream_path} -x 18 -a 100" &
	pid2=$!
fi

#pass pid to wait otherwise wait will return 0 always
wait $pid2 $pid1

RC=$?

return $RC

}

# Function:     test_case_05
# Description   - Test if 1080P playback + 720P enc ok
#  
test_case_05()
{
#TODO give TCID 
TCID="test_1080DEC_720ENC_test"
#TODO give TST_COUNT
TST_COUNT=5
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
cat /sys/devices/system/cpu/cpu0/cpufreq/stats/time_in_state
a_stream_path=/mnt/nfs/test_stream/video/H264_ML_1920x1080_10Mbps_15fps_noaudio.h264
/unit_tests/mxc_vpu_test.out -D "-f 2 -i ${a_stream_path} -w 1920 -h 1080 -a 100" || RC=1
cat /sys/devices/system/cpu/cpu0/cpufreq/stats/time_in_state

#pass pid to wait otherwise wait will return 0 always

return $RC

}

# Function:     test_case_06
# Description   - Test if 720P camera loopback test enc ok
#  
test_case_06()
{
#TODO give TCID 
TCID="test_720CLOOPBACK_test"
#TODO give TST_COUNT
TST_COUNT=5
RC=0

v4l_module.sh setup

/unit_tests/mxc_vpu_test.out -L "-f 2 -w 1280 -h 720 -t 1 -x 17 -a 100" &
pid=$!
a_stream_path=/mnt/nfs/test_stream/video/ToyStory3_H264HP_1920x1080_10Mbps_24fps_AAC_48kHz_192kbps_2ch_track1.h264
/unit_tests/mxc_vpu_test.out -D "-f 2 -i ${a_stream_path} -x 16 -a 100" || RC=1
sleep 15
kill -9 $pid


v4l_module.sh cleanup

return $RC
}

# Function:     test_case_07
# Description   - Test if 1080P overnight playback
#  
test_case_07()
{
#TODO give TCID 
TCID="test_1080PLAYBACK_test"
#TODO give TST_COUNT
TST_COUNT=7
RC=0


#a_stream_path=/mnt/nfs/test_stream/video/ToyStory3_H264HP_1920x1080_10Mbps_24fps_AAC_48kHz_192kbps_2ch_track1.h264
a_stream_path=/mnt/nfs/test_stream/video/H264_ML_1920x1080_10Mbps_15fps_noaudio.h264

#start_date=$(date +%d)
#now_date=$(date +%d)
#while [ $now_date -le $start_date ]; do
i=0
while [ $i -lt 250 ]; do		# it takes 2 mins for 1 loop, 250 times takes around 8 hours.
    /unit_tests/mxc_vpu_test.out -D "-f 2 -y 1 -i ${a_stream_path} -a 100" || RC=$(expr $RC + 1) 
#    now_date=$(date +%d)
	i=$(expr $i + 1)
done

return $RC
}

# Function:     test_case_08
# Description   - Test if dual 720P overnight playback
#  
test_case_08()
{
#TODO give TCID 
TCID="test_720PLAYBACK_test"
#TODO give TST_COUNT
TST_COUNT=8
RC=0

STREAM=/mnt/nfs/test_stream/video
a_stream=${STREAM}/h264_bp_l31_mp3_1280x720_30fps_3955kbps_a_48khz_64kbps_stereo_broken-ntsc_tvc_video.h264

/unit_tests/mxc_vpu_test.out -D "-f 2 -i ${a_stream} -a 100" &
pid1=$!

if [ -e /sys/devices/platform/mxc_v4l2_output.0/video4linux/video19 ]; then
	/unit_tests/mxc_vpu_test.out -D "-f 2 -i ${a_stream} -x 19 -a 100" &
	pid2=$!
elif [ -e /sys/devices/platform/mxc_v4l2_output.0/video4linux/video18 ]; then
	/unit_tests/mxc_vpu_test.out -D "-f 2 -i ${a_stream} -x 18 -a 100" &
	pid2=$!
fi

#pass pid to wait otherwise wait will return 0 always
wait $pid2 $pid1 || RC=$?

return $RC
}

# Function:     test_case_10
# Description   - Test if dual 1080P playback overnight with vpu
#  
test_case_10()
{
#TODO give TCID 
TCID="test_dual_1080P_10hr"
#TODO give TST_COUNT
TST_COUNT=4
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

loop=100

while [ $loop -gt 0 ]; do

vpu_performance.sh 5 &

if [ -e /sys/devices/platform/mxc_v4l2_output.0/video4linux/video19 ]; then
	vpu_performance.sh 5 "-x19" &
	pid2=$!
elif [ -e /sys/devices/platform/mxc_v4l2_output.0/video4linux/video18 ]; then
	vpu_performance.sh 5 "-x18" &
	pid2=$!
fi

#pass pid to wait otherwise wait will return 0 always
wait $pid2 $pid1

RC=$(expr $RC + $? )

loop=$(expr $loop - 1)

done

return $RC

}



usage()
{
echo "$0 [case ID]"
echo "1: "
echo "2: "
echo "3: "
echo "4: "
echo "5: "
echo "6: "
echo "7: "
echo "8: "
echo "9: [GPU] [VPU] [IO] [CPU] [IPU]"
echo "10: "
}

# main function

RC=0

#TODO check parameter
if [ $# -lt 1 ]
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
6)
  test_case_06 || exit $RC
  ;;
7)
  test_case_07 || exit $RC
  ;;
8)
  test_case_08 || exit $RC
  ;;
9)
  run_test $@ || exit $RC
  ;;
10)
  test_case_10 || exit $RC
  ;;
*)
  usage
  ;;
esac

 echo "Test PASS"
