#!/bin/bash

setup()
{
    # Total number of test cases in this file. 
    export TST_TOTAL=1

    # The TCID and TST_COUNT variables are required by the LTP 
    # command line harness APIs, these variables are not local to this program.

    # Test case identifier
    export TCID="v4l2_POWER_test"
    # Set up is initialized as test 0
    export TST_COUNT=0
    # Initialize cleanup function to execute on program exit.
    # This function will be called before the test program exits.
    trap "cleanup" 0
	plt=$(platfm.sh)
    RC=0
    return $RC
}

cleanup()
{
    echo "CLEANUP "
}

usage()
{
    echo "1: for device suspend resume case for no boot cores "	
    echo "2: for device suspend resume case for all cores "	
    echo "3: wait mode test for no boot cores "	
}

# Function:     test_case_01
# Description   - Test if device suspend and resume without bootcore
#  
test_case_01()
{
    #TODO give TCID 
    TCID="v4l2_PM_NOBOOTCORE"
    #TODO give TST_COUNT
    TST_COUNT=1
    RC=1

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    #TODO add function test scripte here
    if [ $plt = "IMX6SL-EVK" ]; then
    v4l2_capture_emma -C 2 -T 1000 &
	else
	v4l_capture_testapp -T 100 &
	fi

    echo "core test"
    i=0
    loops=10
    echo core > /sys/power/pm_test
    while [ $i -lt $loops ]
    do
        i=$(expr $i + 1)
        echo mem > /sys/power/state
        echo standby > /sys/power/state
    done

    sleep 30
    echo none > /sys/power/pm_test

    wait

    RC=0

    return $RC

}

# Function:     test_case_02
# Description   - Test if device suspend and resume without bootcore
#  
test_case_02()
{
    #TODO give TCID 
    TCID="vpu_PM_BOOTCORE"
    #TODO give TST_COUNT
    TST_COUNT=1
    RC=1

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "
    tloops=100
    count=0
    #TODO add function test scripte here
    while [ $count -lt $tloops ]
    do

    if [ $plt = "IMX6SL-EVK" ]; then
    v4l2_capture_emma -C 2 -T 1000 &
	else
	v4l_capture_testapp -T 100 &
	fi

        i=0
        loops=100
        while [ $i -lt $loops ]
        do
            i=$(expr $i + 1)
            rtc_testapp_6 -T 50 -m mem
            rtc_testapp_6 -T 50 -m standby
        done

        wait

        count=$(expr $count + 1)
    done

    RC=0

    return $RC

}

# Function:     test_case_03
# Description   - Test if device suspend and resume without bootcore
#  
test_case_03()
{
    #TODO give TCID 
    TCID="v4l2_PM_WAITMODE"
    #TODO give TST_COUNT
    TST_COUNT=1
    RC=1

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    #TODO add function test scripte here
    v4l_capture_testapp -T 100 &

    echo "core test"
    i=0
    loops=10
    echo core > /sys/power/pm_test
    while [ $i -lt $loops ]
    do
        i=$(expr $i + 1)
        echo standby > /sys/power/state
    done

    sleep 30
    echo none > /sys/power/pm_test

    wait

    RC=0

    return $RC

}

plt=63
setup || exit 1

case "$1" in
1)
    test_case_01 || exit 2 
    ;;
2)
    test_case_02 || exit 3
    ;;
3)
    test_case_03 || exit 3
    ;;
*)
    usage
    ;;
esac

tst_resm TINFO "Test Finish"
