/***
**Copyright (C) 2005-2011 Freescale Semiconductor, Inc. All Rights Reserved.
**
**The code contained herein is licensed under the GNU General Public
**License. You may obtain a copy of the GNU General Public License
**Version 2 or later at the following locations:
**
**http://www.opensource.org/licenses/gpl-license.html
**http://www.gnu.org/copyleft/gpl.html
**/
/*================================================================================================*/
/**
    @file   rtc_main.c

    @brief  RTC test 5 main file

====================================================================================================
Revision History:
                        Modification     Tracking
Author                      Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
E.Gromazina                23/06/2005          TLSbo49951          Initial version
Z.Spring                   23/03/2011        n/a         List an attempting device list
====================================================================================================
Portability:  ARM GCC  gnu compiler
==================================================================================================*/

/*==================================================================================================
Total Tests: 1

Test Name:   rtc_testapp_5

Test Assertion
& Strategy:  Get and set RTC epoch, testing the poll and fysinc features
==================================================================================================*/


#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
    
/* Harness Specific Include Files. */
#include "test.h"
#include "usctest.h"

/* Verification Test Environment Include Files */
#include "rtc_test_5.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/


/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/


/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/
/* Extern Global Variables */
extern int  Tst_count;               /* counter for tst_xxx routines.         */
extern char *TESTDIR;                /* temporary dir created by tst_tmpdir() */

/* Global Variables */
char *TCID     = "rtc_testapp_5"; /* test program identifier.          */
int  TST_TOTAL = 1;                  /* total number of tests in this file.   */
char * pdevice = NULL;
char* RTC_DRIVER_NAME[RTC_DEVICE_NUM] = {"/dev/rtc", "/dev/rtc0"};    /* RTC devices list of attempting to open */
/*==================================================================================================
                                   GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void cleanup(void);
void setup(void);
int main(int argc, char **argv);
void help(void);

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== cleanup =====*/
/**
@brief  Performs all one time clean up for this test on successful
                                completion,  premature exit or  failure. Closes all temporary
                                files, removes all temporary directories exits the test with
                                appropriate return code by calling tst_exit() function.cleanup

@param  Input :      None.
        Output:      None.
  
@return Nothing
*/
/*================================================================================================*/
void cleanup(void)
{
        /* VTE : Actions needed to get a stable target environment */
        int VT_rv = TFAIL;

        VT_rv = VT_rtc_test5_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_cleanup() Failed : error code = %d", VT_rv);
        }

        tst_exit();
}

/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== setup =====*/
/**
@brief  Performs all one time setup for this test. This function is
        typically used to capture signals, create temporary dirs
        and temporary files that may be used in the course of this test.

@param  Input :      None.
        Output:      None.

@return On failure - Exits by calling cleanup().
        On success - returns 0.
*/
/*================================================================================================*/
void setup(void)
{
        int VT_rv = TFAIL;

        VT_rv = VT_rtc_test5_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK , cleanup, "VT_setup() Failed : error code = %d", VT_rv);
        }

        return;
}


/*================================================================================================*/
/*===== main =====*/
/**
@brief  Entry point to this test-case. It parses all the command line
        inputs, calls the global setup and executes the test. It logs
        the test status and results appropriately using the LTP API's
        On successful completion or premature failure, cleanup() func
        is called and test exits with an appropriate return code.

@param  Input :      argc - number of command line parameters.
        Output:      **argv - pointer to the array of the command line parameters.
        Describe input arguments to this test-case
                -l - Number of iteration
                -v - Prints verbose output
                -V - Prints the version number
  
@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
/*================================================================================================*/
int main(int argc, char **argv)
{
        int VT_rv = TFAIL;

        /* parse options. */
        int t_flag=0;                 /* binary flags: opt or not */
        char *test_case;  /* option arguments */
	    int d_flag=0;
        char *msg;
        
        option_t options[] = {
                { "T:", &t_flag, &test_case  },       /* argument required */
                { "d:", &d_flag, &pdevice  },       /* argument required */
                { NULL, NULL, NULL }                    /* NULL required to end array */
        };
        
        if ( (msg=parse_opts(argc, argv, options, &help)) != NULL ) 
                tst_brkm(TBROK , cleanup, "OPTION PARSING ERROR - %s", msg);

        /* perform global test setup */
        setup();

        if(t_flag) 
        {
                /* Print test Assertion using tst_resm() function with argument TINFO. */
                tst_resm(TINFO, "------------------------------------------------");
                tst_resm(TINFO, "Testing if %s_%s test case is OK", TCID,test_case);
        
                if(!strcmp(test_case,"EPOCH")) 
                {
                        VT_rv = VT_rtc_test5(0); 
                } 
                else if(!strcmp(test_case,"POLL"))
                {
                        VT_rv = VT_rtc_test5(1); 
                }
                else if(!strcmp(test_case,"FASYNC"))
                {
                        VT_rv = VT_rtc_test5(2);
                }
                else if(!strcmp(test_case,"READ_WAIT"))
                {
                        /* doesn't support on 3.5.7 currently */
                        tst_resm(TWARN, "The READ_WAIT test case does not support currently");
                        cleanup();
                        return VT_rv;
                        /*VT_rv = VT_rtc_test5(3);*/
                }
                else
                {
                        help();
                        cleanup();
                        return VT_rv;
                }
                    
                if(VT_rv == TPASS)
                        tst_resm(TPASS, "%s test case worked as expected", TCID);
                else
                        tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);
        
        /* cleanup allocated test ressources */        
                cleanup();
        }
        
        if(t_flag==0) 
        {
                /* VTE : print results and exit test scenario */
                help();
        }
          
        return VT_rv;        
}

void help(void)
{
        printf("RTC driver option\n");
        printf("  -T EPOCH   Test epoch read and set RTC funcitons\n");
        printf("  -T POLL      POLL test for RTC \n");
        printf("  -T FASYNC  FASYNC test for RTC \n");
        printf("  -T READ_WAIT  READ and WAIT test for RTC \n");
}

#ifdef __cplusplus
}
#endif
