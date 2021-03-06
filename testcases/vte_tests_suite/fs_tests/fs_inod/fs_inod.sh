#Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#!/bin/sh
##############################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#S.ZAVJALOV/-----             10/06/2004     TLSbo39741  Initial version
#L.Delaspre/rc149c            08/12/2004     TLSbo40142   update with Freescale identity
#L.Delaspre/rc149c            23/03/2005     TLSbo47635   rework the header of test script
# Spring                      28/11/2008       n/a      Modify COPYRIGHT header
#
###################################################################################################
#
#========================================================================
#
# CHANGE HISTORY:
#               DATE            AUTHOR                  REASON
#               04/18/98        Dara Morgenstein        Project Yeager (AIX)
#               02/08/01        Jay Inman               Modified to run standalone on Linux
#               05/24/01        Jay Inman               Added command line args
#               06/27/01        Jay Inman               Ported from Korn to Bash
# 
#***********************************************************************


#=============================================================================
# FUNCTION NAME:        err_log
#
# FUNCTION DESCRIPTION: Log error
#
# PARAMETERS:           None.
#
# RETURNS:              None.
#=============================================================================
err_log()
{
    error "$1"
    let step_errors="$step_errors + 1"
}


#=============================================================================
# FUNCTION NAME:        make_subdirs
#
# FUNCTION DESCRIPTION: Creates $numsubdirs subdirectories
#
# PARAMETERS:           None.
#
# RETURNS:              None.
#=============================================================================
make_subdirs()
{
    i=0;
    while [ "$i" -lt "$numsubdirs" ]; do
        [ -d dir$i ] || { \
            echo "$0: mkdir dir$i"
            mkdir -p dir$i || echo "mkdir dir$i FAILED"
        }
        let i="$i + 1"
    done;
} 


#=============================================================================
# FUNCTION NAME:        touch_files
#
# FUNCTION DESCRIPTION: Creates $numfiles in each of $numsubdirs directories
#
# PARAMETERS:           None.
#
# RETURNS:              None.
#=============================================================================
touch_files()
{
    echo "$0: touch files [0-$numsubdirs]/file$numsubdirs[0-$numfiles]"
    j=0;

    while [ "$j" -lt "$numsubdirs" ]; do
        cd dir$j
        k=0;

        while [ "$k" -lt "$numfiles" ]; do  
            >file$j$k || err_log ">file$j$k FAILED"
            let k="$k + 1"
        done

        let j="$j + 1"
        cd ..
    done
}


#=============================================================================
# FUNCTION NAME:        rm_files
#
# FUNCTION DESCRIPTION: Removes $numfiles in each $numsubdir directory
#
# PARAMETERS:   None.
#
# RETURNS:      None.
#=============================================================================
rm_files()
{
    echo "$0: rm files [0-$numsubdirs]/file$numsubdirs[0-$numfiles]"
    j=0;

    while [ "$j" -lt "$numsubdirs" ]; do
        cd dir$j
        k=0;

        while [ "$k" -lt "$numfiles" ]; do  
            rm -f file$j$k || err_log "rm -f file$j$k FAILED"
            let k="$k + 1"
        done

        let j="$j + 1"
        cd ..
    done
}


#=============================================================================
# FUNCTION NAME:        step1
#
# FUNCTION DESCRIPTION: multiple processes creating and deleting files
#
# PARAMETERS:           None.
#
# RETURNS:              None.
#=============================================================================
step1() 
{ 
    echo "=============================================="
    echo "MULTIPLE PROCESSES CREATING AND DELETING FILES"
    echo "=============================================="

    echo "$0: creating dir2 subdirectories"
    [ -d dir2 ] || { \
        mkdir -p dir2 || end_testcase "mkdir dir2 failed"
    }
    cd dir2 || err_log "cd dir2 FAILED"
    make_subdirs || err_log "make_subdirs on dir2 FAILED"
    cd ..

    echo "$0: creating dir1 subdirectories & files"
    [ -d dir1 ] || { \
        mkdir dir1 || abort "mkdir dir1 FAILED"
    }
    cd dir1 || err_log "cd dir1 FAILED"
    make_subdirs || err_log "make_subdirs on dir1 FAILED"
    touch_files 
    pid1=$!
    
    i=1;   
    while [ "$i" -le "$numloops" ]; do
        echo "Executing loop $i of $numloops..."

#       Added date stamps to track execution time and duration

        echo "$0: cd ../dir1 & creating files"
        cd ../dir1
        wait $pid1 
        touch_files &
        pid1=$!
        
        echo "$0: cd ../dir1 & removing files"
        cd ../dir1
        wait $pid1 
        rm_files &
        pid1=$!

        echo "$0: cd ../dir2 & creating files"
        cd ../dir2
        wait $pid2
        touch_files &
        pid2=$!

        echo "$0: cd ../dir2 & removing files"
        cd ../dir2
        wait $pid2 
        rm_files &
        pid2=$!

        let i="$i + 1"
    done

    # wait for all background processes to complete execution
    wait        
    return $step_errors
}


#=============================================================================
# MAIN 
#     See the description, purpose, and design of this test under TEST 
#     in this test's prolog.
#=============================================================================
    USAGE="Usage: ./fs_inod [volumename] [numsubdirectories] [numfiles] [numloops]"

    if (($# != 4))
    then
       echo $USAGE
       exit 2
    fi

    testvol=$1
    numsubdirs=$2
    numfiles=$3
    numloops=$4

    cd $testvol || exit 2
    
    echo "FS_INODE: File system stress - inode allocation/deallocation"
    echo "Volume under test: $testvol"
    echo "Number of subdirectories: $numsubdirs"
    echo "Number of files: $numfiles"
    echo "Number of loops: $numloops"
    echo "Execution begins "
    date

    STEPS="1"
    for I in $STEPS
    do
         step_errors=0
         step$I
         if [ $? != 0 ]; then
            error "step$I failed - see above errors"
         fi
    done   

# Clean up and timestamp
    rm -rf $testvol/dir*
    echo "Execution completed"
    date

    exit $ERRORS
