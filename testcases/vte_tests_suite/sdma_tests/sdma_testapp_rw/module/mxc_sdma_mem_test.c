/***
 ** Copyright 2008-2009 Freescale Semiconductor, Inc. All Rights Reserved.
 **
 ** The code contained herein is licensed under the GNU General Public
 ** License. You may obtain a copy of the GNU General Public License
 ** Version 2 or later at the following locations:
 **
 ** http://www.opensource.org/licenses/gpl-license.html
 ** http://www.gnu.org/copyleft/gpl.html
 **/

/* Update History
 * 3/4/09, 1. Fix driver upgrade from 2.6.26 -> 28
 * Spring, 2. Comment hardware.h for no such file available
 * 16/11/2009, Spring Zhang
 * change include header file from asm/dma.h to mach/dma.h
 */

/* THIS_MODULE, */
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/mman.h>
#include <linux/init.h>
/* include asm/dma-mapping.h, dma_alloc_coherent(), dma_free_coherent()  
 * include linux/err.h, IS_ERR, PTR_ERR, 
 * include linux/device.h, 
 * */
#include <linux/dma-mapping.h>

/* register_chrdev(), */
#include <linux/fs.h>    
#include <linux/version.h> /* KENREL_VERSION macro */

/* #include <asm/dma.h>, from 2.6.31, need to include mach/dma.h, <=2.6.26,
 * include asm/hardware.h. 
 * include asm/mach/dma.h, mxc_dma_request(), mxc_dma_free(), 
 * mxc_dma_config(), mxc_dma_enable() 
 * MXC_DMA_MEMORY, MXC_DMA_MODE_READ, 
 * */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,31))
#include <mach/dma.h>
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,28))
#include <asm/dma.h>
#endif

/* udelay(), */
/* #include <asm/delay.h> */

/* class_device, class_device_destroy(), class_device_create() */
#include <linux/device.h>  

/* __raw_readl, __raw_writel, */
#include <linux/io.h>
/* mdelay(), include asm/delay.h */
#include <linux/delay.h>

/* include asm/arch/hardware.h */
/* Comment below line for nk28 no file include/asm/hardware.h
#include <asm/hardware.h>*/
/* IO_ADDRESS,  */
/* #include <asm/arch/hardware.h> */

/* GFP_KERNEL, GFP_DMA */
/* #include <linux/gfp.h> */

/*
 * For backward kernel compatible
 */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,28))

#ifndef class_device_destroy
#define class_device_destroy  device_destroy
#endif
#ifndef class_device_create
#define class_device_create(cs, NULL, dev, parent, fmt,args...)  device_create(cs, parent, dev, NULL, fmt, ##args)
#endif
#ifndef class_device
#define class_device device
#endif

#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26))

#ifndef class_device_destroy
#define class_device_destroy  device_destroy
#endif
#ifndef class_device_create
#define class_device_create(cs, NULL, dev, parent, fmt,args...)  device_create(cs, parent, dev, fmt, ##args)
#endif
#ifndef class_device
#define class_device device
#endif

#endif

static int gMajor; /* major number of device */
static struct class *dma_tm_class;
static char *wbuf;
static char *rbuf;
static dma_addr_t wpaddr;
static dma_addr_t rpaddr;
static int sdma_channel = 0;
static int g_sand = 100;

#define SDMA_BUF_SIZE  1024
#define SDMA_SAND 13

void sdma_callback(void *arg, int error, unsigned int count);

static void mxc_printSDMAcontext(int channel)
{
	int i = 0, offset = 0;
	unsigned long tempAddr;
	volatile unsigned int result = 0, reg = 0;

	tempAddr = (unsigned long)(IO_ADDRESS(SDMA_BASE_ADDR));

	offset = 0x800 + (32 * channel);

	/* enable host ONCE */
	reg = __raw_readl(tempAddr + 0x40);
	reg |= 0x1;
	__raw_writel(reg, tempAddr + 0x40);

	/* enter debug mode */
	__raw_writel(0x05, tempAddr + 0x50);

	printk("Context of Channel %d\n", channel);
	for(i = 0; i < 32; i++) {
		mdelay(3);

		__raw_writel(((0x08 << 8) | ((offset >> 8) & 0xFF)) & 0xFFFF, tempAddr + 0x48);
		__raw_writel(0x02, tempAddr + 0x50);

		mdelay(3);
		__raw_writel(0x0011, tempAddr + 0x48);
		__raw_writel(0x02, tempAddr + 0x50);
		mdelay(3);

		/* 0x18xx xx = channel * 20 */
		reg = (((0x18 << 8) | (offset & 0xFF)) + i) & 0xFFFF;
		__raw_writel(reg, tempAddr + 0x48);
		__raw_writel(0x02, tempAddr + 0x50);
		mdelay(3);

		__raw_writel(0x5100, tempAddr + 0x48);
		__raw_writel(0x02, tempAddr + 0x50);
		mdelay(3);

		__raw_writel(0x01, tempAddr + 0x50);
		mdelay(3);

		result = __raw_readl(tempAddr + 0x44);
		/*
		 * Each data read into this register at address 0x53fd4044
		 * at this step is a register of the channel context
		 */
		printk("Context[%d] : %x\n", i, result);
	}
	/* exit debug mode */
	__raw_writel(0x03, tempAddr + 0x50);
}

static void sand(int rand)
{
	g_sand = rand;
	return;
}

static char rand(void)
{
	g_sand = g_sand * 12357 / 1103;
	return g_sand;
}

static void FillMem(char *p, int size, int rd)
{
	int i = 0;

	sand(rd);
	for(i = 0; i < size; i++) {
		*p++ = rand();
	}
        return;
}

static int Verify(char *p, int size, int rd)
{
	int i = 0,j = 0;
	char x;

	sand(rd);
	for(i = 0; i < size; i++) {
		x = rand();
		if (x != p[i]) {
			j++;
			break;
		}
	}

	if (j > 0) {
		return 1;
	} else {
		return 0;
	}
}

int sdma_open(struct inode * inode, struct file * filp)
{
	sdma_channel = mxc_dma_request(MXC_DMA_MEMORY, "SDMA Memory to Memory");
	if (sdma_channel < 0) {
		printk("Error opening the SDMA memory to memory channel\n");
		return -ENODEV;
	}

	wbuf = dma_alloc_coherent(NULL, SDMA_BUF_SIZE, &wpaddr, GFP_DMA);
	rbuf = dma_alloc_coherent(NULL, SDMA_BUF_SIZE, &rpaddr, GFP_DMA);

    /* add callback, but here will cause darg != darg, confused 
    mxc_dma_callback_set(sdma_channel, sdma_callback, (void*)&darg);
    */

	return 0;
}

int sdma_release(struct inode * inode, struct file * filp)
{
	mxc_dma_free(sdma_channel);
	dma_free_coherent(NULL, SDMA_BUF_SIZE, wbuf, wpaddr);
	dma_free_coherent(NULL, SDMA_BUF_SIZE, rbuf, rpaddr);

	return 0;
}

ssize_t sdma_read (struct file *filp, char __user * buf, size_t count, loff_t * offset)
{
	if (Verify(rbuf, SDMA_BUF_SIZE, 100) == 0) {
		printk("SDMA memory test PASSED\n");
	} else {
		printk("SDMA memory test FAILED\n");
		mxc_printSDMAcontext(sdma_channel);
        return -1;
	}

	return 0;
}

ssize_t sdma_write(struct file * filp, const char __user * buf, size_t count, loff_t * offset)
{
    int err = 0;

	mxc_dma_requestbuf_t *sdmachnl_reqelem;

	if ((sdmachnl_reqelem = kmalloc(sizeof(mxc_dma_requestbuf_t), GFP_KERNEL)) == NULL) {
		return -ENOMEM;
	}

	FillMem(wbuf, SDMA_BUF_SIZE, 100);

	sdmachnl_reqelem->dst_addr = rpaddr;
	sdmachnl_reqelem->src_addr = wpaddr;
	sdmachnl_reqelem->num_of_bytes = SDMA_BUF_SIZE;

	mxc_dma_config(sdma_channel, sdmachnl_reqelem, 1,
		       MXC_DMA_MODE_READ);

    /* Set callback function */
    mxc_dma_callback_set(sdma_channel, sdma_callback, (void*)&sdma_channel);

    err = mxc_dma_enable(sdma_channel);
	if (err){
        printk("Data write error!\n");
        kfree(sdmachnl_reqelem);
        return err;
    }

	kfree(sdmachnl_reqelem);

	printk("Data write completed\n");
	return 0;
}

void sdma_callback(void *arg, int error, unsigned int count)
{
    /* arg is sdma_channel */
    int *i=(int *)arg;

    printk("Callback function: ");
    printk("SDMA channel=%d, error=%d, SDMA transferred count=%u\n", *i, error, count);

    return;
}

struct file_operations dma_fops = {
	open:		sdma_open,
	release:	sdma_release,
	read:		sdma_read,
	write:		sdma_write,
};

int __init sdma_init_module(void)
{
	struct class_device *temp_class;
	int error;

	/* register a character device */
	error = register_chrdev(0, "sdma_test", &dma_fops);
	if (error < 0) {
		printk("SDMA test driver can't get major number\n");
		return error;
	}
	gMajor = error;
	printk("SDMA test major number = %d\n",gMajor);

	dma_tm_class = class_create(THIS_MODULE, "sdma_test");
	if (IS_ERR(dma_tm_class)) {
		printk(KERN_ERR "Error creating sdma test module class.\n");
		unregister_chrdev(gMajor, "sdma_test");
		return PTR_ERR(dma_tm_class);
	}

	temp_class = class_device_create(dma_tm_class, NULL,
					     MKDEV(gMajor, 0), NULL,
					     "sdma_test");
	if (IS_ERR(temp_class)) {
		printk(KERN_ERR "Error creating sdma test class device.\n");
		class_destroy(dma_tm_class);
		unregister_chrdev(gMajor, "sdma_test");
		return -1;
	}

	printk("SDMA test Driver Module loaded\n");
	return 0;
}

static void sdma_cleanup_module(void)
{
	unregister_chrdev(gMajor, "sdma_test");
	class_device_destroy(dma_tm_class, MKDEV(gMajor, 0));
	class_destroy(dma_tm_class);

	printk("SDMA test Driver Module Unloaded\n");
}


module_init(sdma_init_module);
module_exit(sdma_cleanup_module);

MODULE_AUTHOR("Freescale Semiconductor");
MODULE_DESCRIPTION("SDMA test driver");
MODULE_LICENSE("GPL");
