#!/bin/bash -x
#Copyright 2008-2010, 2012 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
###############################################################################
#
#    @file   ipu_test.sh
#
#    @brief  shell script template for testcase design "IPU" is where to modify block.
#
###############################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#Hake Huang/-----             <20081209>     N/A          Initial version
#Justin Qiu                   <20091201>     N/A          add ipu performance test
#Spring Zhang                 <20100816>     N/A          copy stream to mem
#Andy Tian                    <20120530>     N/A          copy stream to SATA for case_07
#Andy Tian                    <20120530>     N/A          exit if no scsi device for case_07
# 
###############################################################################


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
    RC=1

    trap "cleanup" 0

    #TODO add setup scripts

    OUTPUTFB="ipu0-1st-ovfb"

    FB0NAME=$(cat /sys/class/graphics/fb1/name)

    if [ "$FB0NAME" = "DISP3 FG"  ]; then
        OUTPUTFB="ipu0-1st-ovfb"
    elif [ "$FB0NAME" = "DISP3 BG - DI1" ]; then
        OUTPUTFB="ipu0-2st-fb"
    elif [ "$FB0NAME" = "DISP4 FG" ]; then
        OUTPUTFB="ipu1-1st-ovfb"
    elif [ "$FB0NAME" = "DISP4 BG - DI1" ]; then
        OUTPUTFB="ipu1-2st-fb"
    fi

    echo "display to $OUTPUTFB"

    if [ "$TARGET" = "31" ]
    then
        MLIST="ipu_prp_enc ipu_prp_vf_sdc_bg ov2640_camera ipu_prp_vf_sdc ipu_still mxc_v4l2_capture"
        for i in $MLIST
        do
            modprobe $i
        done
    fi

    if [ "$TARGET" = "35" ]
    then
        MLIST="ipu_prp_enc ipu_prp_vf_sdc_bg ov2640_camera ipu_prp_vf_sdc ipu_still mxc_v4l2_capture"
        for i in $MLIST
        do
            modprobe $i
        done
    fi

    sleep 3

    if [ -e /dev/mxc_ipu ]
    then
        RC=0
    fi
    #keep lcd on
    echo -e "\033[9;0]" > /dev/tty0

    #get fb setting
    FB0XRES=$(fbset | grep geometry | awk '{print $2}')
    FB0YRES=$(fbset | grep geometry | awk '{print $3}')
    FB0BITS=$(fbset | grep geometry | awk '{print $6}')
    FB2XRES=$(fbset -fb /dev/fb2 | grep geometry | awk '{print $2}')
    FB2YRES=$(fbset -fb /dev/fb2 | grep geometry | awk '{print $3}')
    FB2BITS=$(fbset -fb /dev/fb2 | grep geometry | awk '{print $6}')

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

    #ipu mem is write back need sync
    sync
    umount /mnt/msc

    #TODO add cleanup code here

    return $RC
}

check_format_bits()
{
    #FMLIST="RGBP BGR3 RGB3 BGR4 BGRA RGB4 RGBA ABGR YUYV UYVY Y444 NV12 I420 422P YV16"
    FM=8
    ifm=$1
    case $ifm in
    "RGBP")
    FM=16
    ;;
    "YUYV")
    FM=16
    ;;
    "UYVY")
    FM=16
    ;;
    "UYVY")
    FM=16
    ;;
    "I420")
    FM=12
    ;;
    "442P")
    FM=16
    ;;
    "YV16")
    FM=16
    ;;
    "BGR3")
    FM=24
    ;;
    "RGB3")
    FM=24
    ;;
    "Y444")
    FM=24
    ;;
    "NV12")
    FM=12
    ;;
    *)
    FM=32
    ;;
    esac

    return $FM
}

# Function:     test_case_01
# Description   - Test if pattern test ok
#  
test_case_01()
{
    #TODO give TCID 
    TCID="ipu_dev_pattern_test"
    #TODO give TST_COUNT
    TST_COUNT=1
    RC=1

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "
    if [ -e /dev/mxc_ipu ]; then
        RC=0
    fi

    return $RC

}

# Function:     test_case_02
# Description   - Test ipu main display test
#  
test_case_02()
{
    #TODO give TCID 
    TCID="IPU_API_test"
    #TODO give TST_COUNT
    TST_COUNT=2
    RC=0

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    TOTAL=0

    #TODO add function test scripte here
    for CRP in $CROPLIST 
    do
        echo "TST INFO: CROP is $CRP"
        r=0
        ff=I420
        for tf in $FMLIST
        do
            echo "TST INFO: to form is $tf"
            #end for tf
            exec_test
        done
        #end for rotation
        #done
        #end for CRP
    done
    echo "Total cases is $TOTAL, failed cases $RC, $error_cmd_index"
    return $RC

}

# Function:     test_case_03
# Description   - Test if IPU_PP_test ok
#  
test_case_03()
{
    #TODO give TCID 
    TCID="IPU_PP_TEST"
    #TODO give TST_COUNT
    TST_COUNT=3
    RC=0

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "
    TOTAL=0

    for CRP in $CROPLIST 
    do
        echo "TST INFO: CROP is $CRP"
        for r in $ROTATION
        do
            echo "TST INFO: Rotation is $r"
            tf=I420
            ff=I420
            #end for tf
            exec_test
            #end for rotation
        done
        #end for CRP
    done
    echo "Total cases is $TOTAL, failed cases $RC, $error_cmd_index"
    return $RC
}

# Function:     test_case_04
# Description   - Test if IPU_format_test ok
#  
test_case_04()
{
    #TODO give TCID 
    TCID="IPU_Format_TEST"
    #TODO give TST_COUNT
    TST_COUNT=4
    RC=0

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    TOTAL=0
    CRP=0,0,0,0
    r=0
    for i in $FMLIST
    do
        ff=$i
        for j in $FMLIST
        do
            tf=$j
            exec_test
        done
    done
    echo "Total cases is $TOTAL, failed cases $RC, $error_cmd_index"

    return $RC
}

exec_test()
{
    echo "now start test"

    mkdir /tmp/ipu_dev/
    #output 1 enalble
    echo "TST INFO: Format is $i"
    for j in $IN_FILE
    do
        echo "TST INFO: file $j"
        WD=$(echo $j | sed "s/+/ /g" | awk '{print $1}' )
        HT=$(echo $j | sed "s/+/ /g" | awk '{print $2}' )
        INFILE=$(echo $j | sed "s/+/ /g"| awk '{print $3}')

        if [ "$ff" != "I420" ];then
            ${TST_CMD} -p 0 -d 0 -c 1 -l 1 \
                -i ${WD},${HT},I420,0,0,0,0,0,0 \
                -O  ${WD},${HT},${ff},0,0,0,0,0 -s 0\
                -f /tmp/ipu_dev/tmp.dat ${STREAM_PATH}/video/${INFILE}
            TOTAL=$(expr $TOTAL + 1)
        else
            cp ${STREAM_PATH}/video/${INFILE} /tmp/ipu_dev/tmp.dat
        fi
        if [ $? -ne 0 ]; then
            echo "TST ERROR: can not convert from 422P to $i"
        else
            for k in $RESLIST
            do
                echo "TST INFO: output $k"
                w=$(echo $k | sed "s/,/ /g" | awk '{print $1}')
                h=$(echo $k | sed "s/,/ /g" | awk '{print $2}')
                #if [ $w -gt $FB0XRES ] || [ $h -gt $FB0YRES ]; then
                # echo "TST INFO: skip this resolution for fb not support\n"
                # else
                if [ $motion -eq 0 ] && [ $r -eq 0 ] && [ "$tf" = "$ff" ];then
                    echo "no ipu task need"
                else  
                    echo " no motion"
                    ${TST_CMD} -p 0 -d 0 -c 1 -l 1 -i ${WD},${HT},${ff},${CRP},0,0 \
                        -O  ${w},${h},${tf},${r},${CRP} -f $OUTPUTFB -s 1 /tmp/ipu_dev/tmp.dat \
                        || record_result
                    TOTAL=$(expr $TOTAL + 1)
                    if [ $motion -eq 1 ]; then
                        echo "motion_sel = 0(medium_motion)"
                        ${TST_CMD} -p 0 -d 0 -c 1 -l 1 -i ${WD},${HT},${ff},${CRP},1,0 \
                            -O  ${w},${h},${tf},${r},${CRP} -f $OUTPUTFB -s 1 /tmp/ipu_dev/tmp.dat \
                            || record_result
                        TOTAL=$(expr $TOTAL + 1)
                        echo "motion_sel = 1(low_motion)"
                        ${TST_CMD} -p 0 -d 0 -c 1 -l 1 -i ${WD},${HT},${ff},${CRP},1,1 \
                            -O  ${w},${h},${tf},${r},${CRP} -f $OUTPUTFB -s 1 /tmp/ipu_dev/tmp.dat \
                            || record_result
                        TOTAL=$(expr $TOTAL + 1)
                        echo "motion_sel = 2(high_motion)"
                        ${TST_CMD} -p 0 -d 0 -c 1 -l 1 -i ${WD},${HT},${ff},${CRP},1,2 \
                            -O  ${w},${h},${tf},${r},${CRP} -f $OUTPUTFB -s 1 /tmp/ipu_dev/tmp.dat \
                            || record_result
                        TOTAL=$(expr $TOTAL + 1)
                        #motion
                    fi
                fi
                #fi
                #end for k out res
            done
        fi
        #end for j in file
    done
    rm -rf /tmp/ipu_dev

    return $RC
}

# Function:     test_case_05
# Description   - Test if IPU_motion_test ok
#  
test_case_05()
{
    #TODO give TCID 
    TCID="IPU_motion_TEST"
    #TODO give TST_COUNT
    TST_COUNT=5
    RC=0

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    #TODO add function test scripte here
    TOTAL=0
    CRP=0,0,0,0
    r=0
    ff=I420
    tf=I420
    motion=1
    exec_test

    echo "Total cases is $TOTAL, failed cases $RC, $error_cmd_index"

    return $RC
}

# Function:     test_case_06
# Description   - Test if rotation ok
#  
test_case_06()
{
    #TODO give TCID 
    TCID="IPU_LARGE_SIZE_TEST"
    #TODO give TST_COUNT
    TST_COUNT=6
    RC=0

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    #TODO add function test scripte here
	mkdir /tmp/ipu_dev/
    ff=RGBP
	IN_FILE="4080+2720+zero 4096+4096+zero 352+288+COASTGUARD_CIF_IJT.yuv"
    #output 1 enalble
    for j in $IN_FILE
    do
        echo "TST INFO: file $j"
        WD=$(echo $j | sed "s/+/ /g" | awk '{print $1}' )
        HT=$(echo $j | sed "s/+/ /g" | awk '{print $2}' )
        FILE=$(echo $j | sed "s/+/ /g"| awk '{print $3}')
		if [ $FILE = "zero" ]; then
			INFILE="/dev/zero"
		else
			INFILE="${STREAM_PATH}/video/${FILE}"
		fi
        if [ "$ff" != "I420" ];then
            ${TST_CMD} -p 0 -d 0 -c 1 -l 1 \
                -i ${WD},${HT},I420,0,0,0,0,0,0 \
                -O  1024,768,${ff},0,0,0,0,0 -s 0\
                -f /dev/null ${INFILE} || RC=$(expr $RC + 1)
        fi
	done
    return $RC
}

# Function:		u_mount
# Description 	u_mount the device or mount point given by args
#
u_mount()
{
	for mlist in $*; do
		mt=`mount | grep "$mlist"`
		while [ -n "$mt" ]; do
			umount $mlist
			mt=`mount | grep "$mlist"`
		done
	done
}  

p_node()
{
 RC=1
 #get cylinder size
 SZ=$(fdisk -l $1 | grep "Units =" | awk '{print $9}')
 BEGC=$(echo "((10*1024*1024)/${SZ})" | bc)
 let BEGC=BEGC+1
 #set the end cylinder
 ENDC=$(echo "(($2*1024)/${SZ})" | bc)
 let ENDC=ENDC+1
 tmpfile=$(mktemp -p /tmp)
 if [ $? -ne 0 ]; then
  return $RC
 fi
 cat >$tmpfile <<EOF

n
p
1
$BEGC
$ENDC

w
EOF

fdisk $1 < $tmpfile
if [ $? -eq 0 ]; then
	RC=0
fi

sleep 5
rm -f $tmpfile
return $RC
}

# Function:     test_case_07
# Description   - Performance for IPU decoder
#
test_case_07()
{
    #TODO give TCID
    TCID="IPU_Performance_TEST"
    #TODO give TST_COUNT
    TST_COUNT=7
    RC=1

	#find the scsi device
	scsi_atas=`get_scsi.sh 2`
	scsi_usbs=`get_scsi.sh 4`
	if [ -n "$scsi_atas" ]; then
		scsi_dev=$scsi_atas
	elif [ -n "$scsi_usbs" ]; then
		scsi_dev=$scsi_usbs
	    echo "No ata device, use u-disk $scsi_dev"	
	else
		echo "No scsi device, program will run with sd card or memory"
		scsi_dev=""
	fi

	#fdisk,mkfs to scsi device if needed

	mt_pt=''
	tmp_dir=$(mktemp -d -p /mnt)
	for i in $scsi_dev; do
		p_list=`cat /proc/partitions | grep $scsi_dev[0-9].* | awk '{print $4}'`
		if [ -z "$p_list" ]; then
			#no partition table
			p_node /dev/$i 3145728 || continue
			p_list="${i}1"
			mkfs.ext3 /dev/${p_list}
			mt_pt=$p_list
			break
		else
			for pt in $p_list; do
				u_mount $tmp_dir
				mount -t ext3 /dev/$pt $tmp_dir || ( mkfs.ext3 /dev/$pt && mount -t ext3 /dev/$pt $tmp_dir )
				free_size=`df | grep -m 1 $pt | awk '{print $4}'`
				if [ $free_size -gt 1048576 ]; then
					mt_pt=$pt
					break
				fi
			done
		fi
		if [ -n "$mt_pt" ]; then
			break
		fi
	done
	u_mount $tmp_dir
	rm -rf $tmp_dir

    #mount sata
    if [ -n "${mt_pt}" ]; then
		mkdir -p /mnt/msc
		u_mount /mnt/msc /dev/${mt_pt}
		mount /dev/${mt_pt} /mnt/msc
	else
		if [ -e /dev/mmcblk0p1 ]; then
			mount /dev/mmcblk0p1 /mnt/msc || mount -t tmpfs tmpfs /mnt/msc
		fi
	fi

    #print test info
    tst_resm TINFO "test $TST_Count: $TCID "

    TOTAL=0
    #TODO add function test scripts here
    dmesg -c
    IN_FILE="1920+1080+CITY_1920x1080.yuv 1280+720+CITY_1280x720.yuv 352+288+COASTGUARD_CIF_IJT.yuv 640+480+CITY_640x480_30.yuv"
    #IN_FILE="1280+720+CITY_1280x720.yuv 352+288+COASTGUARD_CIF_IJT.yuv 640+480+CITY_640x480_30.yuv"
    fc=300
    FMLIST="I420 BGR3 RGBP RGB3 BGR4 BGRA RGB4 RGBA ABGR YUYV UYVY Y444 NV12 422P YV16"
    for infile in ${IN_FILE}
    do
        echo "TST_INFO: ---------------------------------------"
        echo "TST_INFO: IPU performance test for file ${infile}"

        WD=$(echo $infile | sed "s/+/ /g" | awk '{print $1}' )
        HT=$(echo $infile | sed "s/+/ /g" | awk '{print $2}' )
        infilename=$(echo $infile | sed "s/+/ /g"| awk '{print $3}')
		pstreams=/mnt/msc
		cp ${STREAM_PATH}/video/${infilename} /mnt/msc/ || pstreams=${STREAM_PATH}/video/

        echo "TST_INFO: --------- rotate only test --------------------"
        for r in $ROTATION
        do
            if [ ${HT} -gt 768 ];then
                continue;
            fi
            time -p ${TST_CMD} -c ${fc} -i ${WD},${HT},I420,0,0,0,0,0,0 \
                -O  ${WD},${HT},I420,${r},0,0,0,0 -f $OUTPUTFB -s 1 ${pstreams}/${infilename}
            TOTAL=$(expr $TOTAL + 1)
        done
        echo "*************************************************************"
        echo "TST_INFO: --------- resize only test --------------------"
        for outsize in $RESLIST
        do
            echo "TST INFO: output $outsize"
            out_w=$(echo $outsize | sed "s/,/ /g" | awk '{print $1}')
            out_h=$(echo $outsize | sed "s/,/ /g" | awk '{print $2}')
            time -p ${TST_CMD} -c ${fc}  -i ${WD},${HT},I420,0,0,0,0,0,0 \
                -O  ${out_w},${out_h},I420,0,0,0,0,0 -f $OUTPUTFB -s 1 ${pstreams}/${infilename}
            TOTAL=$(expr $TOTAL + 1)
        done

        echo "*************************************************************"
        for format in ${FMLIST}
        do
            echo "TST_INFO: output format is: ${format}"
            echo "TST_INFO: --------f $OUTPUTFB -single CSC display---------------"
            time -p ${TST_CMD} -c ${fc} -i ${WD},${HT},I420,0,0,0,0,0,0 \
                -O  ${WD},${HT},${format},0,0,0,0,0 -f $OUTPUTFB -s 1 ${pstreams}/${infilename}
            TOTAL=$(expr $TOTAL + 1)
            dmesg -c
            echo "TST_INFO: --------- rotate with CRS test --------------------"
            for r in $ROTATION
            do
                if [ ${HT} -gt 768 ];then
                    continue;
                fi
                time -p ${TST_CMD} -c ${fc} -i ${WD},${HT},I420,0,0,0,0,0,0 \
                    -O  ${WD},${HT},${format},${r},0,0,0,0 -f $OUTPUTFB -s 1 ${pstreams}/${infilename}
                TOTAL=$(expr $TOTAL + 1)
            done
            echo "TST_INFO: ---------CSC crop test------------"
            echo "CSC crop input"
            time -p ${TST_CMD} -c ${fc} -i ${WD},${HT},I420,32,32,64,64,0,0 \
                -O  ${WD},${HT},${format},0,0,0,0,0 -f $OUTPUTFB -s 1 ${pstreams}/${infilename}
            dmesg -c
            TOTAL=$(expr $TOTAL + 1)
            echo "CSC crop output"
            time -p ${TST_CMD} -c ${fc} -i ${WD},${HT},I420,0,0,0,0,0,0 \
                -O  ${WD},${HT},${format},0,32,32,64,64 -f $OUTPUTFB -s 1 ${pstreams}/${infilename}
            dmesg -c
            TOTAL=$(expr $TOTAL + 1)
            #echo "CSC crop input and output"
            #time -p ${TST_CMD} -c ${fc} -i ${WD},${HT},I420,32,32,64,64,0,0 \
            #-O  ${WD},${HT},${format},0,32,32,64,64 -f $OUTPUTFB -s 1 ${STREAM_PATH}/video/${infilename}
            #			dmesg -c
            #				TOTAL=$(expr $TOTAL + 3)
            echo "TST_INFO: --------- CSC resize test --------------------"
            for outsize in $RESLIST
            do
                echo "TST INFO: output $outsize"
                out_w=$(echo $outsize | sed "s/,/ /g" | awk '{print $1}')
                out_h=$(echo $outsize | sed "s/,/ /g" | awk '{print $2}')
                time -p ${TST_CMD} -c ${fc}  -i ${WD},${HT},I420,0,0,0,0,0,0 \
                    -O  ${out_w},${out_h},${format},0,0,0,0,0 -f $OUTPUTFB -s 1 ${pstreams}/${infilename}
                TOTAL=$(expr $TOTAL + 1)
            done
        done
		rm -rf /mnt/msc/${infilename}
    done

    echo "Total cases $TOTAL"
    RC=$?

    return $RC
}

# main function

RC=0
error_cmd_index="Failed cmd index list: "

OUTPUTFB="ipu0-1st-ovfb"
IN_FILE="352+288+COASTGUARD_CIF_IJT.yuv"
# rotation ref:
# 	IPU_ROTATE_NONE = 0,
#	IPU_ROTATE_VERT_FLIP = 1,
#	IPU_ROTATE_HORIZ_FLIP = 2,
#	IPU_ROTATE_180 = 3,
#	IPU_ROTATE_90_RIGHT = 4,
#	IPU_ROTATE_90_RIGHT_VFLIP = 5,
#	IPU_ROTATE_90_RIGHT_HFLIP = 6,
#	IPU_ROTATE_90_LEFT = 7,
#
ROTATION="0 1 2 3 4 5 6 7"
TARGET=
FB_ENABLE=
INPATH=${LTPROOT}/../test_stream/video

# priority ref:
#	IPU_TASK_PRIORITY_NORMAL = 0
#	IPU_TASK_PRIORITY_HIGH = 1
PRILIST="0 1"

# task_id ref:
#	IPU_TASK_ID_ANY	= 0
#	IPU_TASK_ID_VF	= 1
#	IPU_TASK_ID_PP	= 2
IDLIST="0 1 2"

#format list
# fourcc ref:
#	RGB565->RGBP
#	BGR24 ->BGR3
#	RGB24 ->RGB3
#	BGR32 ->BGR4
#	BGRA32->BGRA
#	RGB32 ->RGB4
#	RGBA32->RGBA
#	ABGR32->ABGR
#	YUYV  ->YUYV
#	UYVY  ->UYVY
#	YUV444->Y444
#	NV12  ->NV12
#	YUV420P->I420
#	YUV422P->422P
#	YVU422P->YV16
#
FMLIST="BGR3 RGBP RGB3 BGR4 BGRA RGB4 RGBA ABGR YUYV UYVY Y444 NV12 I420 422P YV16"

# de-interlace motion ref:
#	MEDIUM_MOTION = 0
#	LOW_MOTION = 1
#	HIGH_MOTION = 2
MLIST="0 1 2"

# overlay alpha mode ref:
#	IPU_ALPHA_MODE_GLOBAL =	0
#	IPU_ALPHA_MODE_LOCAL  =	1
ALIST="0 1"

#resolution list for avga vga cif qcif
RESLIST="1920,1080 1280,720 1024,768 160,120"
fc=1
CROPLIST="32,32,64,64"

TST_CMD=/unit_tests/mxc_ipudev_test.out
#TST_CMD=ipu_dev_test

MODE=
CRP=
r=
ff=
tf=
FB0XRES=
FB0YRES=
FB0BITS=
FB2XRES=
FB2YRES=
FB2BITS=
motion=0

usage()
{
    echo "$0 <case ID> "
    echo "1: module exist test"
    echo "2: API test"
    echo "3: API rotation"
    echo "4: API to from format"
    echo "5: API motion test"
    echo "6: IPU LARGE SIZE test"
    echo "7: IPU performance test"
    echo "the iput size and corp mixing need to be round of 8"
}

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
6)
    test_case_06 || exit $RC
    ;;
7)
    test_case_07 || exit $RC
    ;;
*)
    usage
    ;;
esac

tst_resm TINFO "Test PASS"

