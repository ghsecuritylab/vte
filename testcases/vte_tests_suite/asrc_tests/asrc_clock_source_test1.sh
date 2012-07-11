#!/bin/sh
##############################################################################
#
#  Copyright 2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
##############################################################################
#
#  The code contained herein is licensed under the GNU Lesser General Public
#  License.  You may obtain a copy of the GNU Lesser General Public License
#  Version 2.1 or later at the following locations:
#
#  http://www.opensource.org/licenses/lgpl-license.html
#  http://www.gnu.org/copyleft/lgpl.html
#
##############################################################################
#
# Revision History:
#                      Modification     Tracking
# Author                   Date          Number    Description of Changes
#-------------------   ------------    ----------  ---------------------
# Justin Qiu            09/08/2009        n/a        Initial ver. 
#############################################################################
# Portability:  ARM sh 
#
# File Name:    
# Total Tests:   
# Test Strategy: 
# 
# Input:	- $1 - audio stream
#
# Return:       - 
#
# Use command:  

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
    # Initialize return code to zero.
    RC=0                 # Exit values of system commands used

    export TST_TOTAL=1   # Total number of test cases in this file.
    LTPTMP=${TMP}        # Temporary directory to create files, etc.
    export TCID="TGE_LV_ASRC"       # Test case identifier
    export TST_COUNT=0   # Set up is initialized as test 0
    BIN_DIR=`dirname $0`
    export PATH=$PATH:$BIN_DIR

    if [ -z $LTPTMP ]
    then
        LTPTMP=/tmp
    fi

    trap "cleanup" 0

    if [ ! -e /unit_tests/mxc_asrc_test.out ]
    then
        tst_resm TBROK "ASRC utilities are not ready, pls check..."
        RC=65
        return $RC
    fi

    [ ! -z $STREAM_PATH ] || {
        tst_resm TBROK "STREAM_PATH not set, pls check!" 
        RC=66
        return $RC
    } 
    
    if [ ! -e $STREAM_PATH/alsa_stream/audio44k24S-S24_LE_long.wav ]
    then
        tst_resm TBROK "source audio stream is not ready, pls check..."
        RC=66
        return $RC
    fi

    if [ ! -e $STREAM_PATH/alsa_stream/audio32k16S.wav ]
    then
        tst_resm TBROK "source audio stream is not ready, pls check..."
        RC=66
        return $RC
    fi

    platfm=`platfm.sh`
    eval def_cfg_name="audio_dac_${platfm}.cfg"
    if [ -e /etc/asound/$def_cfg_name ]; then
        CFG_FILE=/etc/asound/$def_cfg_name
    elif [ -e $LTPROOT/testcases/bin/$def_cfg_name ]; then
        CFG_FILE=$LTPROOT/testcases/bin/$def_cfg_name
    else
        tst_resm TWARN "Default config file can't find, will use card 0 as sound card"
    fi

    if [ -e "$CFG_FILE" ]; then
        HW_keyword=`head -n 1 $CFG_FILE`
        STEREO_CARD_NO=`aplay -l |grep card| grep -i "$HW_keyword" |awk '{ print $2 }'|sed 's/://'`
    fi

    [ -z "$STEREO_CARD_NO" ] && STEREO_CARD_NO=0
    echo "stereo device number: ${STEREO_CARD_NO}"

    # cs42888 is for MX6 ARD board
    easi_devices="WM8580 cs42888"
    for dev in $easi_devices; do
        if [ -z "$ESAI_CARD_NO" ]; then
            aplay -l | grep $dev
            if [ $? -eq 0 ]; then
                ESAI_CARD_NO=$(aplay -l | grep $dev | awk '{print $2}' | sed 's/://')
                echo "ESAI card device number: ${ESAI_CARD_NO}"
            fi
        fi
    done

    if [ -z "$ESAI_CARD_NO" ]; then
        echo "No ESAI device, pls check ..."
        RC=66
        return $RC
    fi
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
    return $RC
}

# Function:     test_case_0301()
#
# Description:  -input clock source:     INCLK_NONE
#               -output clock source:    OUTCLK_ESAI_TX
#
# Exit:         - zero on success
#               - non-zero on failure.
#
test_case_0301()
{
    RC=0    # Return value from setup, and test functions.
    aplay -D hw:${ESAI_CARD_NO},0 \
    $STREAM_PATH/alsa_stream/audio44k24S-S24_LE_long.wav -d 15 &
    sleep 5
    rm -f /tmp/asrc.wav
    /unit_tests/mxc_asrc_test.out -to 44100 $STREAM_PATH/asrc_stream/audio32k16S.wav \
    /tmp/asrc.wav 0 1 || RC=$?
    
    check_result $RC

    return $RC
}

# Function:     test_case_0302()
#
# Description:  -input clock source:     INCLK_NONE
#               -output clock source:    OUTCLK_SSI1_TX 
#
# Exit:         - zero on success
#               - non-zero on failure.
#
test_case_0302()
{
    RC=0    # Return value from setup, and test functions.

    aplay -D hw:${STEREO_CARD_NO},0 \
    $STREAM_PATH/alsa_stream/audio44k24S-S24_LE_long.wav -d 15 &
    sleep 5
    rm -f /tmp/asrc.wav
    /unit_tests/mxc_asrc_test.out -to 44100 $STREAM_PATH/asrc_stream/audio32k16S.wav \
    /tmp/asrc.wav 0 2 || RC=$?
    
    check_result $RC

    return $RC
}

# Function:     test_case_030a()
#
# Description:  -input clock source:     INCLK_NONE
#               -output clock source:    OUTCLK_ASRCK1_CLK 
#
# Exit:         - zero on success
#               - non-zero on failure.
#
test_case_030a()
{
    RC=0    # Return value from setup, and test functions.

    rm -f /tmp/asrc.wav
    /unit_tests/mxc_asrc_test.out -to 44100 $STREAM_PATH/asrc_stream/audio32k16S.wav \
    /tmp/asrc.wav 0 10 || RC=$?
    
    check_result $RC

    return $RC
}


# Function:     test_case_0361()
#
# Description:  -input clock source:    OUTCLK_ESAI_TX 
#               -output clock source:   OUTCLK_ESAI_TX     
#
# Exit:         - zero on success
#               - non-zero on failure.
#
test_case_0361()
{
    RC=0    # Return value from setup, and test functions.

    aplay -D plughw:${ESAI_CARD_NO},0 \
    $STREAM_PATH/alsa_stream/audio44k16S_long.wav -d 15 &
    sleep 5
    rm -f /tmp/asrc.wav
    /unit_tests/mxc_asrc_test.out -to 44100 \
    $STREAM_PATH/asrc_stream/audio44k16S.wav /tmp/asrc.wav 6 1 || RC=$?
    
    check_result $RC

    return $RC
}

# Function:     test_case_0362()
#
# Description:  -input clock source:    OUTCLK_ESAI_TX    
#               -output clock source:   OUTCLK_SSI1_TX
#
# Exit:         - zero on success
#               - non-zero on failure.
#
test_case_0362()
{
    RC=0    # Return value from setup, and test functions.

    aplay -D plughw:${ESAI_CARD_NO},0 \
    $STREAM_PATH/alsa_stream/audio44k16S_long.wav -d 15 &
    sleep 2
    aplay -D hw:${STEREO_CARD_NO},0 \
    $STREAM_PATH/alsa_stream/audio32k24S-S24_LE_long.wav -d 15 &
    sleep 5
    rm -f /tmp/asrc.wav
    /unit_tests/mxc_asrc_test.out -to 32000 $STREAM_PATH/asrc_stream/audio44k16S.wav \
    /tmp/asrc.wav 6 2 || RC=$?
    
    check_result $RC

    return $RC
}

# Function:     test_case_036a()
#
# Description:  -input clock source:     OUTCLK_ESAI_TX
#               -output clock source:    OUTCLK_ASRCK1_CLK 
#
# Exit:         - zero on success
#               - non-zero on failure.
#
test_case_036a()
{
    RC=0    # Return value from setup, and test functions.

    aplay -D plughw:${ESAI_CARD_NO},0 \
    $STREAM_PATH/alsa_stream/audio44k16S_long.wav -d 15 &
    sleep 5
    rm -f /tmp/asrc.wav
    /unit_tests/mxc_asrc_test.out -to 32000 $STREAM_PATH/asrc_stream/audio44k16S.wav \
    /tmp/asrc.wav 6 10 || RC=$?
    
    check_result $RC

    return $RC
}

# Function:     test_case_0371()
#
# Description:  -input clock source:     INCLK_SSI1_TX
#               -output clock source:    OUTCLK_ESAI_TX 
#
# Exit:         - zero on success
#               - non-zero on failure.
#
test_case_0371()
{
    RC=0    # Return value from setup, and test functions.
    aplay -D hw:${STEREO_CARD_NO},0 \
    $STREAM_PATH/alsa_stream/audio32k24S-S24_LE_long.wav -d 15 &
    sleep 3
    aplay -D hw:${ESAI_CARD_NO},0 \
    $STREAM_PATH/alsa_stream/audio44k16S_long.wav -d 15 &
    sleep 3
    rm -f /tmp/asrc.wav
    /unit_tests/mxc_asrc_test.out -to 44100 \
    $STREAM_PATH/asrc_stream/audio32k16S.wav /tmp/asrc.wav 7 1 || RC=$?
    
    check_result $RC

    return $RC
}

# Function:     test_case_0372()
#
# Description:  -input clock source:     INCLK_SSI1_TX
#               -output clock source:    OUTCLK_SSI1_TX 
#
# Exit:         - zero on success
#               - non-zero on failure.
#
test_case_0372()
{
    RC=0    # Return value from setup, and test functions.

    aplay -D hw:${STEREO_CARD_NO},0 \
    $STREAM_PATH/alsa_stream/audio44k24S-S24_LE_long.wav -d 15 &
    sleep 3
    rm -f /tmp/asrc.wav
    /unit_tests/mxc_asrc_test.out -to 44100 \
    $STREAM_PATH/asrc_stream/audio44k16S.wav /tmp/asrc.wav 7 2 || RC=$?

    check_result $RC

    return $RC
}

# Function:     test_case_037a()
#
# Description:  -input clock source:     INCLK_SSI1_TX
#               -output clock source:    OUTCLK_ASRCK1_CLK 
#
# Exit:         - zero on success
#               - non-zero on failure.
#
test_case_037a()
{
    RC=0    # Return value from setup, and test functions.

    aplay -D hw:${STEREO_CARD_NO},0 \
    $STREAM_PATH/alsa_stream/audio44k24S-S24_LE_long.wav -d 15 &
    
    sleep 3
    rm -f /tmp/asrc.wav
    
    /unit_tests/mxc_asrc_test.out -to 32000 \
    $STREAM_PATH/asrc_stream/audio44k16S.wav \
    /tmp/asrc.wav 7 10 || RC=$?

    check_result $RC

    return $RC
}

# Function:     test_case_03a1()
#
# Description:  -input clock source:     INCLK_ASRCK1_CLK
#               -output clock source:    OUTCLK_ESAI_TX 
#
# Exit:         - zero on success
#               - non-zero on failure.
#
test_case_03a1()
{
    RC=0    # Return value from setup, and test functions.

    aplay -D plughw:${ESAI_CARD_NO},0 \
    $STREAM_PATH/alsa_stream/audio44k16S_long.wav -d 15&

    sleep 3
    rm -f /tmp/asrc.wav

    /unit_tests/mxc_asrc_test.out -to 44100 \
    $STREAM_PATH/asrc_stream/audio32k16S.wav /tmp/asrc.wav 10 1 || RC=$?

    check_result $RC

    return $RC
}

# Function:     test_case_03a2()
#
# Description:  -input clock source:     INCLK_ASRCK1_CLK
#               -output clock source:    OUTCLK_SSI1_TX 
#
# Exit:         - zero on success
#               - non-zero on failure.
#
test_case_03a2()
{
    RC=0    # Return value from setup, and test functions.

    aplay -D hw:${STEREO_CARD_NO},0 \
    $STREAM_PATH/alsa_stream/audio44k24S-S24_LE_long.wav -d 15 &
    
    sleep 3
    rm -f /tmp/asrc.wav
    
    /unit_tests/mxc_asrc_test.out -to 44100 \
    $STREAM_PATH/asrc_stream/audio32k16S.wav /tmp/asrc.wav 10 2 || RC=$?

    check_result $RC

    return $RC
}

# Function:     test_case_03aa()
#
# Description:  -input clock source:     INCLK_ASRCK1_CLK
#               -output clock source:    OUTCLK_ASRCK1_CLK 
#
# Exit:         - zero on success
#               - non-zero on failure.
#
test_case_03aa()
{
    RC=0    # Return value from setup, and test functions.

    sleep 3
    rm -f /tmp/asrc.wav
    
    /unit_tests/mxc_asrc_test.out -to 32000 \
    $STREAM_PATH/asrc_stream/audio48k16S.wav /tmp/asrc.wav 10 10 || RC=$?

    check_result $RC

    return $RC
}




check_result()
{
    RC=0    # Return value from setup, and test functions.

    if [ $1 -ne 0 ]
    then
        tst_resm TFAIL "Test #1: convert error, please check..."
        return $1
    fi

    tst_resm TINFO "Test #1: wait 10 seconds ..."
    sleep 10
    tst_resm TINFO "Test #1: play the dest audio stream, please check the \
    HEADPHONE, hear if there is voice."
    aplay -D plughw:${STEREO_CARD_NO},0 /tmp/asrc.wav || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: play error, please check..."
        return $RC
    fi

    tst_resm TINFO "Do you hear the voice clearly and smoothly from the headphone?[y/n]"
    read answer
    if [ $answer = "y" ]
    then
        tst_resm TPASS "Test #1: ASRC test success."
    else
        tst_resm TFAIL "Test #1: ASRC test fail"
        RC=67
    fi

    return $RC
}

# Function:     usage
#
# Description:  - display help info.
#
# Return        - none
usage()
{
    cat <<-EOF 

    Use this command to test ASRC clock source functions.
    usage: ./${0##*/} inclk_outckl
    e.g.: ./${0##*/} 0301

EOF
}


# Function:     main
#
# Description:  - Execute all tests, exit with test status.
#
# Exit:         - zero on success
#               - non-zero on failure.
#
RC=0    # Return value from setup, and test functions.

#"" will pass the whole args to function setup()
setup || exit $RC

case "$1" in
0301 | 0302 | 030a| 0361 | 0362 | 036a | 0371 | 0372 | 037a | 03a1 | 03a2 | 03aa)
    test_case_$1 || exit $RC
    ;;
*)
    usage
    ;;
esac

#tst_resm TINFO "Test PASS"
