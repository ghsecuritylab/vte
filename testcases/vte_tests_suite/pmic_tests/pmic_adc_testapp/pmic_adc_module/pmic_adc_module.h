/***
**Copyright 2006-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
        @file   pmic_adc_module.h

        @brief  PMIC ADC test module header file
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
D.Khoroshev/B00313           07/04/2006     TLSbo64235  Initial version

====================================================================================================
Portability:  ARM GCC
==================================================================================================*/
#ifndef PMIC_POWER_TEST_H
#define PMIC_POWER_TEST_H

#include <linux/autoconf.h>
#include <asm/arch/pmic_status.h>
#include <asm/arch/pmic_adc.h>

#ifdef __cplusplus
extern "C"{
#endif


/*==================================================================================================
                CONSTANTS
==================================================================================================*/

/*==================================================================================================
                DEFINES AND MACROS
==================================================================================================*/
#define PMIC_ADC_DEVICE "test_pmic_adc"

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

#ifdef __cplusplus
}
#endif

#endif        /* PMIC_ADC_TEST_H */
