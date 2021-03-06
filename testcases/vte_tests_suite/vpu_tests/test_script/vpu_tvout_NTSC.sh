#Copyright (C) 2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#
#!/bin/sh
###################################################################################################
#
#    @file   mx37_vpu_dec_test.sh
#
#    @brief  shell script for testcase design for VPU decode
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#<Hake Huang>/-----             <2008/11/14>     N/A          Initial version
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
export TST_TOTAL=8

export TCID="setup"
export TST_COUNT=0
RC=1

trap "cleanup" 0

#TODO add setup scripts
if [ -e /dev/mxc_vpu ]
then
RC=0
fi

if [ $TARGET = "37" ]
then
if [ -e /sys/class/regulator/regulator_1_SW2/uV ]
then
echo 1100 > /sys/class/regulator/regulator_1_SW2/uV
fi
fi

if [ $TARGET = "37" ] || [ $TARGET = "51"  ]
then
 echo 1 > /proc/sys/vm/lowmem_reserve_ratio
fi

export VPU_TV_MODE=NTSC


sleep 1
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
if [ $TARGET = "37" ]
then
if [ -e /sys/class/regulator/regulator_1_SW2/uV ]
then
echo 1200 > /sys/class/regulator/regulator_1_SW2/uV
fi
fi

export VPU_TV_MODE=

cd $LTPROOT
return $RC
}

check_platform()
{
LOCAL=0
if [ $LOCAL -eq 1 ]; then
PLATFORM="31 35 37 51"
#  CPU_REV=$(cat /proc/cpuinfo | grep "Revision")
  CPU_REV=$(platfm.sh)
  for i in $PLATFORM
  do
    find=$(echo $CPU_REV | grep $i | wc -l )
    if [ $find -ne 0 ]
    then
     TARGET=$i
    fi
  done
else
 platfm.sh
 TARGET=$?
fi
}

# Function:     test_case_01
# Description   - Test if MPEG2 decode ok
#  
test_case_01()
{
#TODO give TCID 
TCID="vpu_MPEG2_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

cp ${STREAM_PATH}/video/mpeg2_720x576.mpg /tmp
cd /tmp 
#vpu_testapp -C ${LTPROOT}/testcases/bin/config_dec_mpeg2 
${TSTCMD} -D "-i /tmp/mpeg2_720x576.mpg -f 4" || return $RC 
rm -rf /tmp/mpeg2_720x576.mpg
RC=0
return $RC
}

# Function:     test_case_02
# Description   - Test if  vc1 decode ok
#  
test_case_02()
{
#TODO give TCID 
TCID="vpu_dec_vc1_test"
#TODO give TST_COUNT
TST_COUNT=2
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

cp ${STREAM_PATH}/video/SD720x480.vc1.rcv /tmp
cd /tmp 
#vpu_testapp -C ${LTPROOT}/testcases/bin/config_dec_vc1  
${TSTCMD} -D "-i /tmp/SD720x480.vc1.rcv -f 3" || return $RC  
rm -rf /tmp/SD720x480.vc1.rcv
RC=0
return $RC
}

# Function:     test_case_03
# Description   - Test if decode Divx ok
#  
test_case_03()
{
#TODO give TCID 
TCID="vpu_dec_divx_test"
#TODO give TST_COUNT
TST_COUNT=3
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
cp ${STREAM_PATH}/video/divx311_320x240.avi /tmp
cd /tmp 
#vpu_testapp -C ${LTPROOT}/testcases/bin/config_dec_divx 
${TSTCMD} -D "-i /tmp/divx311_320x240.avi -f 5" || return $RC  
rm -rf /tmp/divx311_320x240.avi

RC=0

return $RC
}

# Function:     test_case_04
# Description   - Test if decode MPEG4 ok
#  
test_case_04()
{
#TODO give TCID 
TCID="vpu_dec_mpeg4_test"
#TODO give TST_COUNT
TST_COUNT=4
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
cp ${STREAM_PATH}/video/akiyo.mp4 /tmp
cd /tmp 
#vpu_testapp -C ${LTPROOT}/testcases/bin/config_dec_mpeg4
${TSTCMD} -D "-i /tmp/akiyo.mp4 -f 0" || return $RC
rm -rf /tmp/akiyo.mp4

RC=0
return $RC
}

# Function:     test_case_05
# Description   - Test if decode H263 ok
#  
test_case_05()
{
#TODO give TCID 
TCID="vpu_dec_h263-IJK"
#TODO give TST_COUNT
TST_COUNT=5
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
cp ${STREAM_PATH}/video/COASTGUARD_CIF_IJT.263 /tmp
cd /tmp 
#vpu_testapp -C ${LTPROOT}/testcases/bin/config_dec_h263
${TSTCMD} -D "-i /tmp/COASTGUARD_CIF_IJT.263 -f 1" || return $RC
rm -rf /tmp/COASTGUARD_CIF_IJT.263

RC=0
return $RC

}

# Function:     test_case_06
# Description   - Test if decode H263 with short head ok
#  
test_case_06()
{
#TODO give TCID 
TCID="vpu_dec_h263-head"
#TODO give TST_COUNT
TST_COUNT=6
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
# main function
cp ${STREAM_PATH}/video/cif.263 /tmp
cd /tmp 
#vpu_testapp -C ${LTPROOT}/testcases/bin/config_dec_h263-2
${TSTCMD} -D " -i /tmp/cif.263 -f 1" || return $RC
rm -rf /tmp/cif.263 

RC=0

return $RC
}

# Function:     test_case_07
# Description   - Test if decode H264 HP with short head ok
#  
test_case_07()
{
#TODO give TCID 
TCID="vpu_dec_h264HP_test"
#TODO give TST_COUNT
TST_COUNT=7
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
# main function
cp ${STREAM_PATH}/video/HPCV_BRCM_A.264 /tmp
cd /tmp 
#vpu_testapp -C ${LTPROOT}/testcases/bin/config_dec_h264-hp 
${TSTCMD} -D "-i /tmp/HPCV_BRCM_A.264 -f 2" || return $RC
rm -rf /tmp/HPCV_BRCM_A.264 

RC=0
return $RC
}

# Function:     test_case_08
# Description   - Test if decode H264 BP with short head ok
#  
test_case_08()
{
#TODO give TCID 
TCID="vpu_dec_h264BP_test"
#TODO give TST_COUNT
TST_COUNT=8
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
# main function
cp ${STREAM_PATH}/video/starwars640x480.264 /tmp
cd /tmp 
#vpu_testapp -C ${LTPROOT}/testcases/bin/config_dec_h264-bp 
${TSTCMD} -D "-i /tmp/starwars640x480.264 -f 2" || return $RC 
rm -rf starwars640x480.264

RC=0
return $RC
}

# Function:     test_case_09
# Description   - Test if decode MPEG4 with deblocking ok
#  
test_case_09()
{
#TODO give TCID 
TCID="vpu_dec_mpeg4_deblock_test"
#TODO give TST_COUNT
TST_COUNT=9
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
# main function
cp ${STREAM_PATH}/video/akiyo.mp4 /tmp
cd /tmp 
#vpu_testapp -C ${LTPROOT}/testcases/bin/config_dec_mpeg4
${TSTCMD} -D "-i /tmp/akiyo.mp4 -f 0" || return $RC
rm -rf /tmp/akiyo.mp4

RC=0
return $RC
}

# Function:     test_case_10
# Description   - Test if decode H263 basic
#  
test_case_10()
{
#TODO give TCID 
TCID="vpu_dec_h263-basic"
#TODO give TST_COUNT
TST_COUNT=10
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
# main function
cp ${STREAM_PATH}/video/stream.263 /tmp
cd /tmp 
#vpu_testapp -C ${LTPROOT}/testcases/bin/config_dec_mpeg4
${TSTCMD} -D "-f 1 -i /tmp/stream.263" || return $RC
rm -rf /tmp/stream.263

RC=0
return $RC
}


# Function:     test_case_11
# Description   - Test if MPEG2 deblock ok
#  
test_case_11()
{
#TODO give TCID 
TCID="vpu_MPEG2_deblock_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

#vpu_testapp -C ${LTPROOT}/testcases/bin/config_dec_mpeg2 
${TSTCMD} -D "-i ${STREAM_PATH}/video/mpeg2_720x576.mpg -f 4 -d 1" || return $RC

RC=0
return $RC
}



usage()
{
echo "usage $0 <1/2/3/4/5/6/7/8/9/10/11>"
echo "1: MPEG2 decoder test"
echo "2: VC-1 decoder test"
echo "3: Divx decoder test"
echo "4: MPEG4 decoder test"
echo "5: H263-P3 decoder test"
echo "6: H263+header decoder test"
echo "7: H264 HP decoder test"
echo "8: H264 BP decoder test"
echo "9: MPEG4 decoder+ deblock test"
echo "10: H263 basic test"
echo "11: MPEG2 decoder + deblock test"
}

#TODO check parameter
if [ $# -ne 1 ]
then
echo "usage $0 <1/2/3/4/5/6/7/8/9/10/11>"
usage
exit 1 
fi


TARGET=

check_platform

if [ -z $TARGET ]
then
echo "unknow target"
exit 1
fi

TSTCMD="/unit_tests/mxc_vpu_test.out"

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
  test_case_09 || exit $RC
  ;;
10)
  test_case_10 || exit $RC
  ;;
11)
  test_case_11 || exit $RC
  ;;
*)
#TODO check parameter
  usage
  ;;
esac

tst_resm TINFO "Test PASS"







