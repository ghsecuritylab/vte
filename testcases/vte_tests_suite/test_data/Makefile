
DATA-y = butterfly.png 
DATA-$(CONFIG_VTE_SYS)           += output_video_yuv420 
DATA-$(CONFIG_VTE_FS)            += sample.cramfs
SUBDIRS-$(CONFIG_VTE_V4L_PXP)    += pxp_test_files
SUBDIRS-$(CONFIG_VTE_VPU)        += pc_tool
SUBDIRS-$(CONFIG_VTE_V4L_IPU_PF) += ipu_pf
SUBDIRS-$(CONFIG_VTE_V4L_OUTPUT) += v4l_output_config
SUBDIRS-$(CONFIG_VTE_MBX)        += mbx_3rd_party
SUBDIRS-$(CONFIG_VTE_PTP)        += ptp_test

all:
	
install:
	-ln -f $(DATA-y) ../../bin/
	for i in $(SUBDIRS-y); do $(MAKE) -C $$i install; done 
clean:

