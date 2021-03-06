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
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <linux/fb.h>
#include <linux/mxcfb.h>
#include <errno.h>      /* errno*/
#include "pxp_lib.h"

#define USE_PIC

#ifdef USE_PIC
#include "fsl_logo_480x360.h"
#endif

#define CALL_IOCTL(ioctl_cmd)\
{ \
    if( (ioctl_cmd) < 0 )\
    {\
        printf("%s : %s fails #%d [File: %s, line: %d]\n", __FUNCTION__, "ioctl_", errno, __FILE__, __LINE__-2);\
        perror("ioctl"); \
        return 1;\
    }\
}

#define EPDC_STR_ID		"mxc_epdc_fb"

struct mrect{
int l;
int t;
int w;
int h;
};

struct S_OPT{
int rot;/*rotation*/
struct mrect rsize; /*resize*/
int l;/*invert*/
int m;/*multi-instance*/
int alpha;/*alpha value*/
int c;/*frame count*/
long k;/*color key*/
int scale;/*scale*/
int hflip;/*horizontal flip*/
int vflip;/*vertical flip*/
int bk; /*back ground color*/
struct mrect altresize;/*alternaticely size*/
long vk;/*o0verlay colr key*/
int ga;/*global alpha*/
int la;/*local alpha*/
};

int n_inst = 0;
int fd_fb = 0;
unsigned short * fb_map;
char fb_device[] = "/dev/fb";
int quitflag = 0;
unsigned int marker_val = 1;
sigset_t sigset;

static void copy_image_to_fb(int left, int top, int width, int height, uint *img_ptr, struct fb_var_screeninfo *screen_info)
{
	int i;
	uint bytes_per_pixel;

	if ((width > screen_info->xres) || (height > screen_info->yres)) {
		printf("Bad image dimensions!\n");
		return;
	}

	bytes_per_pixel = screen_info->bits_per_pixel / 8;

	for (i = 0; i < height; i++) {
		memcpy(fb_map + ((i + top) * screen_info->xres + left) * bytes_per_pixel / 4,
			img_ptr + (i * width) * bytes_per_pixel /4,
			width * bytes_per_pixel);
	}
}


static int update_to_display(int left, int top, int width, int height, int wave_mode, int wait_for_complete)
{
	struct mxcfb_update_data upd_data;

	upd_data.update_mode = UPDATE_MODE_FULL;
	upd_data.waveform_mode = wave_mode;
	upd_data.update_region.left = left;
	upd_data.update_region.width = width;
	upd_data.update_region.top = top;
	upd_data.update_region.height = height;
	upd_data.temp = TEMP_USE_AMBIENT;
	upd_data.flags = 0;
	if (wait_for_complete) {
		/* Get unique marker value */
		upd_data.update_marker = marker_val++;
	} else {
		upd_data.update_marker = 0;
	}
	while( ioctl(fd_fb, MXCFB_SEND_UPDATE, &upd_data) < 0){
		static int cnt = 0;
		sleep(1);
		if(cnt++ == 10)
		{
			printf("retry 10 s abort\n");
			cnt = 0;
			break;
		}
	}

//	if (wave_mode == WAVEFORM_MODE_AUTO)
//		dbg(DBG_INFO, "Waveform mode used = %d\n", upd_data.waveform_mode);

	if (wait_for_complete) {
		/* Wait for update to complete */
	#define MAX_WAIT 5
	int wait_time = 0;
	struct mxcfb_update_marker_data upd_marker_data;
	upd_marker_data.update_marker = upd_data.update_marker;
	while(ioctl(fd_fb, MXCFB_WAIT_FOR_UPDATE_COMPLETE,&upd_marker_data) < 0) {
			wait_time++;
			if (wait_time > MAX_WAIT) {
				printf("TINFO: full mode wait time exceed!!!\n");
				return 1;
			}
		}
	}
	return 0;
}

int run_test(void * p_opts)
{
	int ret = 0,i,j;
	int width = 640;
	int height = 480;
	struct pxp_config_data *pxp_conf = NULL;
	struct pxp_proc_data *proc_data = NULL;
	struct pxp_mem_desc mem, mem_o, mem_ol;
	pxp_chan_handle_t pxp_chan;
	struct fb_var_screeninfo var;
	struct fb_fix_screeninfo fix;
	struct S_OPT im_opts;

  if(p_opts == NULL)
		return -1;

	
	memcpy(&im_opts,p_opts,sizeof(struct S_OPT));

	ioctl(fd_fb, FBIOGET_VSCREENINFO, &var);
	width = im_opts.rsize.w == 0? width:im_opts.rsize.w;
	height = im_opts.rsize.h == 0? height:im_opts.rsize.h;
	ret = pxp_init();
	if (ret < 0) {
		printf("pxp init err\n");
		return -1;
	}
	ret = pxp_request_channel(&pxp_chan);
	if (ret < 0) {
		printf("pxp request channel err\n");
		goto err0;
	}
	printf("requested chan_id %d\n", pxp_chan.chan_id);
	/* Prepare the channel parameters */
	mem.size = width * height * 2; /*RGB565 takes 2 bytes*/
	ret = pxp_get_mem(&mem);
	if (ret < 0) {
		printf("get mem err\n");
		goto err1;
	}
	printf("mem.virt_uaddr %08x, mem.phys_addr %08x, mem.size %d\n",
				mem.virt_uaddr, mem.phys_addr, mem.size);
	mem_o.size = width * height;/*to Y*/
	ret = pxp_get_mem(&mem_o);
	if (ret < 0) {
		printf("get mem_o err\n");
		goto err1;
	}
	printf("mem_o.virt_uaddr %08x, mem_o.phys_addr %08x, mem_o.size %d\n",
				mem_o.virt_uaddr, mem_o.phys_addr, mem_o.size);

	mem_ol.size = width * height * 2;/*to Y*/
	ret = pxp_get_mem(&mem_ol);
	if (ret < 0) {
		printf("get mem_ol err\n");
		goto err1;
	}
	printf("mem_ol.virt_uaddr %08x, mem_ol.phys_addr %08x, mem_ol.size %d\n",
				mem_ol.virt_uaddr, mem_ol.phys_addr, mem_ol.size);


	for (i = 0; i < width * height * 2 ; i++)
		*(((unsigned char *)(mem.virt_uaddr) + i)) = 255;
#ifdef USE_PIC
	printf("draw pic in %d\n", sizeof(fb_480x360_2));
	for (j = 0; j < height; j++)
	{
		if( j >= 360)
			break;
		for(i = 0; i < width; i += 2)
		{
			long ps = (j * width + i)/2;
			long ls = (j * 480 + i)/2;
			if(i >= 480)
				break;
			if(quitflag == 1)
				break;
			((unsigned int*)(mem.virt_uaddr))[ps] = fb_480x360_2[ls];
			((unsigned int*)(mem_ol.virt_uaddr))[ps] = fb_480x360_2[ls];
		}
	}

#endif
	/* Configure the channel */
	pxp_conf = malloc(sizeof (*pxp_conf));
	proc_data = &pxp_conf->proc_data;

	/* Initialize non-channel-specific PxP parameters */
	proc_data->srect.left = im_opts.rsize.l;
	proc_data->srect.top = im_opts.rsize.t;
	proc_data->drect.left = im_opts.rsize.l;
	proc_data->drect.top = im_opts.rsize.t;
	proc_data->srect.width = width;
	proc_data->srect.height = height;
	proc_data->drect.width =  width;
	proc_data->drect.height = height;
	proc_data->scaling = im_opts.scale;
	proc_data->hflip = im_opts.hflip;
	proc_data->vflip = im_opts.vflip;
	proc_data->rotate = im_opts.rot;
	proc_data->bgcolor = im_opts.bk;
	proc_data->overlay_state = im_opts.altresize.w == 0?0: 1;
	proc_data->lut_transform = im_opts.l == 0 ? PXP_LUT_NONE:PXP_LUT_INVERT;
	
	/*
	 * Initialize S0 parameters
	 */
	pxp_conf->s0_param.pixel_fmt = PXP_PIX_FMT_RGB565;
	pxp_conf->s0_param.width = width;
	pxp_conf->s0_param.height = height;
	pxp_conf->s0_param.color_key = im_opts.k;
	pxp_conf->s0_param.color_key_enable = im_opts.k == 0? false:true;
	pxp_conf->s0_param.paddr = mem.phys_addr;

	printf("pxp_test s0 paddr %08x\n", pxp_conf->s0_param.paddr);
	/*
	 * Initialize OL parameters
	 */
	 for (i=0; i < 8; i++) {
		pxp_conf->ol_param[i].combine_enable = im_opts.altresize.w ==0?false:true;
		pxp_conf->ol_param[i].width = im_opts.altresize.w;
		pxp_conf->ol_param[i].height = im_opts.altresize.h;
		pxp_conf->ol_param[i].pixel_fmt = PXP_PIX_FMT_RGB565;
		pxp_conf->ol_param[i].color_key_enable = im_opts.vk == 0? false:true;
		pxp_conf->ol_param[i].color_key = im_opts.vk;
		pxp_conf->ol_param[i].global_alpha_enable = im_opts.alpha;
		pxp_conf->ol_param[i].global_alpha = im_opts.ga;
		pxp_conf->ol_param[i].local_alpha_enable = im_opts.la;
		pxp_conf->ol_param[i].paddr  = mem_ol.phys_addr;
	}

	/*
	 * Initialize Output channel parameters
	 * Output is Y-only greyscale
	 */
	pxp_conf->out_param.width = width;
	pxp_conf->out_param.height = height;
	pxp_conf->out_param.pixel_fmt = PXP_PIX_FMT_GREY;

	if (ioctl(fd_fb, FBIOGET_FSCREENINFO, &fix) < 0) {
		printf("FBIOGET_FSCREENINFO error!\n");
		close(fd_fb);
		goto err2;
	}

	if (im_opts.rot % 180)
		pxp_conf->out_param.stride = height > var.yres? var.yres:height;
	else
		pxp_conf->out_param.stride = width > var.yres?var.yres:width;

	pxp_conf->out_param.paddr = mem_o.phys_addr;
	printf("out addr (smem_start): 0x%08x\n", pxp_conf->out_param.paddr);

	ret = pxp_config_channel(&pxp_chan, pxp_conf);
	if (ret < 0) {
		printf("pxp config channel err\n");
		goto err2;
	}

	ret = pxp_start_channel(&pxp_chan);
	if (ret < 0) {
		printf("pxp start channel err\n");
		goto err2;
	}

	ret = pxp_wait_for_completion(&pxp_chan, 3);
	if (ret < 0) {
		printf("pxp wait for completion err\n");
		goto err2;
	}
#if 0
	{
	int g_fb0_size = 0;
	var.bits_per_pixel = 8;
	var.xres = 800;
	var.yres = 600;
	var.grayscale = GRAYSCALE_8BIT;
	var.yoffset = 0;
	var.rotate = FB_ROTATE_UR;
	var.activate = FB_ACTIVATE_FORCE;
	ret = ioctl(fd_fb, FBIOPUT_VSCREENINFO, &var);
  g_fb0_size = var.xres * var.yres * var.bits_per_pixel / 8;
	/* Map the device to memory, */
	fb_map = (unsigned short *)mmap(0, g_fb0_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_fb, 0);
	if ((int)fb_map <= 0)
	{
		printf("\nError: failed to map framebuffer device 0 to memory.\n");
		goto err2;
	}
	}
#endif

	if (im_opts.rot % 180) {
		int tmp = width;
		width = height;
		height = tmp;
	}

	while(im_opts.c--)
	{
	copy_image_to_fb(proc_data->srect.left, proc_data->srect.top,
			 width, height, (void *)mem_o.virt_uaddr, &var);
	if(quitflag == 1)
		break;
	printf("Update to display.\n");
	printf("w/h %d/%d\n", width ,height);
	ret = update_to_display(proc_data->srect.left, proc_data->srect.top,
			  width, height, WAVEFORM_MODE_AUTO, true);
	}
err2:
	pxp_put_mem(&mem);
	pxp_put_mem(&mem_o);
	pxp_put_mem(&mem_ol);
err1:
	free(pxp_conf);
	pxp_release_channel(&pxp_chan);
err0:
	pxp_uninit();
	return ret;
}


static int signal_thread(void *arg)
{
	int sig, err;

	pthread_sigmask(SIG_BLOCK, &sigset, NULL);

	while (1) {
		err = sigwait(&sigset, &sig);
		if (sig == SIGINT) {
			printf("Ctrl-C received\n");
		} else {
			printf("Unknown signal. Still exiting\n");
		}
		quitflag = 1;
		break;
	}
	return 0;
}


int main(int argc, char ** argv)
{
  	int oc,ret = 0;
	int fb_num = 0;
	int g_fb0_size = 0;
	pthread_t sigtid;
	struct S_OPT m_opts;
	struct fb_var_screeninfo var;
	struct fb_fix_screeninfo fix;
	char fb_device[10] = "/dev/fb0";

	sigemptyset(&sigset);
	sigaddset(&sigset, SIGINT);
	pthread_sigmask(SIG_BLOCK, &sigset, NULL);
	pthread_create(&sigtid, NULL, (void *)&signal_thread, NULL);

	while (1) {
		fb_device[7] = '0' + fb_num;
		fd_fb = open(fb_device, O_RDWR, 0);
		if (fd_fb < 0) {
			printf("TINFO,Unable to open %s\n", fb_device);
			return -1;
		}
		/* Check that fb device is EPDC */
		/* First get screen_info */
		ret = ioctl(fd_fb, FBIOGET_FSCREENINFO,
			   &fix);
		if (ret < 0) {
			printf("TINFO,Unable to read fixed screeninfo for %s\n",
				 fb_device);
			close(fd_fb);
		}
		/* If we found EPDC, exit loop */
		if (!strcmp(EPDC_STR_ID, fix.id))
			break;
		fb_num++;
	}


	memset(&m_opts,0,sizeof(struct S_OPT));
	/*default setting goes here*/
	m_opts.m = 1;
	m_opts.rsize.l = 0;
	m_opts.rsize.t = 0;
	m_opts.rsize.w = 512;
	m_opts.rsize.h = 512;
	m_opts.c = 1;

  while((oc = getopt(argc, argv, "VHr:s:l:m:azc:k:o:")) != -1)
  {
		switch(oc)
		{
			case 'V':
				m_opts.vflip = 1;
				break;
			case 'H':
				m_opts.hflip = 1;
				break;
			case 'S':
				m_opts.scale = atoi(optarg);/*fix me*/
				break;
			case 'b':
				m_opts.bk = atoi(optarg);
				break;
			case 'r':
				m_opts.rot = atoi(optarg);
				break;
			case 's':
				sscanf(optarg,"%d:%d:%d:%d",&(m_opts.rsize.l),
				&(m_opts.rsize.t),&(m_opts.rsize.w),&(m_opts.rsize.h));
				break;
			case 'l':
				m_opts.l = atoi(optarg);
				break;
			case 'm':
				m_opts.m = atoi(optarg);
				break;
			case 'a':
				m_opts.alpha = 1;
				break;
			case 'c':
				m_opts.c = atoi(optarg);
				break;
			case 'k':
				sscanf(optarg, "%x",&(m_opts.k));
				break;
			case 'o':
				sscanf(optarg,"%d:%d:%d:%d:%x:%d:%d",&(m_opts.altresize.l),
				&(m_opts.altresize.t),
				&(m_opts.altresize.w),
				&(m_opts.altresize.h),
				&(m_opts.vk),
				&(m_opts.ga),
				&(m_opts.la));
				break;
			default:
				exit(1);
				break;
		}
  }
  printf("setings rotation = %d\n",m_opts.rot);
  printf("setings size = %d:%d:%d:%d\n",
	m_opts.rsize.l,
	m_opts.rsize.t,
	m_opts.rsize.w,
	m_opts.rsize.h);
  printf("setings invert lut = %s\n",
	m_opts.l == 1?"on":"off");
  printf("instance = %d\n",m_opts.m);
  printf("alpha = %d\n",m_opts.alpha);
  printf("frame count = %d\n",m_opts.c);
  printf("color key = 0x%x\n",m_opts.k);
  printf("alternative size = %d:%d:%d:%d\n",
	m_opts.altresize.l,
	m_opts.altresize.t,
	m_opts.altresize.w,
	m_opts.altresize.h);
	printf("vertical flip %s\n", m_opts.vflip == 1? "on":"off");
	printf("horlizontal flip %s\n",m_opts.hflip == 1? "on":"off");
	printf("scale %d\n",m_opts.scale);
	printf("back ground color %d\n", m_opts.bk);
	printf("overlay color key %d\n", m_opts.vk);
	printf("global alpha %d\n", m_opts.ga);
	printf("local alpha %d\n", m_opts.la);
	printf("run %d instance\n", m_opts.m);

/*default fb setting*/
#if 1
	CALL_IOCTL(ioctl(fd_fb, FBIOGET_VSCREENINFO, &var));
	var.bits_per_pixel = 8;
	var.xres = m_opts.rsize.w * 2;
	var.yres = m_opts.rsize.h * 2;
	var.grayscale = GRAYSCALE_8BIT;
	var.yoffset = 0;
	var.rotate = FB_ROTATE_UR;/*Limitation only set framebuffer rotate to this value*/
	var.activate = FB_ACTIVATE_FORCE;
	CALL_IOCTL(ioctl(fd_fb, FBIOPUT_VSCREENINFO, &var));
	g_fb0_size = var.xres * var.yres * var.bits_per_pixel / 8;
	/* Map the device to memory, */
	fb_map = (unsigned short *)mmap(0, g_fb0_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_fb, 0);
	if ((int)fb_map <= 0)
	{
		printf("\nError: failed to map framebuffer device 0 to memory.\n");
		close(fd_fb);
		return 1;
	}
#endif
  if(m_opts.m)
	{
		int i = 0;
		pthread_t * pids = NULL;
		pids = (pthread_t *)malloc(m_opts.m*sizeof(pthread_t));
		for(i= 0; i < m_opts.m ; i++)
		{
			pids[i] = 0;
			pthread_create(&(pids[i]), NULL,
			(void *)&run_test, (void *)&(m_opts));
		}
		for(i = 0; i < m_opts.m; i++)
		{
			int tret = 0;
			if(pids[i] != 0)
				pthread_join(pids[i], (void **)&tret);
			ret += tret;
		}
		if(pids != NULL)
			free(pids);
	}
	/*pthread_join(sigtid, NULL);*/
	close(fd_fb);
	return ret;
}
