
AUTO_TARGETS-$(CONFIG_VTE_IMX6) += imx6q_AI_auto imx6q_lite imx6_stress imx6q_camera imx6q_arm2 imx6q_SABRE_SD imx6q_SMD_auto
AUTO_TARGETS-$(CONFIG_VTE_IMX6) += imx61_AI_auto imx61_AI_long imx61_AI_stress imx61_SMD_auto
AUTO_TARGETS-$(CONFIG_VTE_IMX6) += imx60_ARM2_auto imx60_EVK_auto 
AUTO_TARGETS-$(CONFIG_VTE_IMX6) += imx6dl_smd_auto_nosususpend 
AUTO_TARGETS-$(CONFIG_VTE_IMX6) +=  imx6q_AI_long imx6q_AI_stress
AUTO_TARGETS-$(CONFIG_VTE_IMX5) += imx51_auto imx51_BABBAGE_auto
AUTO_TARGETS-$(CONFIG_VTE_IMX5) += imx53_auto imx53_ALSA_auto
AUTO_TARGETS-$(CONFIG_VTE_IMX5)	+= imx50_auto imx50_auto_p1 imx50_auto_p2 imx50_auto_p3
AUTO_TARGETS-$(CONFIG_VTE_IMX5) += imx53_LOCO_auto imx53_SMD_auto
AUTO_TARGETS-$(CONFIG_VTE_IMX28) += imx28_auto
P_AUTO_TARGETS-$(CONFIG_VTE_IMX5) += imx51_p_auto imx53_p_auto imx50_p_auto
P_AUTO_TARGETS-y += imx51_p_auto imx53_p_auto imx50_p_auto
P_AUTO_TARGETS-$(CONFIG_VTE_IMX28) += imx28_p_auto

all:

install:
	-mkdir ../../../runtest
	-for i in $(AUTO_TARGETS-y); do \
		ln -f $$i ../../../runtest/ ;\
	done
	-for i in $(P_AUTO_TARGETS-y); do \
		./patch_p_auto $$i;  \
		ln -f vte_$$i ../../../runtest/; \
	done

clean:
	-rm -rf vte_*
	for i in $(AUTO_TARGETS-y); do \
		rm -rf ../../../runtest/$$i ;\
	done
	-for i in $(P_AUTO_TARGETS-y); do \
		rm -rf ../../../runtest/vte_$$i; \
	done


