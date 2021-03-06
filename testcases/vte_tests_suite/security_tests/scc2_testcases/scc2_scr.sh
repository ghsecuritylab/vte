#Copyright (C) 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
################################################################################################### 
# 
#    @file   scc_scr.sh
# 
#    @brief  xxx
# 
################################################################################################### 
#Revision History: 
#                            Modification     Tracking 
#Author/core ID                  Date          Number    Description of Changes 
#-------------------------   ------------    ----------  -------------------------------------------
#S.Zavjalov/zvjs001c          23/11/2004     TLSbo39738  Initial version 
#A.Ozerov/b00320              23/11/2006     TLSbo80386  logical error was fixed.
# 
###################################################################################################

#!/bin/sh  

LTPROOT=$PWD

cd $LTPROOT

TESTAPPSCC=scc2_test

TMP_RC=0
rc=0
res=0
anal_res()
{
        TMP_RC=$?
        echo""
        if [ $TMP_RC -eq 0 ];
        then
               echo " testcase      RESULT   Exit value"
               echo " ----------- --------- -----------"
               echo " scc2_test      TPASS       0"
        else
               rc=1
               res=1
               echo " testcase      RESULT   Exit value"
               echo " ----------- --------- -----------"
               echo " scc2_test      TFAIL       1"
        fi
               echo""                             
}

anal_res_inv()
{
        TMP_RC=$?
        echo""
        if [ $TMP_RC -eq 0 ];
        then
               echo " testcase      RESULT   Exit value"
               echo " ----------- --------- -----------"
               echo " scc2_test      TPASS       0"
        else
               rc=1
               echo " testcase      RESULT   Exit value"
               echo " ----------- --------- -----------"
               echo " scc2_test      TFAIL       1"
        fi
               echo""
}

insmod $LTPROOT/testcases/bin/scc2_test_module.ko || exit 1
echo "read register"
echo "================"
$TESTAPPSCC -R 0x0c
anal_res                          
echo "AIC test"
echo "================"
$TESTAPPSCC -Lc
anal_res                          
echo "get scc2 configuration"
echo "================"
$TESTAPPSCC -LC
anal_res                          
echo "encryption and decryption"
echo "================"
$TESTAPPSCC -Le
anal_res_inv
echo "test timer"
echo "================"
$TESTAPPSCC -T -Lt
anal_res_inv
echo "signal a software alaim to the scc2"
echo "================"
$TESTAPPSCC -La
anal_res_inv

echo""
echo " final script      RESULT   "
echo " ------------ ----------------"
 
if [ $rc -eq 0 ];
then
        echo " scc2_scr.sh        TPASS    "
else
        echo " scc2_scr.sh        TFAIL    "
fi

if [ $res -eq 0 ];
then
        echo""
        echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!I!!"
        echo "! It is nessesary to reboot the EVB before running other tests !"
        echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!I!!"
fi
        echo""
 
exit $rc
