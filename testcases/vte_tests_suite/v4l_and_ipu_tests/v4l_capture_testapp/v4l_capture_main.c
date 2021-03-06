/***
**Copyright (C) 2004-2012 Freescale Semiconductor, Inc. All Rights Reserved.
**
**The code contained herein is licensed under the GNU General Public
**License. You may obtain a copy of the GNU General Public License
**Version 2 or later at the following locations:
**
**http://www.opensource.org/licenses/gpl-license.html
**http://www.gnu.org/copyleft/gpl.html
**/
/**
@file   v4l_capture_main.c
@brief  IPU capture test main unit.
Description of the file
@par Portability: ARM GCC
*/
/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
A.Geniatov/gntl002    20/07/2004   TLSbo40898   Initial version
A.Geniatov/gntl002    27/09/2004   TLSbo40898   Change after review 
Filinova Natalya      15/02/2005   TLSbo47117   Added -T flag
Filinova Natalya      11/04/2005   TLSbo48417   Added new flags and options and 
                                                new global variabals.
                                                Updated functions bodies.
Delaspre/rc149c       07/12/2004   TLSbo40142   update copyrights with Freescale
Bezrukov.S/SBAZR1C    08/17/2005   TLSbo53919   Remove the Brightness feature
N.Filinova/nfili1c    27/09/2005   TLSbo54946   Change after review
N.Filinova/nfili1c    20/10/2005   TLSbo56683   Change after review
N.Filinova/nfili1c    23/11/2005   TLSbo58746   Add the settings of cropping restangle via four new options: 
                                                left corner, top corner, width, height
Hake Huang            2/18/2009    NA           Add capture frame rate setting
=============================================================================*/
/*============================================================================
Total Tests: 1
Test Name:   IPU capture test
Test Assertion
& Strategy:    
=============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
/*======================== INCLUDE FILES ====================================*/
/* Standard Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
/* Harness Specific Include Files. */
#include "test.h"
#include "usctest.h"
/* Verification Test Environment Include Files */
#include "v4l_capture_test.h"
/*======================== LOCAL CONSTANTS ==================================*/

/*======================== LOCAL MACROS =====================================*/

/*======================== LOCAL VARIABLES ==================================*/

/*======================== GLOBAL CONSTANTS =================================*/

/*======================== GLOBAL VARIABLES =================================*/
/* Extern Global Variables */
        sResoluConfig gResoluConfig;
        int gChangeResoluTimes = 1;
	extern int Tst_count;	/* counter for tst_xxx routines.         */
	extern char *TESTDIR;	/* temporary dir created by tst_tmpdir */
     
        int gTestPerf = 0;
        int gChangResolu = 0;
	int gExitCleanup = 1;
       
/* Global Variables */
	char *TCID = "v4l_capture_testapp";	/* test program identifier.          */
	int TST_TOTAL = 1;	/* total number of tests in this file.   */

	sV4LTestConfig gV4LTestConfig;
/*======================== LOCAL FUNCTION PROTOTYPES ========================*/

	void setup(void);
	void help(void);
	int main(int argc, char **argv);

/*======================== LOCAL FUNCTIONS ==================================*/

/*===== setup =====*/
/**
@brief  Performs all one time setup for this test. This function is
        typically used to capture signals, create temporary dirs
        and temporary files that may be used in the course of this test.
@param  Input :      None.
        Output:      None.
@return Nothing.
*/

	void setup(void) {
		int VT_rv;
		
		system("v4l_module.sh setup");

		VT_rv = VT_v4l_capture_setup();
      
		if (VT_rv != TPASS) {
			tst_brkm(TBROK, cleanup,
				 "VT_setup() Failed : error code = %d", VT_rv);
		}
	}
/*===== help =====*//**
@brief  Inform of the available options and the associated parameters
@param  None.
@return Nothing.
*/ void help(void) {
		printf
		    ("Usage : -D <V4L_Device>.To put V4L device (default /dev/video0)\n");
		printf("Usage : -H <height>. To put capture image height\n");
		printf("Usage : -W <width>. Capture image width\n");
		printf
		    ("Usage : -R <n>, n - [0-7] is the type of rotation, 8 - configurable, 9 - 15 VF rotation\n");
		printf("Usage : -S To do interactive resizing\n");
		printf
		    ("Usage : -B <left,top,width,height> To do cropping of the video.\n");
		printf
		    (" The cropping restangle size is (width,heyght) pixels.\n");
		printf
		    (" The offset of this restangle is (left,top)\n");
		printf
		    ("Usage : -E To imply \"v4l_capture_testcase -V\" error cases with unsupported format\n");
		printf("Usage : -T <n>. To make the capture n times\n");
		printf
		    ("Usage : -C <case_number>.To put number corresponded to case of capture\n");
		printf("\t-C <1>:\tTo get video overlay device test\n");
		printf
		    ("\t-C <2>:\tTo get video capture device test and to display video on frame buffer device\n");
		printf
		    ("\t-C <3>:\tTo get video capture device test and to write snapshort to dump file\n");
		printf("\t-C other value:\t to get TBROK result\n");
		printf
		    ("Usage : -o <output file>.  To put output file(full path)\n");
		printf("Usage : -O <pixel_format>. To put pixel format\n");
		printf("Usage : -u <output device>. To put output device\n");
		printf
		    ("Usage : -Y <overlay_type>. 1=V4L2_FBUF_FLAG_PRIMARY, 2=V4L2_FBUF_FLAG_OVERLAY\n");
		printf
		    ("Usage : -r capture frame rate setting. <15 to 30 > default is  30\n");
		printf("Usage: -M <camera input mode> <0 - 5>\n");
		printf("Usage: -s <IPU path> CSI_IC_MEM / CSI_MEM\n");
		printf("D: define the video capture device\n");	/* Video capturing device */
		printf("K: block IO \n");
		printf("N:Capture times\n");
		printf("R: Rotation test\n");
		printf("S Resize test\n");
		printf("X  Disable asking user \n");
		printf("v Verbose mode \n");
  		printf("Z frame rate performance test \n");
        printf("d: dynamic change resolution configure file \n");
        printf("n: change resolution times \n");

	}

/*===== main =====*/
/**
@brief  Entry point to this test-case. It parses all the command line
        inputs, calls the global setup and executes the test. It logs
                                the test status and results appropriately using the LTP API's
                                On successful completion or premature failure, cleanup() func
                                is called and test exits with an appropriate return code.
@param  Input :      argc - number of command line parameters.
        Output:      **argv - pointer to the array of the command line parameters.
@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
	int main(int argc, char **argv) {
		int VT_rv = TFAIL;
		int ret = 0;
		const int heightTable[3] = { 640, 320, 200 };
		const int widthTable[3] = { 480, 240, 120 };
		const char userOutput[3][40] = { "biggest size",
			"middle size",
			"smallest size"
		};
		/* parse options. */
		char *msg;
		int Dflag = 0, Hflag = 0, Wflag = 0, Rflag = 0, Sflag = 0, Bflag = 0,
		Tflag = 0, oflag = 0, Oflag = 0, Cflag = 0, uflag = 0, Eflag = 0,
		Mflag = 0, Xflag = 0, Nflag = 0, Yflag = 0, vflag = 0, rflag = 0, Kflag = 0,
		sflag = 0, Zflag = 0, dflag = 0,nflag = 0;
		char *Dopt, *Hopt, *Wopt, *Ropt, *Topt, *oopt, *Oopt,
		*Copt, *uopt, *Yopt, *Mopt, *Nopt, *ropt, *Kopt, *sopt,
		*Bopt, *dopt, *nopt;
		option_t options[] = {
			{"B:", &Bflag, &Bopt},	/* Cropping test                        */
			{"C:", &Cflag, &Copt},	/* Case number                          */
			{"D:", &Dflag, &Dopt},	/* Video capturing device               */
			{"E",  &Eflag, NULL},	/* Error cases                          */
			{"H:", &Hflag, &Hopt},	/* Capturing height                     */
			{"K:", &Kflag, &Kopt},	/*block IO                              */
			{"N:", &Nflag, &Nopt},	/* Capture times                        */
			{"M:", &Mflag, &Mopt},
			{"O:", &Oflag, &Oopt},	/* Pixel format                         */
			{"R:", &Rflag, &Ropt},	/* Rotation test                        */
			{"S",  &Sflag, NULL},	/* Resize test                          */
			{"T:", &Tflag, &Topt},	/* Capture times                        */
			{"W:", &Wflag, &Wopt},	/* Capturing width                      */
			{"X",  &Xflag, NULL},	/* Disable asking user                  */
			{"Y:", &Yflag, &Yopt},	/* Overlay Type (for Overlay only)      */
           	{"o:", &oflag, &oopt},	/* Path to output file                  */
			{"u:", &uflag, &uopt},	/* Output device (default /dev/fb0)     */
			{"r:", &rflag, &ropt},
			{"s:", &sflag, &sopt},
			{"v",  &vflag, NULL},	/* Verbose mode                         */
  		    {"Z",  &Zflag, NULL},    /* frame rate performance test       */
            {"d:", &dflag, &dopt},    /* dynamic change resolution configure file*/
            {"n:", &nflag, &nopt},    /* change resolution times*/
			{NULL, NULL, NULL}	/* NULL required to end array           */
		};

		if ((msg = parse_opts(argc, argv, options, help)) != NULL) {
			tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s",
				 msg);
		}
		/* Init struct sV4LTestConfig */
		gV4LTestConfig.mV4LDevice = Dflag ? Dopt : "/dev/video0";
		gV4LTestConfig.mOutputDevice = uflag ? uopt : "/dev/fb0";
		gV4LTestConfig.mOutputFile = NULL;
		gV4LTestConfig.mWidth = Wflag ? atoi(Wopt) : 320;
		gV4LTestConfig.mHeight = Hflag ? atoi(Hopt) : 240;
		if ((gV4LTestConfig.mCount = Tflag ? atoi(Topt) : 10) <= 0)
			tst_resm(TWARN, "Invalid argument for -T  %d",
				 gV4LTestConfig.mCount);
		gV4LTestConfig.mCaseNum = Cflag ? atoi(Copt) : PRP_VF;
		gV4LTestConfig.mRotationMode = Rflag ? atoi(Ropt) : 0;
		gV4LTestConfig.mOutputFormat = Oflag;
		gV4LTestConfig.mCrop = Bflag;
		gV4LTestConfig.mRotation = Rflag;
		gV4LTestConfig.mMode = Mflag ? atoi(Mopt) : 0;
		gV4LTestConfig.mNeedAsk = Xflag;
		gV4LTestConfig.mVerbose = vflag;
		gV4LTestConfig.mPixFormat = Oflag ? Oopt : NULL;
		gV4LTestConfig.mCropRect.left = 0;
		gV4LTestConfig.mCropRect.top = 0;
		gV4LTestConfig.mCropRect.width = 640;
		gV4LTestConfig.mCropRect.height = 480;
		gV4LTestConfig.mOverlayType =
		    Yflag ? atoi(Yopt) : V4L2_FBUF_FLAG_OVERLAY;
		gV4LTestConfig.mFrameRate = rflag ? atoi(ropt) : 30;
		gV4LTestConfig.mIsBlock = Kflag ? atoi(Kopt) : 0;
		tst_resm(TINFO, "IO blocking is %d\n", gV4LTestConfig.mIsBlock);
                if( dflag)
                {
			gChangResolu = 1;
                        char ResoluFileName[40];
                        char * pResoluFile = NULL;
                        int num=0;
                        char line[20];
                        int width;
	                int height;
                        int mode;
                        sscanf(dopt, "%s",ResoluFileName);
                        if (!(pResoluFile = fopen(ResoluFileName, "r"))) {
                                 tst_resm(TWARN,
                                         "Error read resolution file() : Unable to open file %s : %s",
                                         ResoluFileName, strerror(errno));
                                 return TFAIL;}
                        fgets(line,20,pResoluFile);
                        while (!feof (pResoluFile))
                        {
                                 sscanf(line,"%d%d%d",&width,&height,&mode);
                                 gResoluConfig.mResolu[num].mWidth = width;
                                 gResoluConfig.mResolu[num].mHeight = height;
                                 gResoluConfig.mResolu[num].mMode = mode;
                                 num ++; 
                                 tst_resm(TINFO,
                                         "The width is  %d : the height is %d : the mode is %d",
                                        width,height,mode);
                                 fgets(line,20,pResoluFile);

                        } 
                        gResoluConfig.mCount = num;
			fclose(pResoluFile);
                        pResoluFile = NULL;
                }        
                if( nflag)
                {
                        sscanf(nopt, "%d",&gChangeResoluTimes);
                        if (gChangeResoluTimes < 0)
                        {
                             tst_resm(TBROK, "The Change resolution times should be greater than zero");
                             return TFAIL;
                        }
               
		}
		if( Zflag){            
			gTestPerf = 1;
		 	if(gV4LTestConfig.mCount < 101 )
                           {
                               tst_resm(TBROK, "The framecount must be  > 100");
                                return TFAIL;
                           }
                }
		if (sflag) {
			char mstr[255];
			char *pstr = mstr;
			int len = strlen(sopt);
			strncpy(mstr, sopt, len > 255 ? 255 : len);
			mstr[len + 1] = 0;
			while (*pstr != '\0') {
				*pstr = toupper(*pstr);
				pstr++;
			}
			if (strncmp(mstr, "CSI_IC_MEM", 10) == 0) {
				gV4LTestConfig.inputSrc = eInCSI_IC_MEM;
			} else if (strncmp(mstr, "CSI_MEM", 7) == 0) {
				gV4LTestConfig.inputSrc = eInCSI_MEM;
			} else {
				gV4LTestConfig.inputSrc = -1;	/*ignore the input */
			}
			tst_resm(TINFO, "capture source is %d\n",
				 gV4LTestConfig.inputSrc);
		} else {
			gV4LTestConfig.inputSrc = eInCSI_MEM;
		}
		if (gV4LTestConfig.mFrameRate > 30
		    || gV4LTestConfig.mFrameRate < 15) {
			tst_resm(TINFO,
				 "the frame rate is not within the recommanded range! 15-30\n");
		}
		if ((gV4LTestConfig.mCaseNum > 3)
		    || (gV4LTestConfig.mCaseNum < 1)) {
			tst_resm(TBROK, "Invalid option for -C flag : %d",
				 gV4LTestConfig.mCaseNum);
			return TFAIL;
		}
		if (Bflag) {
			sscanf(Bopt,
			       "%d,%d,%d,%d",
			       &gV4LTestConfig.mCropRect.left,
			       &gV4LTestConfig.mCropRect.top,
			       &gV4LTestConfig.mCropRect.width,
			       &gV4LTestConfig.mCropRect.height);
			if (gV4LTestConfig.mCropRect.left < 0) {
				tst_resm(TBROK,
					 "Invalid left corner for -B (Cropping): %d",
					 gV4LTestConfig.mCropRect.left);
				return TFAIL;
			}
			if (gV4LTestConfig.mCropRect.top < 0) {
				tst_resm(TBROK,
					 "Invalid top corner for -B (Cropping): %d",
					 gV4LTestConfig.mCropRect.top);
				return TFAIL;
			}
			if (gV4LTestConfig.mCropRect.width < 0) {
				tst_resm(TBROK,
					 "Invalid width for -B (Cropping): %d",
					 gV4LTestConfig.mCropRect.width);
				return TFAIL;
			}
			if (gV4LTestConfig.mCropRect.height < 0) {
				tst_resm(TBROK,
					 "Invalid height for -B (Cropping): %d",
					 gV4LTestConfig.mCropRect.height);
				return TFAIL;
			}
			if (gV4LTestConfig.mCropRect.left +
			    gV4LTestConfig.mCropRect.width > 640) {
				tst_resm(TBROK,
					 "Invalid right bound for -B (Cropping): left = %d and width = %d.It must be > 640",
					 gV4LTestConfig.mCropRect.left,
					 gV4LTestConfig.mCropRect.width);
				return TFAIL;
			}
			if (gV4LTestConfig.mCropRect.top +
			    gV4LTestConfig.mCropRect.height > 480) {
				tst_resm(TBROK,
					 "Invalid bottom bound for -B (Cropping): top = %d and height = %d.It must be > 480",
					 gV4LTestConfig.mCropRect.top,
					 gV4LTestConfig.mCropRect.height);
				return TFAIL;
			}
		}
		if (oflag) {
			gV4LTestConfig.mOutputFile =
			    malloc(strlen(oopt) * sizeof(char));
			strcpy(gV4LTestConfig.mOutputFile, oopt);
		} else {
			gV4LTestConfig.mOutputFile = malloc(9 * sizeof(char));
			strcpy(gV4LTestConfig.mOutputFile, "./output");
		}
		if (vflag) {
			tst_resm(TINFO, "V4L2 device = %s",
				 gV4LTestConfig.mV4LDevice);
			tst_resm(TINFO, "Output device = %s",
				 gV4LTestConfig.mOutputDevice);
			tst_resm(TINFO, "Height = %d", gV4LTestConfig.mHeight);
			tst_resm(TINFO, "Width = %d", gV4LTestConfig.mWidth);
			tst_resm(TINFO, "Case number = %d",
				 gV4LTestConfig.mCaseNum);
			tst_resm(TINFO, "Path to output file : %s",
				 gV4LTestConfig.mOutputFile);
			if (Bflag)
				tst_resm(TINFO,
					 "Crop to image: left = %d, top = %d, width = %d, height = %d",
					 gV4LTestConfig.mCropRect.left,
					 gV4LTestConfig.mCropRect.top,
					 gV4LTestConfig.mCropRect.width,
					 gV4LTestConfig.mCropRect.height);
		}
		/* Rotations tests */
		if (gV4LTestConfig.mRotation) {
			if ((gV4LTestConfig.mRotationMode < 0)
			    || (gV4LTestConfig.mRotationMode > 16)) {
				tst_resm(TBROK,
					 "You are entered wrong rotation option");
				return TFAIL;
			}
			if (gV4LTestConfig.mRotationMode == 8) {	/* ask user in cycle and perform a rotation */
				char rot_num[9];
				do {
					memset(rot_num, 0, 9);
					/* Get the rotation type */
					tst_resm(TINFO,
						 "Please specify the rotation type... [0-7].To exit press 9.");
					fflush(stdout);
					fgets(rot_num, 9, stdin);
					gV4LTestConfig.mRotationMode =
					    atoi(rot_num);
					if (gV4LTestConfig.mRotationMode == 8
					    || gV4LTestConfig.mRotationMode > 9
					    || gV4LTestConfig.mRotationMode <
					    0) {
						tst_resm(TINFO,
							 "Please specify the rotation type again... [0-7].To exit press 9.");
						continue;
					}
					if (gV4LTestConfig.mRotationMode == 9)
						break;
					/* Perform the test */
					setup();
					/* Test Case Body. */
					ret |= VT_v4l_capture_test();
					gExitCleanup = 0;
					cleanup();
				} while (gV4LTestConfig.mRotationMode != 9);
			}	/* else go on - perform the trivial test call */
			if (Nflag) {
				int i, j;
				j = atoi(Nopt);
				tst_resm(TINFO, "Capture %d times", j);
				for (i = 0; i < j; i++) {
					setup();
					/* Test Case Body. */
					ret |= VT_v4l_capture_test();
					if (ret) {
						tst_resm(TINFO,
							 "Error!!! on Rotate now is %d , mode is %d\n",
							 i,
							 gV4LTestConfig.
							 mRotationMode);
						break;
					}
					gExitCleanup = 0;
					cleanup();
				}
			} else {
				/* Perform the test */
				setup();
				/* Test Case Body. */
				ret |= VT_v4l_capture_test();
				gExitCleanup = 0;
				cleanup();
			}
			if (ret == TPASS)
				tst_resm(TPASS,
					 "%s test case worked as expected",
					 TCID);
			else
				tst_resm(TFAIL,
					 "%s test case did NOT work as expected",
					 TCID);
			return (ret);
		}
		/* Resizing test */
		if ((Sflag) && (gV4LTestConfig.mCaseNum == 1)) {
			int i;
			/* Print test Assertion using tst_resm() function with argument TINFO. */
			tst_resm(TINFO, "Testing if %s test case is OK", TCID);
			for (i = 0; i < 3; i++) {
				tst_resm(TINFO,
					 "Performing resizing to the %s\n",
					 userOutput[i]);
				gV4LTestConfig.mHeight = heightTable[i];
				gV4LTestConfig.mWidth = widthTable[i];
				/* Perform global test setup, call setup() function. */
				setup();
				/* Test Case Body. */
				ret |= VT_v4l_capture_test();
				gExitCleanup = 0;
				cleanup();
			}
			if (ret == TPASS) {
				tst_resm(TPASS,
					 "%s test case worked as expected",
					 TCID);
			} else
				tst_resm(TFAIL,
					 "%s test case did NOT work as expected",
					 TCID);
			return ret;
		} else if ((Sflag) && (gV4LTestConfig.mCaseNum != 1)) {
			tst_resm(TBROK,
				 "Rotation not supported while capturing");
			return TFAIL;
		}
		/* Perform global test setup, call setup() function. */
		setup();
		/* Print test Assertion using tst_resm() function with argument TINFO. */
		tst_resm(TINFO, "Testing if %s test case is OK", TCID);
		/* Test Case Body. */
		VT_rv = VT_v4l_capture_test();
		gExitCleanup = 0;
		if (Eflag) {	/* threat error as PASS */
			if (VT_rv == TFAIL)
				VT_rv = TPASS;
			else
				VT_rv = TFAIL;
		}
		if (VT_rv == TPASS)
			tst_resm(TPASS, "%s test case worked as expected",
				 TCID);
		else
			tst_resm(TFAIL, "%s test case did NOT work as expected",
				 TCID);
		cleanup();
		return VT_rv;

	}
/*======================== GLOBAL FUNCTIONS =================================*/
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
	void cleanup(void) {
		int VT_rv;
		if (gV4LTestConfig.mOutputFile)
			free(gV4LTestConfig.mOutputFile);
		gV4LTestConfig.mOutputFile = NULL;
		VT_rv = VT_v4l_capture_cleanup();
    	system("v4l_module.sh cleanup");	
		if (gExitCleanup) {
			if (VT_rv != TPASS) {
				tst_resm(TWARN,
					 "VT_cleanup() Failed : error code = %d",
					 VT_rv);
			}
			tst_exit();
		}
	}

#ifdef __cplusplus
}
#endif
