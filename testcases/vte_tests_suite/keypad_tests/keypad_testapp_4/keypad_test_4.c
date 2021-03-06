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
        @file   keypad_test_4.c

        @brief  Threads implementation of keypad test : display and read a keypad key in RAW mode
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
V.BECKER/rc023c              30/03/2004     TLSbo38735  Initial version
V.BECKER/rc023c              15/04/2004     TLSbo38735  Correction after code inspection
V.BECKER/rc023c              29/04/2004     TLSbo38735  File name change to keypad_test_1.c
                                                        RAW mode testing. Configure matrix
D.SIMAKOV/smkd001c           31/05/2004     TLSbo39737  The configuration of row and column number
                                                        through test arguments is added
V.BECKER/rc023c              03/06/2004     TLSbo39684  Corrections for End key press
C.Gagneraud                  08/11/2004     TLSbo44474  Warning fixup.
L.Delaspre/rc149c            22/03/2005     TLSbo48665  Update mapcode
A.Ozerov/NONE                10/01/2006     TLSbo61037  Update in accordance with linux-2.6.10-rel-L26_1_15
I.Inkina/nknl001             10/01/2006     TLSbo61037  Update in accordance with linux-2.6.10-rel-L26_1_15
A.Ozerov/B00320              15/02/2006     TLSbo61037  Device was changed and testapp was reworked accordingly

====================================================================================================
Portability:  ARM GCC
==================================================================================================*/


/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <errno.h>

/* Harness Specific Include Files. */
#include <test.h>

/* Verification Test Environment Include Files */
#include "keypad_test_4.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                        LOCAL CONSTANTS
==================================================================================================*/
/*=================================================================================================
                        This Keyarraycode is for MXC30030 Platform
==================================================================================================*/
#if defined CONFIG_MACH_MXC30030ADS
char   *keyarraycode[] =
{"TBD", "0", "1","2", "3","4","5", "6","7","8",
 "9","0","0","0","0","0","0","0","0","0",
 "0","0","0","0","0","0","0","0","5navkey","0",
 "0","0","0","0","0","0","0","0","0","0","0",
 "0","shift","0","0","0","0","0","0","0","0","0",
 "#","0","0","*","0","0","0","spare",
 "spare","voice call","video call","carrier","right soft","menu","left menu",
 "spare", "VR","0","0","0","0","0","0","0","0","0",
 "+","0","0","0","0","0","0","0","0","camera",
 "speaker phone","left open","0","0","0","0","0","0","0","0",
 "0","0","0","0","0","UP","0","LEFT","RIGTH",
 "END","DOWN","0","0","0","0","0","VOL_DOWN",
 "VOL_UP","0","0","0","0","0","0","0","0","0",
 "0","0","0","0","0","0","0","0","0","0","0",
 "VIDEO","AUDIO","0","menu","0","0","0","0","0","0","0",
 "0","0","0","browser","0","0","0","0","0","0","0","0",
 "0","0","0","0","0","0","0","0","0","0","spk_phone",
 "0","0","0","0","0","0","0","0","0","0","0","0","0",
 "0","0","0","0","0","0","0","0","0","0","0"
 "0","0","0","0","0","0","0","0","0","0","0","0","0",
 "0","0","0","0","0","0","camera",
};

/*===================================================================================================
                        This Keyarraycode is for MX21/MX27 Platform
====================================================================================================*/
#elif defined CONFIG_MACH_MX27ADS
char   *keyarraycode[] =
{"0", "0", "1","2", "3","4","5", "6","7","8",
 "9","0","0","0","bs","Tab","q","w","e","r",
 "t","y","u","i","o","p","0","0","action","0",
 "a","s","d","f","g","h","j","k","l","0",
 "0", "0","shift","0","z","x","c","v","b","n",
 "m","0", ".","0","0","*","0","space","Caps","key1",
 "key2","#","send","0","on/off","symb","0","0", "0","0",
 "0","extra3","extra4","extra5","0","app4","extra1","extra2", "+","app1",
 "app2","app3","0","0","0","0","0","0", "0","0",
 "0","0","0","0","0","0","0","0", "0","0",
 "0","0","Home","UP","0","LEFT","RIGTH", "END","DOWN","0",
 "0","0","0","0","VOL_DOWN", "VOL_UP","POWER","0","0","0",
 "0","0","0","0","0", "0","0","0","0","0",
 "0","0","0","0","0","0", "0","0","0","0",
 "0","0","0","0","0","Send","0", "0","0","0",
 "0","0","0","0","0","0","0","0","Back", "0",
 "0","SEL","0","0","0","0","0","Record",
};

/*===================================================================================================================
This keyarraycode is for MX37 Platform
===================================================================================================================*/
#else
 char   *keyarraycode[] =
{"0", "ESC", "1","2", "3","4","5", "6","7","8",
 "9","0","0","0","bs","Tab","q","w","e","r",
 "t","y","u","i","o","p","0","0","Enter","0",
 "a","s","d","f","g","h","j","k","l","0","0",
 "1","shift","0","z","x","c","v","b","n","m","0",
 ".","0","0","*","LEFTALT","space","Caps","Key1",
 "on/off","0","0","0","app1","symb","app2","app3",
 "app4","0","0","0","0","0","0","0","0","0",
 "+","0","0","0","0","0","0","0","0","#",
 "key2","0","0","0","0","0","0","0","0","0",
 "0","0","0","0","Home","UP","0","LEFT","RIGTH",
 "END","DOWN","0","0","0","0","0","VOL_DOWN",
 "VOL_UP","0","0","0","0","0","0","0","0","0",
 "0","0","0","0","0","0","0","0","0","0","0",
 "0","0","0","0","0","0","0","0","0","Send","0",
 "0","0","0","0","0","0","0","0","0","0","0","Back",
 "0","0","SEL","0","0","0","0","0","Record",
};

#endif
/*==================================================================================================
                                        LOCAL VARIABLES
==================================================================================================*/
int     fd = 0;
/*==================================================================================================
                                        GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                        GLOBAL VARIABLES
==================================================================================================*/
static bool gfinished;

/*==================================================================================================
                                    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
int     readkeycode_function(void);
int     ask_user(void);

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/
/*================================================================================================*/
/*===== VT_keypad_test4_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution
ioctl: Inappropriate ioctl for device
@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_keypad_test4_setup(void)
{
        int     rv = TFAIL;

        /* Open keypad driver */
        fd = open(KEYPAD_DRIVER_EVENT, O_RDONLY);
        sleep(1);
        if (fd < 0)
        {
                tst_resm(TFAIL, "ERROR : Open keypad driver fails \n");
        }
        else
        {
                rv = TPASS;
        }

        return rv;
}

/*================================================================================================*/
/*===== VT_keypad_test4_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_keypad_test4_cleanup(void)
{
        int     rv = TFAIL;
        int     ret = 0;

        /* Close keypad driver */
        ret = close(fd);
        if (ret < 0)
        {
                tst_resm(TFAIL, "ERROR : Close keypad driver fails \n");
                perror("Error: cannot close device \n");
        }
        else
        {
                rv = TPASS;
        }

        return rv;
}

/*================================================================================================*/
/*===== VT_keypad_test4 =====*/
/**
@brief  Keypad test with read and display key scan code in RAW mode

@param  row_count - Number of rows of the keypad matrix
@param  col_count - Number of coloumns of the keypad matrix

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_keypad_test4(int tcase)
{
        int     rv = TFAIL;

        switch(tcase)
        {
                case 0:
                        gfinished = FALSE;

                        if (readkeycode_function())
                        {
                                tst_resm(TFAIL, "ERROR : keycode  fails ");
                                return rv;
                        }
                        if (ask_user() == TFAIL)
                        {
                                return rv;
                        }

                        rv = TPASS;
                        break;
                case 1:
                        tst_resm(TFAIL, "Test case not supported");
        }

        return rv;
}

/*================================================================================================*/
/*===== readkeycode_function =====*/
/**
@brief  read keycode pressed by user

@param

@return On success
        On failure
*/
/*================================================================================================*/
int readkeycode_function(void)
{
        int     i = 0,
        short_scancodes = 0;
        unsigned char buf[10];
        int     rv = TPASS;
         int n=0;

        tst_resm(TINFO, "click DOWN Key to exit");
        tst_resm(TINFO, "Please press keys on the keypad:");
        while (gfinished == FALSE)

        {
                   
                memset(buf, 0, sizeof(buf));
                short_scancodes = read(fd, buf, 20);
                if (short_scancodes > 0)
                {
                            n++;

                        if (buf[10] == 108)
                        {
                                gfinished = TRUE;
                                     printf("Suport UP,DOWN,ENTER,LEFT,RIGHT,MENU6,MENU7,MENU8 \n");
                                printf("KPP TEST APP: 'DOWN'  key Pressed, Keypad testing is terminated \n");
                                     i = buf[10];
                                     if(n%2==0)
                                         tst_resm(TINFO, "The status of key is released");
                                     else
                                         tst_resm(TINFO, "The status of key is pressed");
                        }
                        i = buf[10];
                        printf("KPP TEST APP: Pressed key is %s\n", keyarraycode[i]);
                            if(n%2==0)
                                  tst_resm(TINFO, "The status of key is released");
                            else
                                  tst_resm(TINFO, "The status of key is pressed");


                }
                else
                {
                        rv = TFAIL;
                        gfinished = TRUE;

                }
        }

        return rv;
}

/*================================================================================================*/
/*===== ask_user =====*/
/**
@brief  Asks user to answer the question: is the test right?

@param  Input:  None
        Output: None

@return 1 - if user asks "No,  wrong"
        0 - if user asks "Yes, right"
*/
/*================================================================================================*/
int ask_user(void)
{
        unsigned char answer;
        int     ret = 2;

        do
        {
                printf("Did test work as expected? [y/n] ");
                answer = fgetc(stdin);
                if (answer == 'Y' || answer == 'y')
                        ret = TPASS;
                else if (answer == 'N' || answer == 'n')
                        ret = TFAIL;
        }
        while (ret == 2);
        fgetc(stdin);
        return ret;
}
