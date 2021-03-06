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
        @file   keypad_test_2.h

        @brief  keypad test 2 header file
====================================================================================================
Revision History:
                            Modification     Tracking
Author/Core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
V. Becker/rc023c             30/04/2004     TLSbo38735  Initial version
V. Becker/rc023c             25/05/2004     TLSbo38735  Change file name
L. DELASPRE / rc149c         16/08/2004     TLSbo40891  VTE 1.4 integration 
C. Gagneraud cgag1c          08/11/2004     TLSbo44474  Fix #include issues.
A.Ozerov/NONE                10/01/2006     TLSbo61037  Update in accordance with linux-2.6.10-rel-L26_1_15

==================================================================================================*/
#ifndef KEYPAD_TEST2_H
#define KEYPAD_TEST2_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <linux/kd.h>
#include <sys/ioctl.h>
#include <asm/arch/mxc_keyb_ioctl.h>

/*==================================================================================================
                                            CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/
#define KEYPAD_DRIVER "/dev/vc/0"

#define NUMBER_OF_KEYS 64

#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif

/*==================================================================================================
                                            ENUMS
==================================================================================================*/

/*==================================================================================================
                                STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                    FUNCTION PROTOTYPES
==================================================================================================*/
int VT_keypad_test2_setup(void);
int VT_keypad_test2_cleanup(void);

int VT_keypad_test2(void);

#ifdef __cplusplus
}
#endif

#endif        /* KEYPAD_TEST2_H */
