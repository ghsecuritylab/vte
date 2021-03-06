/***
 * **Copyright (C) 2008-2009 Freescale Semiconductor, Inc. All Rights Reserved.
 * **
 * **The code contained herein is licensed under the GNU General Public
 * **License. You may obtain a copy of the GNU General Public License
 * **Version 2 or later at the following locations:
 * **
 * **http://www.opensource.org/licenses/gpl-license.html
 * **http://www.gnu.org/copyleft/gpl.html
 * **/
/*================================================================================================*/
/**
    @file   rtc_test_6.h

    @brief  RTC test 6 header
*/
/*===================================================================================================
Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------

Blake                       12/29/2008
====================================================================================================
Portability:  ARM GCC  gnu compiler
==================================================================================================*/

#ifndef RTC_TEST7_H
#define RTC_TEST7_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>

/*==================================================================================================
                                           CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/
/* number of attempting RTC device list */
#define RTC_DEVICE_NUM 2
#define WAKEUP_ALARM_ENABLED 1
#define WAKEUP_ALARM_DISABLED 0
#define WKALM_PENDING 0
#define WKALM_NOT_PENDING 1

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
int VT_rtc_test7_setup();
int VT_rtc_test7_cleanup();

int VT_rtc_alarm_on(int seconds);
int VT_rtc_alarm_off();

void cleanup();
void help();

#ifdef __cplusplus
}
#endif

#endif
