/*
 * Copyright (C) 2010 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*
 * <p> Title: EDPC Frame buffer test </p>
 * <p> Description: this file include test cases for epdc </p>
 * <p> Copyright: 2010 Freescale Semiconductor, Inc. </p>
 * <p> Company: Freescale Semiconductor, Inc. </p>
 * @author: b20222
 * @file   epdc_test.c
 * @version: 9
*/
#ifdef __cplusplus
extern "C" {
#endif

/* Standard Include Files */
#include <sys/types.h>		/* open()                          */
#include <sys/stat.h>		/* open()                          */
#include <fcntl.h>		/* open()                          */
#include <unistd.h>		/* close()                         */
#include <stdio.h>		/* tst_resm(TINFO,), fgetc(), stdin        */
#include <errno.h>		/* errno                           */
#include <string.h>		/* strerror()                      */
#include <sys/ioctl.h>		/* ioctl()                         */
#include <sys/mman.h>		/* mmap(), munmap()                */
#include <linux/fb.h>		/* framebuffer related information */
#include <asm/types.h>
#include <time.h>
#include <linux/mxcfb.h>
#include <linux/pxp_device.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "epdc_test.h"
#ifdef USE_PIC
#include "soc_blk.c"
#include "diasy.c"
#endif
/*********************************************************************************/
/* Macro name:  CALL_IOCTL()                                                     */
/* Description: Macro checks for any error in function execution                 */
/*              based on the return value. In case of error, the function exits. */
/*********************************************************************************/
#define CALL_IOCTL(ioctl_cmd)\
{ \
    if( (ioctl_cmd) < 0 )\
    {\
        tst_resm( TFAIL, "%s : %s fails #%d [File: %s, line: %d]", __FUNCTION__, "ioctl_", errno, __FILE__, __LINE__);\
        perror("ioctl"); \
        return FALSE;\
    }\
}


#define  ALIGN_PIXEL_128(x)  ((x+ 127) & ~127)

#define EPDC_STR_ID		"mxc_epdc_fb"
#define PXP_DEVICE_NAME "/dev/pxp_device"
#define BUFFER_WIDTH 256
#define BUFFER_HEIGHT 256
#define PXP_BUFFER_SIZE (BUFFER_WIDTH*BUFFER_HEIGHT)
#define MAX_WAIT 4
/*update interval in us*/
#define UPDATE_INTERVAL 1000
#define PR (unsigned char)(0.257*256)
#define PG (unsigned char)(0.504*256)
#define PB (unsigned char)(0.098*256)
/*global virables*/
	extern epdc_opts m_opt;

/*internal virables*/
	int fb_fd;		/* Framebuffer device file descriptor            */
	struct fb_fix_screeninfo fb_info;	/* Framebuffer constant information */
	unsigned char *fb_mem_ptr;	/* Pointer to the mapped momory of the fb device */
	sigset_t sigset;
	static int quitflag = 0;
	static int g_fb_size;

	unsigned long last_t = 0;
/*local functions*/
	int get_modeinfo(struct fb_var_screeninfo *info);
	void print_fbinfo(void);
	unsigned char *draw_px(unsigned char *where, struct pixel *p);
	int draw_pattern(int fd, unsigned char *pfb, int r, int g, int b);
	int draw_pattern_diasy(unsigned char *pfb, int gwidth, int gheight);
	int draw_pattern_pic(int fd, unsigned char *pfb, int r, int g, int b);
	BOOL pan_test();
	BOOL draw_test();
	BOOL full_update();
	static BOOL test_power_delay();

	inline unsigned char BOUND255(short a) {
		return (a > 255) ? 255 : (a < 0) ? 0 : a;
	}
/*
 * <p>Draw a 512*512 text image </p>
 * <p>procedure:</p> 
 * <p>1. setting up update_data structure. </p>
 * <p>2. draw a text pic in framebeuffer. </p>
 * <p>3. send update ioctl, and wait finish. </p>
 * <p>4. check whether timeout occurs </p>
 * @param: hard coded, region: full screen
 * @param: hard coded, waveform mode:auto
 * @param: hard coded, update mode: partial
 * @param: hard coded, update marker: 0x111
 * @param: hard coded, tempture: ambient temperature
 * @param: hard coded, alt buffer: no use
 * @returns: TRUE success, FALSE failure.
 * @error: ioctl fail or update request timeout
 */ BOOL draw_test() {
		int wait_time = 0;
		int update_marker = 0x101;
		struct fb_var_screeninfo mode_info;
		struct mxcfb_update_data im_update = {
			{0, 0, 16, 16},	/*region round to 8 */
			257,	/*waveform mode 0-255, 257 auto */
			0,	/*update mode 0(partial),1(Full) */
			update_marker,	/*update_marker assigned by user */
			TEMP_USE_AMBIENT,	/*use ambient temperature set */
			0,	/*do not use alt buffer */
			{0, 0, 0, {0, 0, 0, 0}}
		};

		im_update.flags = m_opt.update.flags;
		CALL_IOCTL(ioctl(fb_fd, FBIOGET_VSCREENINFO, &mode_info));
		im_update.update_region.width = mode_info.xres;
		im_update.update_region.height = mode_info.yres;
		tst_resm(TINFO, "draw a white in bg ground");
		if (TPASS != draw_pattern_pic(fb_fd, fb_mem_ptr, 255, 255, 255)) {
			tst_resm(TINFO, "fail to draw patter on bg");
			return FALSE;
		}
		tst_resm(TINFO, "updating the screen now\n");
		if (m_opt.au != -1)
			return TRUE;	/*for auto update, not needs to send update request */
		while (ioctl(fb_fd, MXCFB_SEND_UPDATE, &im_update) < 0) {
			static int iloops = 0;
			if (iloops++ > MAX_WAIT) {
				iloops = 0;
				break;
			}
			sleep(1);
		}
	if(1){
	struct mxcfb_update_marker_data upd_marker_data;
	upd_marker_data.update_marker = im_update.update_marker;
	while(ioctl(fb_fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE,&upd_marker_data) < 0) {
			wait_time++;
			if (wait_time > MAX_WAIT) {
				tst_resm(TINFO,
					 "full mode wait time exceed!!!\n");
				return FALSE;
			}
		}
	}
		return TRUE;
	}

/*
 * <p>Pan test </p>
 * <p>procedure:</p> 
 * <p>1. double the yres size </p>
 * <p>2. remap the framebuffer memory </p>
 * <p>3. draw a text pattern in framebuffer </p>
 * <p>4. send y pan 8 times in step of 1/8 screen height</p>
 * <p>5. reset pan setting</p>
 * @returns: TRUE success, FALSE failure.
 * @error: ioctl fail
 */
	BOOL pan_test() {
		int y = 0, old_yvres;
		int wait_time = 0;
		int update_marker = 0x101;
		struct fb_var_screeninfo mode_info;
		struct mxcfb_update_data im_update = {
			{0, 0, 16, 16},	/*region round to 8 */
			257,	/*waveform mode 0-255, 257 auto */
			1,	/*update mode 0(partial),1(Full) */
			update_marker,	/*update_marker assigned by user */
			TEMP_USE_AMBIENT,	/*use ambient temperature set */
			0,	/*do not use alt buffer */
			{0, 0, 0, {0, 0, 0, 0}}
		};
/*x pan is not supported*/
		im_update.flags = m_opt.update.flags;
#if 1
		tst_resm(TINFO, "test fb0 pan");
		CALL_IOCTL(ioctl(fb_fd, FBIOGET_VSCREENINFO, &mode_info));
		old_yvres = mode_info.yres_virtual;
		mode_info.yres_virtual = mode_info.yres * 2;
		CALL_IOCTL(ioctl(fb_fd, FBIOPUT_VSCREENINFO, &mode_info));
		im_update.update_region.width = mode_info.xres;
		im_update.update_region.height = mode_info.yres;
		/*remap the devices */
		munmap(fb_mem_ptr, fb_info.smem_len);
		CALL_IOCTL(ioctl(fb_fd, FBIOGET_FSCREENINFO, &fb_info));
		fb_mem_ptr =
		    mmap(NULL, fb_info.smem_len, PROT_READ | PROT_WRITE,
			 MAP_SHARED, fb_fd, 0);
		if ((int)fb_mem_ptr == -1) {
			tst_brkm(TFAIL, cleanup,
				 "Can't map framebuffer device into memory: %s\n",
				 strerror(errno));
		}

		tst_resm(TINFO, "draw a white screen in back ground");
		if (TPASS != draw_pattern(fb_fd, fb_mem_ptr, 255, 255, 255)) {
			tst_resm(TINFO, "fail to draw patter on fb0");
			return FALSE;
		}
		sleep(5);
		for (y = 0; y <= mode_info.yres; y += mode_info.yres / 8) {
			mode_info.yoffset = y;
			tst_resm(TINFO, "\r offset at %d\n", y);
			CALL_IOCTL(ioctl(fb_fd, FBIOPAN_DISPLAY, &mode_info));
			if (m_opt.au != -1)
				continue;	/*for auto update, not needs to send update request */
			while (ioctl(fb_fd, MXCFB_SEND_UPDATE, &im_update) < 0) {
				static int iloops = 0;
				if (iloops++ > MAX_WAIT) {
					iloops = 0;
					break;
				}
				sleep(1);
			}
			if(1){
				struct mxcfb_update_marker_data upd_marker_data;
				upd_marker_data.update_marker = im_update.update_marker;
				while(ioctl(fb_fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE,
					&upd_marker_data) < 0) {
					wait_time++;
					if (wait_time > MAX_WAIT) {
						tst_resm(TINFO,
							 "full mode wait time exceed!!!\n");
						break;
					}
				}
			}

			sleep(1);
		}
		/*reset pan postion */
		mode_info.yoffset = 0;
		CALL_IOCTL(ioctl(fb_fd, FBIOPAN_DISPLAY, &mode_info));
		mode_info.yres_virtual = old_yvres;
		CALL_IOCTL(ioctl(fb_fd, FBIOPUT_VSCREENINFO, &mode_info));
#endif
		return TRUE;
	}

	static BOOL update_once(void *p_update) {
		/*step 1: set up update data */
		int wait_time = 0;
		int count = 1;
		unsigned long st = 0i, et = 0;
		struct timeval tv;
		struct timezone tz;
		struct mxcfb_update_data *p_im_update =
		    (struct mxcfb_update_data *)p_update;
		/*do not use alt buffer */
		p_im_update->flags = 0;
		/*step 3: now using full update mode */
		count = 1;
		p_im_update->update_mode = 1;
		gettimeofday(&tv, &tz);
		st = tv.tv_usec + tv.tv_sec * 1000000;
		while (count--) {
			/*black and white alternative */
			if (m_opt.au != -1)
				continue;	/*for auto update, not needs to send update request */
			while (ioctl(fb_fd, MXCFB_SEND_UPDATE, p_im_update) < 0) {
				static int iloops = 0;
				if (iloops++ > MAX_WAIT) {
					iloops = 0;
					break;
				}
				sleep(1);
			}
			if(1){
				struct mxcfb_update_marker_data upd_marker_data;
				upd_marker_data.update_marker = p_im_update->update_marker;
				while(ioctl(fb_fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE,
					&upd_marker_data) < 0) {
					wait_time++;
					if (wait_time > MAX_WAIT) {
						tst_resm(TINFO,
							 "full mode wait time exceed!!!\n");
						break;
					}
				}
			}
			gettimeofday(&tv, &tz);
			et = tv.tv_usec + tv.tv_sec * 1000000;
			if (et > st)
				last_t += (et - st);
			else
				tst_resm(TINFO,
					 "WARNING!!! gettimeof day wrong");
			wait_time = 0;
			/*tst_resm(TINFO,"next update\n"); */
		}
		return TRUE;
	}

/*
 * <p>test power delay </p>
 * <p>procedure:</p> 
 * <p>1. set and get the power delay </p>
 * <p>2. verify the setting is OK </p>
 * <p>3. update screen once </p>
 * <p>4. wait 1 second, the framebuffer will powerdown</p>
 * <p>5. update screen again</p>
 * <p>6. verify the update is OK.</p>
 * @returns: TRUE success, FALSE failure.
 * @error: ioctl fail or update fail.
 */
	static BOOL test_power_delay() {
		int id = FB_POWERDOWN_DISABLE;
		CALL_IOCTL(ioctl
			   (fb_fd, MXCFB_SET_PWRDOWN_DELAY, &(m_opt.delay)));
		CALL_IOCTL(ioctl(fb_fd, MXCFB_GET_PWRDOWN_DELAY, &id));
		tst_resm(TINFO, "delay time set to %dms\n", id);
		if (id != m_opt.delay)
			return FALSE;
		if (!full_update())
			return FALSE;
		sleep(1);
		if (!full_update())
			return FALSE;

		id = FB_POWERDOWN_DISABLE;
		CALL_IOCTL(ioctl(fb_fd, MXCFB_SET_PWRDOWN_DELAY, &id));
		return TRUE;
	}

	static BOOL gray_scale_test() {
		int i, j;
		int wait_time = 0;
		int update_marker = 0x101;
		struct fb_var_screeninfo mode_info;
		struct fb_fix_screeninfo fx_fb_info;	/* Framebuffer constant information */
		unsigned char *fb_wr_ptr = fb_mem_ptr;
		struct pixel px;
		struct mxcfb_update_data im_update = {
			{0, 0, 16, 16},	/*region round to 8 */
			257,	/*waveform mode 0-255, 257 auto */
			0,	/*update mode 0(partial),1(Full) */
			update_marker,	/*update_marker assigned by user */
			TEMP_USE_AMBIENT,	/*use ambient temperature set */
			0,	/*do not use alt buffer */
			{0, 0, 0, {0, 0, 0, 0}}
		};

		im_update.flags = m_opt.update.flags;
		CALL_IOCTL(ioctl(fb_fd, FBIOGET_VSCREENINFO, &mode_info));
		im_update.update_region.width = mode_info.xres;
		im_update.update_region.height = mode_info.yres;
		tst_resm(TINFO, "draw a gray scale");
		CALL_IOCTL(ioctl(fb_fd, FBIOGET_FSCREENINFO, &fx_fb_info));
		/* Fill in the px struct */
		px.bpp = mode_info.bits_per_pixel / 8;
		px.xres =
		    mode_info.xres % 32 ==
		    0 ? mode_info.xres : mode_info.xres_virtual;
		px.yres = mode_info.yres;
		px.r_field.offset = mode_info.red.offset;
		px.r_field.length = mode_info.red.length;
		px.g_field.offset = mode_info.green.offset;
		px.g_field.length = mode_info.green.length;
		px.b_field.offset = mode_info.blue.offset;
		px.b_field.length = mode_info.blue.length;
		px.trans = 0x00;
		px.line_length = fx_fb_info.line_length / px.bpp;
		px.r_color = px.g_color = px.b_color = 0;
		for (j = 0; j < mode_info.yres; j++) {
			px.r_color = px.g_color = px.b_color = 0;
			for (i = 0; i < mode_info.xres_virtual; i++) {
				px.r_color =
				    i % 2 == 0 ? px.r_color + 1 : px.r_color;
				px.r_color = px.r_color > 255 ? 0 : px.r_color;
				px.g_color = px.r_color;
				px.b_color = px.r_color;	/* Set color values */
				fb_wr_ptr = draw_px(fb_wr_ptr, &px);
			}
		}
		tst_resm(TINFO, "updating the screen now\n");
		if (m_opt.au != -1)
			return TRUE;	/*for auto update, not needs to send update request */
		while (ioctl(fb_fd, MXCFB_SEND_UPDATE, &im_update) < 0) {
			static int iloops = 0;
			if (iloops++ > MAX_WAIT) {
				iloops = 0;
				break;
			}
			sleep(1);
		}
		if(1){
			struct mxcfb_update_marker_data upd_marker_data;
			upd_marker_data.update_marker = im_update.update_marker;
			while(ioctl(fb_fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE,
				&upd_marker_data) < 0) {
				wait_time++;
				if (wait_time > MAX_WAIT) {
					tst_resm(TINFO,
						 "full mode wait time exceed!!!\n");
					return FALSE;
				}
			}
		}
		return TRUE;
	}

	static BOOL single_update(void *p_update) {
		/*step 1: set up update data */
		int wait_time = 0;
		int count = 1;
		pid_t tid = syscall(SYS_gettid);
		struct mxcfb_update_data *p_im_update =
		    (struct mxcfb_update_data *)p_update;
		/*do not use alt buffer */
		//p_im_update->flags = 0;
		tst_resm(TINFO,
			 "process %d runing at t= %d, l = %d, w= %d, h = %d\n",
			 tid, p_im_update->update_region.top,
			 p_im_update->update_region.left,
			 p_im_update->update_region.width,
			 p_im_update->update_region.height);

		/*step 2: update and wait finished */
		while (count--) {
			/*black and white alternative */
#if 1
			if (m_opt.au != -1)
				continue;	/*for auto update, not needs to send update request */
			while (ioctl(fb_fd, MXCFB_SEND_UPDATE, p_im_update) < 0) {
				static int iloops = 0;
				if (iloops++ > MAX_WAIT) {
					iloops = 0;
					break;
				}
				sleep(1);
			}
			if(1){
				struct mxcfb_update_marker_data upd_marker_data;
				upd_marker_data.update_marker = p_im_update->update_marker;
				while(ioctl(fb_fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE,
					&upd_marker_data) < 0) {
					wait_time++;
					if (wait_time > MAX_WAIT) {
						tst_resm(TINFO,
							 "wait time exceed!!!\n");
						break;
					}
				}
			}
#endif
			usleep(100);
			tst_resm(TINFO, "process %d update\n", tid);
			if (quitflag)
				goto QUIT;
			wait_time = 0;
		}
		/*step 3: now using full update mode */
		count = 1;
		p_im_update->update_mode = 1;
		while (count--) {
			/*black and white alternative */
#if 1
			if (m_opt.au != -1)
				continue;	/*for auto update, not needs to send update request */
			while (ioctl(fb_fd, MXCFB_SEND_UPDATE, p_im_update) < 0) {
				static int iloops = 0;
				if (iloops++ > MAX_WAIT) {
					iloops = 0;
					break;
				}
				sleep(1);
			}
			if(1){
				struct mxcfb_update_marker_data upd_marker_data;
				upd_marker_data.update_marker = p_im_update->update_marker;
				while(ioctl(fb_fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE,
					&upd_marker_data) < 0) {
					wait_time++;
					if (wait_time > MAX_WAIT) {
						tst_resm(TINFO,
							 "full mode wait time exceed!!!\n");
						return FALSE;
					}
				}
			}
#endif
			usleep(100);
			tst_resm(TINFO, "process %d full update\n", tid);
			if (quitflag)
				goto QUIT;
			wait_time = 0;
		}
	      QUIT:
		tst_resm(TINFO, "process %d quit\n", tid);
		return TRUE;
	}

	static void draw_thread(int *pon) {
		while (*pon == 1) {
			if (quitflag)
				break;
#ifdef USE_PIC
			draw_pattern_pic(fb_fd, fb_mem_ptr, 255, 255, 255);
#else
			draw_pattern(fb_fd, fb_mem_ptr, 255, 255, 255);
#endif
			usleep(UPDATE_INTERVAL);
			draw_pattern(fb_fd, fb_mem_ptr, 0, 0, 0);
			usleep(UPDATE_INTERVAL);
		}
		tst_resm(TINFO, "draw quit\n");
	}

	static int signal_thread(void *arg) {
		int sig, err;
		pthread_sigmask(SIG_BLOCK, &sigset, NULL);
		while (1) {
			err = sigwait(&sigset, &sig);
			if (sig == SIGINT) {
				tst_resm(TINFO, "Ctrl-C received\n");
			} else {
				tst_resm(TINFO,
					 "Unknown signal. Still exiting\n");
			}
			quitflag = 1;
			break;
		}
		return 0;
	}

/*
 * <p>test update rate</p>
 * <p>procedure:</p> 
 * <p>1. devide the update region half screen</p>
 * <p>2. update the left half. culmulate the updating time</p>
 * <p>2. update the right half. culmulate the updating time</p>
 * <p>3. loop 1000 times </p>
 * <p>4. calculate the average update time for 2 halves</p>
 * @returns: TRUE success, FALSE failure.
 * @error: ioctl fail or update fail.
 */
	BOOL test_rate_update() {
#define FRAME_CNT 100
		struct mxcfb_update_data im_update[2];
		int i = 0;
		struct fb_var_screeninfo mode_info;
		CALL_IOCTL(ioctl(fb_fd, FBIOGET_VSCREENINFO, &mode_info));
		while (i++ < FRAME_CNT) {
			int x, y, w, h;
			x = 0;
			y = 0;
			w = (mode_info.xres >> 1) & (~0x03);
			h = mode_info.yres;
			im_update[0].update_region.top = y;
			im_update[0].update_region.left = x;
			im_update[0].update_region.width = w;
			im_update[0].update_region.height = h;
			im_update[0].waveform_mode = 257;
			im_update[0].update_marker = 0x300;
			im_update[0].temp = 24;
#ifdef USE_PIC
			draw_pattern_pic(fb_fd, fb_mem_ptr, 255, 255, 255);
#else
			draw_pattern(fb_fd, fb_mem_ptr, 255, 255, 255);
#endif
			update_once(&(im_update[0]));
			x = (mode_info.xres >> 1) & (~0x03);
			y = 0;
			w = (mode_info.xres >> 1) & (~0x03);
			h = mode_info.yres;
			im_update[1].update_region.top = y;
			im_update[1].update_region.left = x;
			im_update[1].update_region.width = w;
			im_update[1].update_region.height = h;
			im_update[1].waveform_mode = 257;
			im_update[1].update_marker = 0x301;
			im_update[1].temp = 24;
			update_once(&(im_update[1]));
		}
		if (last_t > 0)
			tst_resm(TINFO, "total update fps is:%f\n",
				 (float)((FRAME_CNT * 1000000.0f * 2) /
					 last_t));
		return TRUE;
	}

/*
 * <p>test max update</p>
 * <p>procedure:</p> 
 * <p>1. devide screen into 4x4 blocks</p>
 * <p>2. create a thread to draw a image then black alternatively</p> 
 * <p>3. in each blocks create a thread to send update 10 times</p>
 * <p>2. the parent thread wait all child to finish</p>
 * @param: hard coded, waveform mode:auto
 * @param: hard coded, update mode: partial
 * @param: hard coded, tempture: ambient temperature
 * @returns: TRUE success, FALSE failure.
 * @error: ioctl fail or update fail.
 */
	BOOL test_max_update() {
		int state = 1;
		int ret = 0;
		int i = 0, j = 0;
		int id = 0;
#define MAX_CNT_X 4
#define MAX_CNT_Y 4
		struct mxcfb_update_data im_update[MAX_CNT_X * MAX_CNT_Y];
		pthread_t sigtid, drawid, updates_id[MAX_CNT_X * MAX_CNT_Y];
		sigemptyset(&sigset);
		sigaddset(&sigset, SIGINT);
		pthread_sigmask(SIG_BLOCK, &sigset, NULL);
		pthread_create(&sigtid, NULL, (void *)&signal_thread, NULL);
		pthread_create(&drawid, NULL, (void *)&draw_thread, &state);
		if (state) {
			int cn = 0;
			struct fb_var_screeninfo mode_info;
			CALL_IOCTL(ioctl
				   (fb_fd, FBIOGET_VSCREENINFO, &mode_info));
			memset(im_update, 0, sizeof(im_update));
			for (i = 0; i < MAX_CNT_X; i++)
				for (j = 0; j < MAX_CNT_Y; j++) {
					int x, y, w, h;
					x = i * (mode_info.xres / MAX_CNT_X);
					y = j * (mode_info.yres / MAX_CNT_Y);
					w = (mode_info.xres /
					     MAX_CNT_X) & (~0x03);
					h = (mode_info.yres /
					     MAX_CNT_Y) & (~0x03);
					im_update[cn].update_region.top = y;
					im_update[cn].update_region.left = x;
					im_update[cn].update_region.width = w;
					im_update[cn].update_region.height = h;
					im_update[cn].waveform_mode = 257;
					im_update[cn].update_marker = i + 0x200;
					im_update[cn].temp = 24;
					pthread_create(&(updates_id[id++]),
						       NULL,
						       (void *)&single_update,
						       (void *)
						       &(im_update[cn]));
					tst_resm(TINFO, "carete pid %d\n",
						 (int)updates_id[i]);
					cn++;
				}
			for (i = 0; i < (MAX_CNT_X * MAX_CNT_Y); i++) {
				int pret = 0;
				if (updates_id[i] != 0)
					pthread_join(updates_id[i],
						     (void **)&(pret));
				ret = ((BOOL) pret) ? 0 : ret + 1;
				tst_resm(TINFO, "%d return with %s\n",
					 (int)updates_id[i],
					 ret == 0 ? "OK" : "FAIL");
			}
			state = 0;
			//pthread_join(sigtid,NULL);
			pthread_join(drawid, NULL);
		}
		state = 0;
		return ret == 0 ? TRUE : FALSE;
	}

/*
 * <p>test collision update</p>
 * <p>procedure:</p> 
 * <p>1. setup update data</p>
 * <p>2. get phy memory for alt buffer, and memory map it</p> 
 * <p>3. draw a daisy image to alt buffet</p>
 * <p>4. loop 20 time for below</p>
 * <p>5. in odd loop draws black pattern in frame buffer, while draws white in even </p>
 * <p>6. send regional update ioctl with alt buffer enabled</p>
 * <p>7. shift the update region x,y at offset 5. </p>
 * @param: hard coded, region update size: BUFFER_WIDTH,BUFFER_HEIGH
 * @param: hard coded, waveform mode:auto
 * @param: hard coded, update mode: partial
 * @param: hard coded, tempture: ambient temperature
 * @param: alt buffer: enable
 * @param: alt buffer size: usergiven or use PXP_BUFFER_SIZE
 * @returns: TRUE success, FALSE failure.
 * @error: ioctl fail or update fail.
 */
	BOOL test_collision_update() {
		int state = 1;
		int ret = 0;
		int i = 0, j = 0;
		int id = 0;
#define CNT_X 2
#define CNT_Y 2
		struct mxcfb_update_data im_update[CNT_X * CNT_Y];
		pthread_t updates_id[CNT_X * CNT_Y];
		sigemptyset(&sigset);
		sigaddset(&sigset, SIGINT);
		pthread_sigmask(SIG_BLOCK, &sigset, NULL);
		if (TPASS != draw_pattern_pic(fb_fd, fb_mem_ptr, 255, 255, 255)) {
			tst_resm(TINFO, "fail to draw patter on bg");
			return FALSE;
		}
		if (state) {
			int cn = 0;
			struct fb_var_screeninfo mode_info;
			CALL_IOCTL(ioctl
				   (fb_fd, FBIOGET_VSCREENINFO, &mode_info));
			memset(im_update, 0, sizeof(im_update));
			for (i = 0; i < CNT_X; i++)
				for (j = 0; j < CNT_Y; j++) {
					int x, y, w, h;
					x = i * (mode_info.xres / CNT_X);
					y = j * (mode_info.yres / CNT_Y);
					w = (mode_info.xres / CNT_X) & (~0x03);
					h = (mode_info.yres / CNT_Y) & (~0x03);
					/*overlap a bit */
					x = x > 32 ? x - 16 : x;
					y = y > 32 ? y - 16 : y;
					im_update[cn].update_region.top = y;
					im_update[cn].update_region.left = x;
					im_update[cn].update_region.width = w;
					im_update[cn].update_region.height = h;
					im_update[cn].waveform_mode = 257;
					im_update[cn].update_marker = i + 0x200;
					im_update[cn].temp = 24;
					pthread_create(&(updates_id[id++]),
						       NULL,
						       (void *)&single_update,
						       (void *)
						       &(im_update[cn]));
					tst_resm(TINFO, "carete pid %d\n",
						 (int)updates_id[i]);
					cn++;
				}
			for (i = 0; i < (CNT_X * CNT_Y); i++) {
				int pret = 0;
				if (updates_id[i] != 0)
					pthread_join(updates_id[i],
						     (void **)&(pret));
				ret = ((BOOL) pret) ? 0 : ret + 1;
				tst_resm(TINFO, "%d return with %s\n",
					 (int)updates_id[i],
					 ret == 0 ? "OK" : "FAIL");
			}
			state = 0;
		}
		state = 0;
		return ret == 0 ? TRUE : FALSE;
	}

/*
 * <p>test alt update</p>
 * <p>procedure:</p> 
 * <p>1. setup update data</p>
 * <p>2. get phy memory for alt buffer, and memory map it</p> 
 * <p>3. draw a daisy image to alt buffet</p>
 * <p>4. loop 20 time for below</p>
 * <p>5. in odd loop draws black pattern in frame buffer, while draws white in even </p>
 * <p>6. send regional update ioctl with alt buffer enabled</p>
 * <p>6. send full update ioctl with alt buffer enabled</p>
 * @param: hard coded, region update size: BUFFER_WIDTH,BUFFER_HEIGH
 * @param: hard coded, waveform mode:auto
 * @param: hard coded, update mode: partial
 * @param: hard coded, tempture: ambient temperature
 * @param: alt buffer: enable
 * @param: alt buffer size: usergiven or use PXP_BUFFER_SIZE
 * @returns: TRUE success, FALSE failure.
 * @error: ioctl fail or update fail.
 */
	BOOL test_alt_update() {
		BOOL ret = FALSE;
		int wait_time = 0;
		int fd_pxp;
		int count = 1;
		int update_marker = 0x112;
		struct pxp_mem_desc mem;
		struct mxcfb_update_data im_update = {
			{0, 0, BUFFER_WIDTH, BUFFER_HEIGHT},	/*region round to 8 */
			257,	/*waveform mode 0-255, 257 auto */
			0,	/*update mode 0(partial),1(Full) */
			update_marker,	/*update_marker assigned by user */
			TEMP_USE_AMBIENT,	/*use ambient temperature set */
			EPDC_FLAG_USE_ALT_BUFFER,	/*enable alt buffer */
			{0, 0, 0, {0, 0, 0, 0}}	/*set this later */
		};
		tst_resm(TINFO, "start alt update test\n");
/*step 1: set up update data*/
		fd_pxp = open(PXP_DEVICE_NAME, O_RDWR, 0);
		if (fd_pxp < 0) {
			tst_resm(TINFO, "open pxp devices fialed\n");
			return FALSE;
		}
		if (m_opt.update.alt_buffer_data.width *
		    m_opt.update.alt_buffer_data.height > 0)
			mem.size =
			    m_opt.su ==
			    1 ? (m_opt.update.alt_buffer_data.width *
				 m_opt.update.alt_buffer_data.height) *
			    2 : PXP_BUFFER_SIZE * 2;
		else
			mem.size = PXP_BUFFER_SIZE * 2;
		tst_resm(TINFO, "try to get memory size %d\n", mem.size);
/*#define USE_PMEM*/
#ifdef USE_PMEM
		if (ioctl(fd_pxp, PXP_IOC_GET_PHYMEM, &mem) < 0) {
			mem.phys_addr = 0;
			mem.cpu_addr = 0;
			tst_resm(TINFO, "get memory failed\n");
			goto END;
		}
#else
		{
			struct fb_var_screeninfo screen_info;
			struct fb_fix_screeninfo fix_screen_info;
			CALL_IOCTL(ioctl
				   (fb_fd, FBIOGET_FSCREENINFO,
				    &fix_screen_info));
			CALL_IOCTL(ioctl
				   (fb_fd, FBIOGET_VSCREENINFO, &screen_info));
			mem.phys_addr =
			    fix_screen_info.smem_start +
			    screen_info.xres_virtual *
			    ALIGN_PIXEL_128(screen_info.yres) *
			    screen_info.bits_per_pixel / 8;
		}
#endif
#if 1
		{
			int fd_mem = open("/dev/mem", O_RDWR);
			if (fd_mem < 0) {
				tst_resm(TINFO, "open mem device error\n");
				goto END;
			}
			mem.virt_uaddr = (unsigned long)mmap(NULL, mem.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_mem, mem.phys_addr);	/* + 0x70000000); */
			close(fd_mem);
		}
#else
		mem.virt_uaddr = (unsigned long)mmap(NULL, mem.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_pxp, mem.phys_addr);	/* + 0x70000000); */
#endif
		if (mem.virt_uaddr == 0) {
			tst_resm(TINFO, "virtual address error!\n");
			goto END;
		}
		tst_resm(TINFO, "success get memory at %x\n", mem.virt_uaddr);
		if (m_opt.su == 1) {
			memcpy(&im_update, &m_opt.update,
			       sizeof(struct mxcfb_update_data));
		} else {
			im_update.alt_buffer_data.width = BUFFER_WIDTH;
			im_update.alt_buffer_data.height = BUFFER_HEIGHT;
			im_update.alt_buffer_data.alt_update_region.top = 0;
			im_update.alt_buffer_data.alt_update_region.left = 0;
			im_update.alt_buffer_data.alt_update_region.width =
			    BUFFER_WIDTH;
			im_update.alt_buffer_data.alt_update_region.height =
			    BUFFER_HEIGHT;
		}
		im_update.alt_buffer_data.phys_addr = mem.phys_addr;
#ifdef USE_PIC
		draw_pattern_diasy((unsigned char *)mem.virt_uaddr,
				   BUFFER_WIDTH, BUFFER_HEIGHT);
#else
		for (i = 0; i < BUFFER_HEIGHT; i++)
			for (j = 0; j < BUFFER_WIDTH; j++) {
				((unsigned char *)mem.virt_uaddr)[2 *
								  (i *
								   BUFFER_HEIGHT
								   + j)] = 128;
				((unsigned char *)mem.virt_uaddr)[2 *
								  (i *
								   BUFFER_HEIGHT
								   + j) + 1] =
				    128;
			}
#endif
		tst_resm(TINFO, "start test\n");
/*step 2: start test*/
		/*partial update */
		while (count--) {
			/*black and white alternative */
			if (count & 0x01)
				draw_pattern(fb_fd, fb_mem_ptr, 255, 255, 255);
			else
				draw_pattern(fb_fd, fb_mem_ptr, 0, 0, 0);
			if (m_opt.au != -1)
				continue;	/*for auto update, not needs to send update request */
			while (ioctl(fb_fd, MXCFB_SEND_UPDATE, &im_update) < 0) {
				static int iloops = 0;
				if (iloops++ > MAX_WAIT) {
					iloops = 0;
					break;
				}
				sleep(1);
			}
			if(1){
				struct mxcfb_update_marker_data upd_marker_data;
				upd_marker_data.update_marker = im_update.update_marker;
				while(ioctl(fb_fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE,
					&upd_marker_data) < 0) {
						wait_time++;
						if (wait_time > MAX_WAIT) {
							tst_resm(TINFO,
						 		"wait time exceed!!!\n");
							return FALSE;
						}
				}
			}
			wait_time = 0;
			tst_resm(TINFO, "partial mode next update\n");
		}
		/*full update */
		count = 1;
		im_update.update_mode = 1;
		while (count--) {
			/*black and white alternative */
			if (count & 0x01)
				draw_pattern(fb_fd, fb_mem_ptr, 255, 255, 255);
			else
				draw_pattern(fb_fd, fb_mem_ptr, 0, 0, 0);
			if (m_opt.au != -1)
				continue;	/*for auto update, not needs to send update request */
			while (ioctl(fb_fd, MXCFB_SEND_UPDATE, &im_update)) {
				static int iloops = 0;
				if (iloops++ > MAX_WAIT) {
					iloops = 0;
					break;
				}
				sleep(1);
			}
			if(1){
				struct mxcfb_update_marker_data upd_marker_data;
				upd_marker_data.update_marker = im_update.update_marker;
				while(ioctl(fb_fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE,
					&upd_marker_data) < 0) {
					wait_time++;
					if (wait_time > MAX_WAIT) {
						tst_resm(TINFO,
							 "wait time exceed!!!\n");
						return FALSE;
					}
				}
			}
			wait_time = 0;
			tst_resm(TINFO, "full mode next update\n");
		}

		/*step 4: clean up */
#ifdef USE_PMEM
		if (ioctl(fd_pxp, PXP_IOC_PUT_PHYMEM, &mem) < 0) {
			mem.phys_addr = 0;
			mem.cpu_addr = 0;
			goto END;
		}
#endif
		ret = TRUE;
	      END:
		if (fd_pxp > 0)
			close(fd_pxp);
		return ret;
	}

/*
 * <p>test full update</p>
 * <p>procedure:</p> 
 * <p>1. setup update data</p>
 * <p>2. draw txt image to framebuffer. </p>
 * <p> send update ioctl. </p.
 * @param: hard coded, region update size: full screen
 * @param: hard coded, waveform mode:auto
 * @param: hard coded, update mode: partial
 * @param: hard coded, tempture: ambient temperature
 * @param: alt buffer: disable
 * @returns: TRUE success, FALSE failure.
 * @error: ioctl fail or update fail.
 */
	BOOL full_update() {
		/*step 1: set up update data */
		int wait_time = 0;
		int update_marker = 0x001;
		struct mxcfb_update_data im_update = {
			{0, 0, 16, 16},	/*region round to 8 */
			257,	/*waveform mode 0-255, 257 auto */
			1,	/*update mode 0(partial),1(Full) */
			update_marker,	/*update_marker assigned by user */
			TEMP_USE_AMBIENT,	/*use ambient temperature set */
			0,	/*do not use alt buffer */
			{0, 0, 0, {0, 0, 0, 0}}
		};
		struct fb_var_screeninfo mode_info;
		CALL_IOCTL(ioctl(fb_fd, FBIOGET_VSCREENINFO, &mode_info));
		im_update.update_region.width = mode_info.xres;
		im_update.update_region.height = mode_info.yres;
		draw_pattern(fb_fd, fb_mem_ptr, 255, 255, 255);
		im_update.flags = m_opt.update.flags;
		if (m_opt.au != -1)
			return TRUE;	/*for auto update, not needs to send update request */
		if (m_opt.su == 1) {
			memcpy(&im_update, &m_opt.update,
			       sizeof(struct mxcfb_update_data));
		}
		/*do not use alt buffer */
		// im_update.flags = 0;
		/*step 2: update and wait finished */
		if (m_opt.au != -1)
			return TRUE;	/*for auto update, not needs to send update request */
		while (ioctl(fb_fd, MXCFB_SEND_UPDATE, &im_update) < 0) {
			static int iloops = 0;
			if (iloops++ > MAX_WAIT) {
				iloops = 0;
				break;
			}
			sleep(1);
		}
		if(im_update.update_marker){
			struct mxcfb_update_marker_data upd_marker_data;
			upd_marker_data.update_marker = im_update.update_marker;
			while(ioctl(fb_fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE,&upd_marker_data) < 0) {
				wait_time++;
				if (wait_time > MAX_WAIT) {
					tst_resm(TINFO, "wait time exceed!!!\n");
					return FALSE;
				}
			}
		}
		wait_time = 0;
		return TRUE;
	}

/*
 * <p>test wait update</p>
 * <p>procedure:</p> 
 * <p>1. setup update data</p>
 * <p> partila update mode. draw pattern black and white</p>
 * <p> verify no update timeout occure.</p>
 * <p> run in full update mode. </p>
 * @param: hard coded, region update size: BUFFER_WIDTH,BUFFER_HEIGH
 * @param: hard coded, waveform mode:auto
 * @param: hard coded, update mode: partial
 * @param: hard coded, tempture: ambient temperature
 * @param: alt buffer: enable
 * @param: alt buffer size: usergiven or use PXP_BUFFER_SIZE
 * @returns: TRUE success, FALSE failure.
 * @error: ioctl fail or update fail.
 */
	BOOL test_wait_update() {
/*suppose you have set up the device before run this case*/
		/*step 1: set up update data */
		int count = 1;
		int wait_time = 0;
		int update_marker = 0x111;
		struct mxcfb_update_data im_update = {
			{0, 0, 16, 16},	/*region round to 8 */
			257,	/*waveform mode 0-255, 257 auto */
			0,	/*update mode 0(partial),1(Full) */
			update_marker,	/*update_marker assigned by user */
			TEMP_USE_AMBIENT,	/*use ambient temperature set */
			0,	/*do not use alt buffer */
			{
			 0, 0, 0,
			 {
			  0, 0, 0, 0}
			 }
		};
		im_update.flags = m_opt.update.flags;
		if (m_opt.su == 1) {
			memcpy(&im_update, &m_opt.update,
			       sizeof(struct mxcfb_update_data));
		}
		/*do not use alt buffer */
		//im_update.flags = 0;
		/*step 2: update and wait finished */
		while (count--) {
			im_update.update_marker++;
			/*black and white alternative */
			if (count & 0x01)
				draw_pattern(fb_fd, fb_mem_ptr, 255, 255, 255);
			else
				draw_pattern(fb_fd, fb_mem_ptr, 0, 0, 0);
			if (m_opt.au != -1)
				continue;	/*for auto update, not needs to send update request */
			while (ioctl(fb_fd, MXCFB_SEND_UPDATE, &im_update) < 0) {
				static int iloops = 0;
				if (iloops++ > MAX_WAIT) {
					iloops = 0;
					break;
				}
				sleep(1);
			}
			if(1){
				struct mxcfb_update_marker_data upd_marker_data;
				upd_marker_data.update_marker = im_update.update_marker;
				while(ioctl(fb_fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE,
					&upd_marker_data) < 0) {
					wait_time++;
					if (wait_time > MAX_WAIT) {
					tst_resm(TINFO,
						 "wait time exceed!!!\n");
					break;
					}
				}
			}
			wait_time = 0;
			tst_resm(TINFO, "partial mode next update\n");
			draw_pattern(fb_fd, fb_mem_ptr, 0, 0, 0);
		}
		/*step 3: now using full update mode */
		count = 1;
		im_update.update_mode = 1;
		while (count--) {
			im_update.update_marker++;
			/*black and white alternative */
			if (count & 0x01)
				draw_pattern(fb_fd, fb_mem_ptr, 255, 255, 255);
			else
				draw_pattern(fb_fd, fb_mem_ptr, 0, 0, 0);
			if (m_opt.au != -1)
				continue;	/*for auto update, not needs to send update request */
			while (ioctl(fb_fd, MXCFB_SEND_UPDATE, &im_update) < 0) {
				static int iloops = 0;
				if (iloops++ > MAX_WAIT) {
					iloops = 0;
					break;
				}
				sleep(1);
			}
		if(1){
		struct mxcfb_update_marker_data upd_marker_data;
		upd_marker_data.update_marker = im_update.update_marker;
		while(ioctl(fb_fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE,
			&upd_marker_data) < 0) {
				wait_time++;
				if (wait_time > MAX_WAIT) {
					tst_resm(TINFO,
						 "full mode wait time exceed!!!\n");
					break;
				}
			}
			wait_time = 0;
			tst_resm(TINFO, "next update\n");
			draw_pattern(fb_fd, fb_mem_ptr, 0, 0, 0);
		}
	}
		return TRUE;
	}

/*===== epdc_fb_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution. Opens the framebuffer device,
        gets information into the fb_fix_screeninfo structure, and maps fb device into memory.

@param  Input:  None
        Output: None

@return On success - return TPASS
        On failure - return the error code
*/
	int epdc_fb_setup(void) {
		int rv = TFAIL;
		char fb_dev[10] = "/dev/fb";
		int fb_num = 0;
		struct fb_fix_screeninfo screen_info_fix;
		struct fb_var_screeninfo mode_info;
		/* Open the framebuffer device */
		/* Find EPDC FB device */
		while (1) {
			fb_dev[7] = '0' + fb_num;
			fb_fd = open(fb_dev, O_RDWR, 0);
			if (fb_fd < 0) {
				tst_resm(TINFO, "Unable to open %s\n", fb_dev);
				rv = TFAIL;
				return rv;
			}

			/* Check that fb device is EPDC */
			/* First get screen_info */
			rv = ioctl(fb_fd, FBIOGET_FSCREENINFO,
				   &screen_info_fix);
			if (rv < 0) {
				tst_resm(TINFO,
					 "Unable to read fixed screeninfo for %s\n",
					 fb_dev);
				close(fb_fd);
			}
			/* If we found EPDC, exit loop */
			if (!strcmp(EPDC_STR_ID, screen_info_fix.id))
				break;
			fb_num++;
		}
		/*unblank the fb*/
		CALL_IOCTL(ioctl(fb_fd, FBIOBLANK, FB_BLANK_UNBLANK));
/*
    fb_fd = open(m_opt.dev, O_RDWR);
    if (fb_fd < 0)
    {
        tst_brkm(TBROK, cleanup, "Cannot open framebuffer: %s", strerror(errno));
    }
*/
		CALL_IOCTL(ioctl(fb_fd, FBIOGET_VSCREENINFO, &mode_info));
		if (m_opt.grayscale != -1) {
			mode_info.bits_per_pixel = 8;
			/*mode_info.grayscale = GRAYSCALE_8BIT; */
			mode_info.rotate = m_opt.rot;
			mode_info.grayscale = m_opt.grayscale;
			mode_info.yoffset = 0;
			mode_info.activate = FB_ACTIVATE_FORCE;
			CALL_IOCTL(ioctl
				   (fb_fd, FBIOPUT_VSCREENINFO, &mode_info));
			tst_resm(TINFO, "set gray scale mode to %d\n",
				 mode_info.grayscale);
		} else {
			mode_info.bits_per_pixel = 16;
			mode_info.grayscale = 0;
			mode_info.yoffset = 0;
			mode_info.rotate = FB_ROTATE_UR;
			mode_info.activate = FB_ACTIVATE_FORCE;
			CALL_IOCTL(ioctl
				   (fb_fd, FBIOPUT_VSCREENINFO, &mode_info));
			tst_resm(TINFO, "set gray scale mode to %d\n",
				 mode_info.grayscale);
		}
		CALL_IOCTL(ioctl
			   (fb_fd, MXCFB_SET_PWRDOWN_DELAY, &(m_opt.delay)));
#if 1
		/* Get constant fb info */
		if ((ioctl(fb_fd, FBIOGET_FSCREENINFO, &fb_info)) < 0) {
			tst_brkm(TFAIL, cleanup,
				 "Cannot get framebuffer fixed parameters due to ioctl error: %s",
				 strerror(errno));
		}
		/* Map fb device file into memory */
		fb_mem_ptr =
		    mmap(NULL, fb_info.smem_len, PROT_READ | PROT_WRITE,
			 MAP_SHARED, fb_fd, 0);
		if ((int)fb_mem_ptr == -1) {
			tst_brkm(TFAIL, cleanup,
				 "Can't map framebuffer device into memory: %s\n",
				 strerror(errno));
		}
		g_fb_size = fb_info.smem_len;
		sleep(5);
#else
		g_fb_size =
		    mode_info.xres_virtual * mode_info.yres_virtual *
		    mode_info.bits_per_pixel / 8;
		tst_resm(TINFO,
			 "screen_info.xres_virtual = %d\nscreen_info.yres_virtual = %d\nscreen_info.bits_per_pixel = %d\n",
			 mode_info.xres_virtual, mode_info.yres_virtual,
			 mode_info.bits_per_pixel);
		fb_mem_ptr =
		    (unsigned char *)mmap(0, g_fb_size, PROT_READ | PROT_WRITE,
					  MAP_SHARED, fb_fd, 0);
		if ((int)fb_mem_ptr == -1) {
			tst_brkm(TFAIL, cleanup,
				 "Can't map framebuffer device into memory: %s\n",
				 strerror(errno));
		}
		sleep(1);
		tst_resm(TINFO, "mmaped\n");
#endif
		/*keep system on */
		{
			int tfd = open("/dev/tty0", O_RDWR);
			if (write(tfd, "\033[9;0]", 7) < 0)
				perror("write");
			close(tfd);
		}

		CALL_IOCTL(ioctl
			   (fb_fd, MXCFB_SET_WAVEFORM_MODES, &m_opt.waveform));
		if (m_opt.temp != -1)
			CALL_IOCTL(ioctl
				   (fb_fd, MXCFB_SET_TEMPERATURE,
				    &m_opt.waveform));
		if (m_opt.au != -1)
			CALL_IOCTL(ioctl
				   (fb_fd, MXCFB_SET_AUTO_UPDATE_MODE,
				    &m_opt.au));

		CALL_IOCTL(ioctl
			   (fb_fd, MXCFB_SET_UPDATE_SCHEME, &m_opt.scheme));
		rv = TPASS;
		return rv;
	}

/*===== epdc_fb_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution. Closes the framebuffer device.

@param  Input:  None
        Output: None

@return On success - return TPASS
        On failure - return the error code
*/
	int epdc_fb_cleanup(void) {
		draw_pattern(fb_fd, fb_mem_ptr, 0, 0, 0);
		munmap(fb_mem_ptr, g_fb_size);
		close(fb_fd);
		return TPASS;
	}

	int epdc_fb_test() {
		int rv = TPASS;
		switch (m_opt.Tid) {
		case 0:
			tst_resm(TINFO, "normal test");
			if (!full_update()) {
				rv = TFAIL;
				tst_resm(TFAIL, "normal test FAIL");
			} else
				tst_resm(TPASS, "normal test ok");

			break;
		case 1:
			tst_resm(TINFO, "pan test");
			if (!pan_test()) {
				rv = TFAIL;
				tst_resm(TFAIL, "pan test FAIL");
			} else
				tst_resm(TPASS, "pan test ok");
			break;
		case 2:
			tst_resm(TINFO, "draw test test");
			if (!draw_test()) {
				rv = TFAIL;
				tst_resm(TFAIL, "draw test FAIL");
			} else
				tst_resm(TPASS, "draw test ok");
			break;
		case 3:	/*wait update test */
			if (!test_wait_update()) {
				rv = TFAIL;
				tst_resm(TFAIL, "wait update FAIL");
			} else
				tst_resm(TPASS, "wait update ok");
			break;
		case 4:	/*alt buffer overlay test */
			if (!test_alt_update()) {
				rv = TFAIL;
				tst_resm(TFAIL, "alt update FAIL");
			} else
				tst_resm(TPASS, "alt update ok");
			break;
		case 5:	/*collision region update test */
			if (!test_collision_update()) {
				rv = TFAIL;
				tst_resm(TFAIL, "collision update FAIL");
			} else
				tst_resm(TPASS, "collision update ok");
			break;
		case 6:	/*max update region count test */
			if (!test_max_update()) {
				rv = TFAIL;
				tst_resm(TFAIL, "max update FAIL");
			} else
				tst_resm(TPASS, "max update ok");
			break;
		case 7:	/*1000 frames sequence region no collision frame rate test */
			if (!test_rate_update()) {
				rv = TFAIL;
				tst_resm(TFAIL, "max update FAIL");
			} else
				break;
		case 8:
			if (!test_power_delay()) {
				rv = TFAIL;
				tst_resm(TFAIL, "power delay FAIL");
			} else
				tst_resm(TPASS, "power delay OK");
			break;
		case 9:
			if (!gray_scale_test()) {
				rv = TFAIL;
				tst_resm(TFAIL, "gray scale FAIL");
			} else
				tst_resm(TPASS, "gray scale OK");
			break;

		default:
			break;
		}
		return rv;
	}

#ifdef USE_PIC
	int draw_pattern_diasy(unsigned char *pfb, int gwidth, int gheight) {
		int i, j;
		unsigned char *pd = pfb;
		struct pixel px;
		long size;
		px.bpp = 2;
		px.xres = gwidth;
		px.yres = gheight;
		px.r_field.offset = 11;
		px.r_field.length = 5;
		px.g_field.offset = 5;
		px.g_field.length = 6;
		px.b_field.offset = 0;
		px.b_field.length = 5;
		px.trans = 0x00;
		px.line_length = gwidth;
		size = px.line_length * px.yres;

		/* Set color values */
		for (j = 0; j < px.yres; j++) {
			for (i = 0; i < px.xres; i++) {
				unsigned long l = j * gimp_diasy.width + i;
				if (i < gimp_diasy.width
				    && j < gimp_diasy.height) {
					px.r_color =
					    gimp_diasy.pixel_data[l * 3];
					px.g_color =
					    gimp_diasy.pixel_data[l * 3 + 1];
					px.b_color =
					    gimp_diasy.pixel_data[l * 3 + 2];
				} else {
					if (i % 2 == 0) {
						px.r_color = 0;
						px.g_color = 0;
						px.b_color = 0;	/* Set color values */
					} else {
						px.r_color = 255;
						px.g_color = 255;
						px.b_color = 255;	/* Set color values */
					}
				}
				pd = draw_px(pd, &px);
			}
		}
		return 0;
	}

	int draw_pattern_pic(int fd, unsigned char *pfb, int r, int g, int b) {
		struct pixel px;
		int size;
		int i, j;
		unsigned char *fb_wr_ptr = pfb;
		int act_mode;
		int rv = TPASS;
		struct fb_fix_screeninfo fx_fb_info;	/* Framebuffer constant information              */
		struct fb_var_screeninfo mode_info;

		/* Print some fb information */
		if ((ioctl(fd, FBIOGET_VSCREENINFO, &mode_info)) < 0) {
			perror("ioctl");
			rv = TFAIL;
			return rv;
		}

		/* Change activation flag and apply it */
		act_mode = mode_info.activate;
		mode_info.activate = FB_ACTIVATE_NOW | FB_ACTIVATE_FORCE;
		if (ioctl(fd, FBIOPUT_VSCREENINFO, &mode_info)) {
			perror("ioctl");
			rv = TFAIL;
			return rv;
		}

		CALL_IOCTL(ioctl(fd, FBIOGET_FSCREENINFO, &fx_fb_info));
		/* Fill in the px struct */
		px.bpp = mode_info.bits_per_pixel / 8;
		px.xres =
		    mode_info.xres % 32 ==
		    0 ? mode_info.xres : mode_info.xres_virtual;
		px.yres = mode_info.yres;
		px.r_field.offset = mode_info.red.offset;
		px.r_field.length = mode_info.red.length;
		px.g_field.offset = mode_info.green.offset;
		px.g_field.length = mode_info.green.length;
		px.b_field.offset = mode_info.blue.offset;
		px.b_field.length = mode_info.blue.length;
		px.trans = 0x00;
		px.line_length = fx_fb_info.line_length / px.bpp;
		size = px.line_length * px.yres;

		for (j = 0; j < px.yres; j++) {
			for (i = 0; i < px.xres; i++) {
				unsigned long l = j * gimp_image.width + i;
				if (i < gimp_image.width
				    && j < gimp_image.height) {
					px.r_color =
					    gimp_image.pixel_data[l * 3];
					px.g_color =
					    gimp_image.pixel_data[l * 3 + 1];
					px.b_color =
					    gimp_image.pixel_data[l * 3 + 2];
					fb_wr_ptr = draw_px(fb_wr_ptr, &px);
				} else {
					if (i % 2 == 0) {
						px.r_color = 0;
						px.g_color = 0;
						px.b_color = 0;	/* Set color values */
					} else {
						px.r_color = 255;
						px.g_color = 255;
						px.b_color = 255;	/* Set color values */
					}
					fb_wr_ptr = draw_px(fb_wr_ptr, &px);
				}
			}
		}
		/* Restore activation flag */
		if (mode_info.activate != act_mode) {
			mode_info.activate = act_mode;
			ioctl(fd, FBIOPUT_VSCREENINFO, &mode_info);
		}
		return rv;

	}
#endif
	int draw_pattern(int fd, unsigned char *pfb, int r, int g, int b) {
		struct pixel px;
		int size;
		int i;
		unsigned char *fb_wr_ptr = pfb;
		int act_mode;
		int rv = TPASS;
		struct fb_fix_screeninfo fx_fb_info;	/* Framebuffer constant information              */
		struct fb_var_screeninfo mode_info;

		/* Print some fb information */
		if ((ioctl(fd, FBIOGET_VSCREENINFO, &mode_info)) < 0) {
			perror("ioctl");
			rv = TFAIL;
			return rv;
		}

		/* Change activation flag and apply it */
		act_mode = mode_info.activate;
		mode_info.activate = FB_ACTIVATE_NOW | FB_ACTIVATE_FORCE;
		if (ioctl(fd, FBIOPUT_VSCREENINFO, &mode_info)) {
			perror("ioctl");
			rv = TFAIL;
			return rv;
		}

		CALL_IOCTL(ioctl(fd, FBIOGET_FSCREENINFO, &fx_fb_info));
		/* Fill in the px struct */
		px.bpp = mode_info.bits_per_pixel / 8;
		px.xres =
		    mode_info.xres % 32 ==
		    0 ? mode_info.xres : mode_info.xres_virtual;
		px.yres = mode_info.yres;
		px.r_field.offset = mode_info.red.offset;
		px.r_field.length = mode_info.red.length;
		px.g_field.offset = mode_info.green.offset;
		px.g_field.length = mode_info.green.length;
		px.b_field.offset = mode_info.blue.offset;
		px.b_field.length = mode_info.blue.length;
		px.trans = 0x00;
		px.line_length = fx_fb_info.line_length / px.bpp;
		size = px.line_length * px.yres;

		/* Clear screen and fill it with some pattern */
		px.r_color = r;
		px.g_color = g;
		px.b_color = b;	/* Set color values */
		for (i = 0; i < size; i++) {
			fb_wr_ptr = draw_px(fb_wr_ptr, &px);
		}
		/* Restore activation flag */
#if 1
		mode_info.activate = act_mode;
		ioctl(fd, FBIOPUT_VSCREENINFO, &mode_info);
#endif
		return rv;
	}
/*===== draw_px =====*/
/**
@brief  Computes byte values from given color values depending on color depth and draws one pixel

@param  Input:  where - pointer to the pixel that will be drawn
                p     - pointer to struct pixel that contains color values and screen color info
        Output: None

@return pointer to the next pixel that will be drawn
*/
	unsigned char *draw_px(unsigned char *where, struct pixel *p) {
		__u32 value;
		if (!where) {
			tst_resm(TFAIL,
				 "where isn't a valid pointer to 'unsigned char' ");
			return where;
		}
		if (!p) {
			tst_resm(TFAIL,
				 "p isn't a valid pointer to 'struct pixel' ");
			return where;
		}
		/* Convert pixel color represented by 3 bytes to appropriate color depth */
		if (p->r_field.offset == 0 && p->g_field.offset == 0
		    && p->b_field.offset == 0) {
			/*gray scale image */
			unsigned char r = p->r_color;
			unsigned char g = p->g_color;
			unsigned char b = p->b_color;
			value = (unsigned char)
			    BOUND255(((PR * r + PG * g + PB * b) >> 8) + 16);
		} else {
			value =
			    (p->r_color * (1 << p->r_field.length) /
			     (1 << 8)) << p->r_field.offset;
			value |=
			    (p->g_color * (1 << p->g_field.length) /
			     (1 << 8)) << p->g_field.offset;
			value |=
			    (p->b_color * (1 << p->b_field.length) /
			     (1 << 8)) << p->b_field.offset;
		}
		switch (p->bpp * 8) {
		case 8:
			/*fix me */
			*where++ = value;
			break;
		case 12 ... 16:
			*where++ = *((unsigned char *)&value);
			*where++ = *((unsigned char *)&value + 1);
			break;
		case 24:
			*where++ = *((unsigned char *)&value);
			*where++ = *((unsigned char *)&value + 1);
			*where++ = *((unsigned char *)&value + 2);
			break;
		case 32:
			*where++ = *((unsigned char *)&value);
			*where++ = *((unsigned char *)&value + 1);
			*where++ = *((unsigned char *)&value + 2);
			*where++ = *((unsigned char *)&value + 3);
			break;
		default:
			break;
		}
		return where;
	}

#ifdef __cplusplus
}
#endif
