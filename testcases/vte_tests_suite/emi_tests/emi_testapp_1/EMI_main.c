/***
**Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
    @file   EMI_main.c

    @brief  LTP Motorola template.
     
====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Sergey Zavjalov           10/06/2004      TLSbo39741   Initial version 

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/

/*==================================================================================================
Total Tests: TO BE COMPLETED

Test Name:   TO BE COMPLETED 

Test Assertion
& Strategy:  A brief description of the test Assertion and Strategy
             TO BE COMPLETED
==================================================================================================*/



/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#ifdef __cplusplus
extern "C"{
#endif

    
/* Harness Specific Include Files. */
#include "test.h"
#include "usctest.h"

#ifdef __cplusplus
}
#endif


/* Verification Test Environment Include Files */
#include "EMI_test_X.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/


/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/
int verbose_flag=0;  /* verbose_mode */


/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

option_t options[] =
  {
    { "v", &verbose_flag, NULL },
    { NULL, NULL, NULL }
  };

/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/
#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif

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
char *TCID     = "message_testapp_1"; /* test program identifier.          */
int  TST_TOTAL = 1;                  /* total number of tests in this file.   */

/*==================================================================================================
                                   GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void cleanup();
void setup();
int main(int argc, char **argv);

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
void cleanup()
{
    /* VTE : Actions needed to get a stable target environment */
		int VT_rv = TFAIL;
		
		VT_rv = VT_EMI_cleanup();
		if (VT_rv != TPASS)
		{
			tst_resm(TWARN, "VT_cleanup() Failed : error code = %d", VT_rv);
		}
		/* VTE */
		
		/* Close all open file descriptors. */

    /** Insert code here. In case an unexpected failure occurs report it 
    and exit setup(), the following lines of code will demonstrate 
    this.

    if (close(fd) == -1) 
    {
       tst_resm(TWARN, "close(%s) Failed, errno=%d : %s",
           fname, errno, strerror(errno));
    } */
 
    /* Remove all temporary directories used by this test. */

    /** Insert real code here */
    
    /* kill child processes if any. */
   
    /** Insert code here */

    /* Exit with appropriate return code. */

    tst_exit();
}

/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/
/*================================================================================================*/
/*===== help =====*/
/**
@brief  Inform of the available options and the associated parameters

@param  Input :      None.
        Output:      None.
 
*/
/*================================================================================================*/
void help()
{
  printf("Options:\n\n");
  printf("-v	       verbose mode\n");
}

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
void setup()
{
 	int rv = TFAIL;
   /* Capture signals. */

    /** Insert code here */

    /* Create temporary directories */

    /** Insert code here */
   
    /* Create temporary files. */

    /** Insert real code here. In case an unexpected failure occurs 
    report it and exit setup(). Follow the code below for example.
 
    if ((fd = open(fname, O_RDWR|O_CREAT, 0700)) == -1 )
    {
        tst_brkm(TBROK, cleanup,
           "Unable to open %s for read/write.  Error:%d, %s", 
           fname, errno, strerror(errno));
    } */
	
    /* VTE : Actions needed to prepare the test running */
		rv = VT_EMI_setup();
		if (rv != TPASS)
		{
			tst_brkm(TBROK , cleanup, "VT_setup() Failed : error code = %d", rv);
		}
		/* VTE */
    
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
                   **argv - pointer to the array of the command line parameters.
        Output:   TPASS/TFAIL
@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
/*================================================================================================*/
int main(int argc, char **argv)
{
  int VT_rv = TFAIL;
/*  parse options
  char *msg;

 
  printf("Parse options\n");
  msg=parse_opts(argc, argv, options, help);
  if (msg != (char *)NULL)
    {
      tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", msg);
    }
  
*/		

    /** LTP test harness provides a function called parse_opts() that
    may be used to parse standard options. For a list of standard 
    option that are available refer on-line man page at the LTP 
    web-site */

    /* perform global test setup, call setup() function. */
    setup();

    /* Print test Assertion using tst_resm() function with argument TINFO. */
    tst_resm(TINFO, "Testing if %s test case is OK", TCID);

    /* Test Case Body. */

    /** Insert real code goes here. In case of unexpected failure, or 
    failure not directly related to the entity being tested report 
    using tst_brk() or tst_brkm() functions.

    if (something bad happened)
    {
        tst_brkm(TBROK, cleanup, "Unable to open test file %s", fname);
    } */

    /* VTE : print results and exit test scenario */
    VT_rv = VT_EMI_test_X(argc,argv); /*with the parameters needed come from parse_opt())*/
	
    if(VT_rv == TPASS)
        tst_resm(TPASS, "%s test case worked as expected", TCID);
    else
        tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);
		
		
    cleanup(); /** OR tst_exit(); */
		/* VTE */
	
   return VT_rv;
	
}
