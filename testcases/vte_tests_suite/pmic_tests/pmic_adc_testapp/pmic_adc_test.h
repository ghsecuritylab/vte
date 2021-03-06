/***
**Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
        @file   pmic_adc_test.h

        @brief  Test scenario C header PMIC(SC55112 and MC13783) ADC driver.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core id                  Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
V. Halabuda/HLBV001          07/25/2005     TLSbo52694   Initial version
V. HALABUDA/HLBV001          11/21/2005     TLSbo58395   Update for linux-2.6.10-rel-L26_1_14
E. Gromazina/NONE            12/27/2005     TLSbo59968   Update for MXC91231 and MXC91131
D. Khoroshev/b00313          07/06/2006     TLSbo64235   Added PMIC ADC test module
D. Khoroshev/b00313          07/26/2006     TLSbo64235   Added mc13783 legacy support
====================================================================================================
Portability: ARM GCC

==================================================================================================*/
#ifndef _PMIC_ADC_TEST_H
#define _PMIC_ADC_TEST_H

#ifdef __cplusplus
extern "C"{
#endif

#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <stdlib.h>            /* atoi() */
#include <stdio.h>             /* sscanf() & perror() */
#include <fcntl.h>             /* open() */
#include <sys/ioctl.h>         /* ioctl() */
#include <errno.h>
#include <linux/autoconf.h>

/* Harness Specific Include Files. */
#include <test.h>
#include <usctest.h>


#include <linux/pmic_status.h>

#if defined( CONFIG_MXC_PMIC_SC55112 ) || defined( CONFIG_MXC_PMIC_MC13783 )
#include <linux/pmic_adc.h>
#elif defined( CONFIG_MXC_MC13783_LEGACY )
#include <mc13783_adc.h>
#endif

/*==================================================================================================
                                            CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/
#if defined( CONFIG_MXC_PMIC_SC55112 ) || defined( CONFIG_MXC_PMIC_MC13783 )
#        define PMIC_ADC_DEV "/dev/pmic_adc"
#elif defined( CONFIG_MXC_MC13783_LEGACY )
#        define PMIC_ADC_DEV "/dev/mc13783_adc"
#endif

#define SLEEP_TIME 400

/*==================================================================================================
                                            ENUMS
==================================================================================================*/
enum
{
        PMIC_ADC_CONVERT_T, PMIC_ADC_CONVERT_8X_T, PMIC_ADC_CONVERT_MULTICHANNEL_T, PMIC_ADC_SET_TOUCH_MODE_T,
        PMIC_ADC_GET_TOUCH_SAMPLE_T, PMIC_ADC_GET_BATTERY_CURRENT_T, PMIC_ADC_ACTIVATE_COMPARATOR_T, TS_READ_T
} testcase_nb;

/*==================================================================================================
                                STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                    FUNCTION PROTOTYPES
==================================================================================================*/
int VT_pmic_adc_test_setup(void);
int VT_pmic_adc_test_cleanup(void);
int VT_pmic_adc_test(void);

#ifdef __cplusplus
}
#endif

#endif  /* _PMIC_ADC_TEST_H */
