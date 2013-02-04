#!/bin/bash -x
# Copyright (C) 2011 Freescale Semiconductor, Inc. All Rights Reserved.
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
#    @file   storage_mx53.sh
#
#    @brief  shell script template for "storage".
#
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#Hake                         2011/03/17        NA        init multi-device test
#-------------------------   ------------    ----------  -------------------------------------------
#

p_node()
{
 RC=1
 SZ=$(fdisk -lu $1 | grep "Units =" | awk '{print $9}')
 SHIFT=$(echo "((10*1024*1024*10)/${SZ}+9)/10" | bc)
 SHIFT=$(expr $SHIFT + 1)
 tmpfile=$(mktemp -p /tmp)
 if [ $? -ne 0 ]; then
  return $RC
 fi
 cat >$tmpfile <<EOF

n
p
1
$SHIFT


w
EOF

fdisk -u $1 < $tmpfile
if [ $? -eq 0 ]; then
	RC=0
fi

sleep 5
rm -f $tmpfile
return $RC

}


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

soc=$(platfm.sh)
if [ "$soc" = "IMX6-SABREAUTO" ] || [ "$soc" = "IMX6ARM2" ]; then
    #enable ata module
    modprobe ahci_platform
    sleep 3
fi

clear
tmp_dir=$(mktemp -d -p /mnt)
if [ $? -ne 0 ];then
	RC=1
	echo "can not mktemp folder"
	return $RC
fi
#start detect devices
for i in $scsi_device_list
do
  #1. find devices node
  d_list=$(cat /proc/partitions | grep -i $i | awk '{print $4}'| grep -vi "${i}[0-9]\+")
  #2. deteminate partition
	for j in $d_list
	do
		p_list=$(cat /proc/partitions |grep -i "${j}.*[0-9]\+" \
		| awk '{print $4}'| grep -i "${i}[0-9]\+")
		for k in $p_list
		do
			#check whether mount
	 		mount | grep $k
	 		if [ $? -ne 0 ]; then
     		echo "not mount then try mount if fail will format to ext3"
				echo "then try mount again, if still fail then quit"
				mount /dev/$k $tmp_dir || mkfs.ext3 /dev/$k || break
				sleep 2
				umount $tmp_dir
				sleep 2
			fi
      echo "$k mounted now check free space"
			echo "..."
			mount /dev/$k $tmp_dir
      free_size=$(df -m /dev/$k | tail -1 | awk '{print $4}')
#if free size is enough then add to target list
			if [ $free_size -gt 100 ];then
        target_list=$target_list" "$k
      else
				 echo "$k free space is not enough for test"
				 echo "skip it!"
			fi
			umount $tmp_dir
		done
    if [ $(echo $p_list | wc -w) -eq 0 ]; then
#no partition then partition it to 1 partition
       p_node /dev/$j || break
			 target_list=$target_list" "${j}1
		fi
	done
done


#on_chip_controller_device_list="mmcblk.*[0-9]"
for i in $on_chip_controller_device_list
do
  #1. find devices node
  d_list=$(cat /proc/partitions | grep -i $i | awk '{print $4}'| grep -vi "${i}p")
  #2. deteminate partition
	for j in $d_list
	do
		p_list=$(cat /proc/partitions | awk '{print $4}' | grep -i "${j}p")
		for k in $p_list
		do
			#check whether mount
	 		mount | grep $k
	 		if [ $? -ne 0 ]; then
     		echo "not mount then try mount if fail will format to ext3"
				echo "then try mount again, if still fail then quit"
				mount /dev/$k $tmp_dir || mkfs.ext3 /dev/$k || break
				sleep 2
				umount $tmp_dir
				sleep 2
			fi
      		echo "$k mounted now check free space"
			echo "..."
			mount /dev/$k $tmp_dir
			if [ $? -ne 0 ]; then
				echo "$k can not mount"
				echo "skip it!"
			else
      			free_size=$(df -m /dev/$k | tail -1 | awk '{print $4}')
#if free size is enough then add to target list
				if [ $free_size -gt 100 ];then
        			target_list=$target_list" "$k
      			else
				 	echo "$k free space is not enough for test"
				 	echo "skip it!"
				fi
			fi
			umount $tmp_dir
		done
    	if [ $(echo $p_list | wc -w) -eq 0 ]; then
#no partition then partition it to 1 partition
       		p_node /dev/$j || break
			target_list=$target_list" "${j}p1
		fi
	done
done

rm -rf $tmp_dir
echo $target_list
RC=0

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
if [ "$soc" = "IMX6-SABREAUTO" ] || [ "$soc" = "IMX6ARM2" ]; then
    #enable ata module
    modprobe -r ahci_platform
    sleep 3
fi

return $RC
}

run_single_test_list()
{
	RC=0
	need_umount=1
	 for i in $target_list
	 do
    #test if already mout
    	mount | grep $i
		if [ $? -eq 0 ]; then
			#is mounted
			mount_point=$(mount | grep $i |cut -d" " -f 3)
		 	need_umount=0
    	else
     		#not mount
     		mount_point=$(mktemp -d -p /tmp)
			mount /dev/$i $mount_point
			if [ $? -ne 0 ]; then
				RC=$(echo $RC m$i)
				continue
			fi
     		need_umount=1
		fi
		for j in $mount_point
		do
         free_size=`get_free_size_mb $j`
	 	 bonnie\+\+ -d $j -u 0:0 -s $free_size  -r 100 || RC=$(echo $RC b$i)
		 dt of=$j/test_file bs=4k limit=${free_size}m passes=10 || RC=$(echo $RC d$i)
		 if [ $need_umount -eq 1  ];then
      		umount $mount_point || RC=$(echo $RC u$i)
			rm -rf $mount_point
		 fi
		 break
		done
	 done
	 if [ "$RC" != "0"  ];then
	 echo $RC
	 RC=1
	 fi
	 return $RC
}

run_iozone_test_list()
{
   RC=0
   need_umount=1
	 for i in $target_list
	 do
    #test if already mout
	if [ ! -z "$1" ]; then
		meet=$(echo $i | grep $1 | wc -l)
		if [ $meet -eq 0 ];then
	  		continue;
		fi
	fi
    mount | grep $i
		if [ $? -eq 0 ]; then
     		#is mounted
		 	mount_point=$(mount | grep $i |cut -d" " -f 3)
		 	need_umount=0
    	else
     		#not mount
     		mount_point=$(mktemp -d -p /tmp)
			mount /dev/$i $mount_point
     		if [ $? -ne 0 ]; then
				RC=$(echo $RC m$i)
				continue
			fi
			need_umount=1
		fi
		for j in $mount_point
		do
		 free_size=$(df -m $j | tail -1 | awk '{print $4}')
		 free_size=$(expr $free_size - 50)
	 	 iozone -a -n $free_size -g $free_size -i 0 -i 1 -f $j/iozone.tmpfile || RC=$(echo $RC i$i)
		 if [ $need_umount -eq 1  ];then
      		umount $mount_point || RC=$(echo $RC u$i)
			rm -rf $mount_point
		 fi
		 break
		done
	 done
	 if [ "$RC" != "0"  ];then
	 echo $RC
	 RC=1
	 fi
	 return $RC
}


# Function:     test_case_01
# Description   - Test if single ok
#
test_case_01()
{
#TODO give TCID
TCID="test_storage_single"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

#test list
run_single_test_list

return $RC

}


# Function:     test_case_02
# Description   - Test if single ok
#
test_case_02()
{
#TODO give TCID
TCID="test_storage_single"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

#test list
loop=1000
while [ $loop -gt 0 ] && [ $RC -eq 0 ]; do
run_iozone_test_list $1 || RC=1
loop=$(expr $loop - 1)
done

return $RC

}

usage()
{
echo "$0 [case ID]"
echo "1: full patrition read/write test"
echo "2: full patrition iozone test"
}

# main function

RC=0

scsi_device_list="sd.*"
on_chip_controller_device_list="mmcblk.*[0-9]"

target_list=""

#TODO check parameter

setup || exit $RC

source `which api_storage`

case "$1" in
1)
  test_case_01 || exit $RC
  ;;
2)
  test_case_02 $2 || exit $RC
  ;;
*)
  usage
  ;;
esac

tst_resm TINFO "Test PASS"
