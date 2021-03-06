/***
**Copyright (C) 2006-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
        @file   watchdog_test_2.h

        @brief  watchdog test header file

====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  --------------------------------------------
V.Khalabuda/b00306           06/07/2006     TLSbo69495  Update for MX27 platform in accordance with changes in time.c file
Gamma Gao/b14842             12/24/2007                 Chang for MX37 platform(Kernel 2.6.22)
==================================================================================================*/
#ifndef WATCHDOG_TEST_2_H
#define WATCHDOG_TEST_2_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/watchdog.h>
#include <sys/ioctl.h>

/*==================================================================================================
                                            CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/
#define WATCHDOG_TEST     "/dev/watchdog"

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
int VT_watchdog_test2_setup(void);
int VT_watchdog_test2_cleanup(void);

int VT_watchdog_test2(void);


#ifdef __cplusplus
}
#endif

#endif        /* WATCHDOG_TEST_2_H */
