#!/bin/sh
#Copyright (C) 2008,2011 Freescale Semiconductor, Inc. All Rights Reserved.
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
#                      Modification     Tracking
# Author                   Date          Number    Description of Changes
#-------------------   ------------    ----------  ---------------------
# Spring Zhang          03/11/2008        n/a        Initial ver. 
# Spring                28/11/2008       n/a        Modify COPYRIGHT header
# Spring                16/02/2011       n/a        Auto check audio stream
#                                                   parameters
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

    if [ $# -ne 3 ] 
    then
        usage
        exit 1
    fi

    SRC_FILE=$3

    trap "cleanup" 0

    if [ ! -e /unit_tests/mxc_asrc_test.out ]
    then
        tst_resm TBROK "ASRC utilities are not ready, pls check..."
        RC=65
        return $RC
    fi

    [ ! -z "$STREAM_PATH" ] || {
        tst_resm TBROK "STREAM_PATH not set, pls check!" 
        RC=66
        return $RC
    } 

    if [ ! -e $SRC_FILE ]
    then
        tst_resm TBROK "source audio stream is not ready, pls check..."
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
    rm play.info
    return $RC
}

# Function:     asrc_convert()
#
# Description:  - asrc test, convert source stream to dest, and play dest
#                 stream.
#
# Exit:         - zero on success
#               - non-zero on failure.
#
asrc_convert()
{
    RC=0    # Return value from setup, and test functions.

    SAM_FREQ=$2

    /unit_tests/mxc_asrc_test.out $@ /dev/target.wav ||RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "convert error, please check..."
        return $RC
    fi

    tst_resm TINFO "play the dest audio stream, please check the \
HEADPHONE, hear if there is voice."
    aplay -N -M /dev/target.wav 2> play.info || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "converted stream play error, please check..."
        return $RC
    fi

    grep $SAM_FREQ play.info > /dev/null || RC=$?
    [ $RC -eq 0 ] || {
        tst_resm TFAIL "wrong sampling rate in coverted stream"
    }

    tst_resm TPASS "Playback finished."

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

    Use this command to test ASRC functions.
    usage: ./${0##*/} -to [destination sample rate] [source stream]
    e.g.: ./${0##*/} -to 48000 audio8k16S.wav

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
setup "$@" || exit $RC

asrc_convert "$@" || exit $RC

