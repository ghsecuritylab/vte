/***
**Copyright 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
        @file   oss_sound_driver_test.h

        @brief  OSS audio mulpitple play test header file.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
RB657C/gsch1c                20/07/2004     TLSbo40898  Initial version  of OSS sound driver test development
RB657C /gsch1c               20/07/2004     TLSbo43102  Add a parameter to the application
D.Khoroshev/B0031            02/15/2006     TLSbo62323  Updates accoring to the last specifications

==================================================================================================*/
#ifndef oss_sound_driver_TEST_H
#define oss_sound_driver_TEST_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>
#include <pthread.h>

/*==================================================================================================
                                            CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/

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
unsigned short get_audio_channels(FILE* file);

int VT_oss_sound_driver_test(int device, char * file1, char * file2, int loop);
int VT_oss_sound_driver_setup(void);
int VT_oss_sound_driver_cleanup(void);
int VT_oss_sound_driver_setup(void);
int VT_oss_sound_driver_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif        /* oss_sound_driver_TEST_H */
