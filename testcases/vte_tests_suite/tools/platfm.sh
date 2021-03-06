#!/bin/bash
##############################################################################
#Copyright 2008-2013 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
##############################################################################
#
# Revision History:
#                      Modification     Tracking
# Author                   Date          Number    Description of Changes
#-------------------   ------------    ----------  ---------------------
# Spring Zhang/B17931   27/11/2008       n/a        Initial ver. 
# Spring                28/11/2008       n/a        Modify COPYRIGHT header
# Spring                15/01/2008       n/a        Add MX35TO2 judgement,
#                                                   move to tools/
# Spring                02/04/2009       n/a        Add MX51Babbage support
# Spring                02/08/2009       n/a        Use own determination
# Spring                11/03/2009       n/a        Add MX28EVK support
# Spring                18/03/2009       n/a        Add MX53EVK support
# Hake                  01/10/2011       n/a        Add LOCO and SMD 53. etc
# Hake                  03/23/2011       n/a        Add MX50 rdp3 etc
# Spring                07/04/2011       n/a        Add MX53SMD TO2.1 support
# Spring                11/22/2011       n/a        Add MX6Q description
# Spring                09/02/2013       n/a        Use L3.10 new framework
#############################################################################
# Usage1(return string):
#   platform=`platfm.sh`
#   Then use $platform to judge the platform
#
#   $platform   PLATFORM
#   IMX31ADS    IMX31ADS
#   IMX32ADS    IMX32ADS
#   IMX25-3STACK IMX25-3STACK
#   IMX31-3STACK IMX31-3STACK
#   IMX35-3STACK IMX35-3STACK
#   IMX37-3STACK IMX37-3STACK
#   IMX51-3STACK IMX51-3STACK
#   IMX51-BABBAGE IMX51-BABBAGE
#   IMX28EVK    IMX28EVK
#   IMX53EVK    IMX53EVK
#   IMX53LOCO   IMX53LOCO
#   IMX53SMD    IMX53SMD
#   IMX50ARM2   IMX50ARM2
#   IMX50RDP    IMX50RDP
#   IMX50-RDP3    IMX50-RDP3
#   IMX6Q-Sabre-SD IMX6Q-Sabre-SD
#   IMX6-SABREAUTO IMX6-SABREAUTO
#   IMX6DL-Sabre-SD IMX6DL-Sabre-SD
#   IMX6Solo-SABREAUTO IMX6Solo-SABREAUTO
#   IMX6DL-SABREAUTO IMX6DL-SABREAUTO
#   IMX6SL-EVK IMX6SL-EVK
#   IMX6-SABRELITE IMX6-SABRELITE
#   IMX6ARM2    IMX6ARM2
#
# Usage2(return number): 
#   platfm.sh || platform=$?
#   Then use $platform to judge the platform
#
# Return value:
# 1. 31~53 for mx31~mx53 board.
#     rt value  Board
# e.g.  31       mx31
#       35       mx35
#       41       mx51 babbage
#       51       mx51 3ds
#       28       mx28 evk
#       53       mx53 evk loco smd
#       63       mx6q arm2 and sabre-lite
#       61       mx6dl
#
# 2. 378%256(=122) for SMTP378X board.(for return value is 0~255)
#       rt value    Board
# e.g.  378%256   SMTP378X
#
# 3. 67 for "Platform not recognized".(67 can be defined to other num)

# Some Platform Info
#35 TO2 platform:
#Revision        : 35120


# Find platform type in DT kernel
determine_platform_dt()
{
    local find=0
    RC=67

    find=`grep "MX6Q" /sys/devices/soc0/soc_id |wc -l`
    if [ $find -eq 1 ]
    then
        RC=63
    fi

    find=`grep "MX6DL" /sys/devices/soc0/soc_id |wc -l`
    if [ $find -eq 1 ]
    then
        RC=61
    fi

    find=`grep "MX6SL" /sys/devices/soc0/soc_id |wc -l`
    if [ $find -eq 1 ]
    then
        RC=60
    fi

    find=`grep "MX6 Quad SABRE Smart Device" /sys/devices/soc0/machine |wc -l`
    if [ $find -eq 1 ]
    then
        p=IMX6Q-Sabre-SD
    fi

    find=`grep "MX6 DualLite SABRE Smart Device" /sys/devices/soc0/machine |wc -l`
    if [ $find -eq 1 ]
    then
        p=IMX6DL-Sabre-SD
    fi

    find=`grep "MX6 Quad SABRE Automotive" /sys/devices/soc0/machine |wc -l`
    if [ $find -eq 1 ]
    then
        p=IMX6-SABREAUTO
    fi

    find=`grep "MX6 DualLite/Solo SABRE Automotive" /sys/devices/soc0/machine |wc -l`
    if [ $find -eq 1 ]
    then
        p=IMX6DL-SABREAUTO
    fi

    find=`grep "MX6 SoloLite EVK" /sys/devices/soc0/machine |wc -l`
    if [ $find -eq 1 ]
    then
        p=IMX6SL-EVK
    fi
}

# Find the platform type
determine_platform()
{
    local find=0
    
    # Determine the platform
    find=`cat /proc/cpuinfo | grep "Revision" | grep " 31.*" | wc -l`;
    if [ $find -eq 1 ]
    then
        p=IMX31-3STACK
    fi

	find=`cat /proc/cpuinfo | grep "Revision" | grep " 25.*" | wc -l`;
	if [ $find -eq 1 ]
	then
		p=IMX25-3STACK
	fi

    find=`cat /proc/cpuinfo | grep "Revision" | grep " 35.*" | wc -l`;
    if [ $find -eq 1 ]
    then
        p=IMX35-3STACK
    fi

    find=`cat /proc/cpuinfo | grep "Revision" | grep " 37.*" | wc -l`;
    if [ $find -eq 1 ]
    then
        p=IMX37-3STACK
    fi

    # MX51 TO2.0: Revision: 51020
    find=`cat /proc/cpuinfo | grep "Revision" | grep " 51.*" | wc -l`;
    if [ $find -eq 1 ]
    then
        p=IMX51-3STACK
    fi

    # MX51 Babbage TO1.1: Revision: 51011
    # MX51 Babbage TO3.0: Revision: 51130
    find=`cat /proc/cpuinfo | grep "Hardware" | grep "Babbage" | wc -l`;
    if [ $find -eq 1 ]
    then
        p=IMX51-BABBAGE
    fi

    # MX53 EVK TO1.0: Revision: 53010
    find=`cat /proc/cpuinfo | grep "Revision" | grep "53.10" | wc -l`;
    if [ $find -eq 1 ]
    then
        p=IMX53EVK
		find=`cat /proc/cpuinfo | grep "Hardware" | grep "MX53 SMD" | wc -l`;
       if [ $find -eq 1 ]; then
			 		p=IMX53SMD
				fi
    fi
    
	# MX53 LOCO TO2.0: Revision: 53020
    find=`cat /proc/cpuinfo | grep "Revision" | grep "53.20" | wc -l`;
    if [ $find -eq 1 ]
    then
	    find=`cat /proc/cpuinfo | grep "Hardware" | grep "MX53 LOCO" | wc -l`;
        if [ $find -eq 1 ]; then
            p=IMX53LOCO
        fi
		find=`cat /proc/cpuinfo | grep "Hardware" | grep "MX53 SMD" | wc -l`;
        if [ $find -eq 1 ]; then
            p=IMX53SMD
        fi
    fi

   	# MX53 LOCO TO2.1: Revision: 53321
    find=`cat /proc/cpuinfo | grep "Revision" | grep "53.21" | wc -l`;
    if [ $find -eq 1 ]
    then
		find=`cat /proc/cpuinfo | grep "Hardware" | grep "MX53 LOCO" | wc -l`;
        if [ $find -eq 1 ]; then
	        p=IMX53LOCO
        fi
		find=`cat /proc/cpuinfo | grep "Hardware" | grep "MX53 SMD" | wc -l`;
        if [ $find -eq 1 ]; then
            p=IMX53SMD
        fi
    fi

    #find STMP378X
    find=`cat /proc/cpuinfo | grep "Hardware" | grep " 378X" | wc -l`;
    if [ $find -eq 1 ]
    then
        p=SMTP378X
    fi

    find=`cat /proc/cpuinfo | grep "Hardware" | grep " 23.*" | wc -l`;
    if [ $find -eq 1 ]
    then
        p=IMX23EVK
    fi
    #find MX28EVK
    find=`cat /proc/cpuinfo | grep "Hardware" | grep "MX28EVK" | wc -l`;
    if [ $find -eq 1 ]
    then
        p=IMX28EVK
    fi

    #find MX50ARM2
    find=`cat /proc/cpuinfo | grep "Hardware" | grep "MX50 ARM2" | wc -l`;
    if [ $find -eq 1 ]
    then
        p=IMX50ARM2
    fi
    
		#find MX50RDP
    find=`cat /proc/cpuinfo | grep "Hardware" | grep "MX50 Reference Design" | wc -l`;
    if [ $find -eq 1 ]
    then
    	find=`cat /proc/cpuinfo | grep "Revision" | grep "50311" | wc -l`;
        if [ $find -eq 1 ]
        then
            p=IMX50-RDP3
        else
            p=IMX50RDP
        fi
    fi
		
    find=`cat /proc/cpuinfo | grep "Hardware" | grep "6Quad" | grep "Armadillo2 Board" | wc -l`;
    if [ $find -eq 1 ]
    then
        find=`cat /proc/cpuinfo | grep "Revision" | grep "63" | wc -l`;
		if [ $find -eq 1 ]; then
        	p=IMX6ARM2
		else
        	find=`cat /proc/cpuinfo | grep "Revision" | grep "61" | wc -l`;
			if [ $find -eq 1 ]; then
        		p=IMX6DL-ARM2
			fi
		fi
    fi
	
    find=`cat /proc/cpuinfo | grep "Hardware" | grep "6Quad" | grep "Sabre Auto" | wc -l`;
    if [ $find -eq 1 ]
    then
        find=`cat /proc/cpuinfo | grep "Revision" | grep "63" | wc -l`;
		if [ $find -eq 1 ]; then
        	p=IMX6-SABREAUTO
		fi
    fi

    find=`cat /proc/cpuinfo | grep "Hardware" | grep "Sabre-SD" | wc -l`;
    if [ $find -eq 1 ]
    then
        find=`cat /proc/cpuinfo | grep "Revision" | grep "63" | wc -l`;
		if [ $find -eq 1 ]; then
        	p=IMX6Q-Sabre-SD
		fi
        find=`cat /proc/cpuinfo | grep "Revision" | grep "61" | wc -l`;
		if [ $find -eq 1 ]; then
        	p=IMX6DL-Sabre-SD
		fi
    fi

	find=`cat /proc/cpuinfo | grep "Hardware" | grep "Solo" | grep "Sabre Auto" | wc -l`;
    if [ $find -eq 1 ]
    then
        find=`cat /proc/cpuinfo | grep "Revision" | grep "61" | wc -l`;
		if [ $find -eq 1 ]; then
            find=`cat /proc/cpuinfo | grep "BogoMIPS" | wc -l`
            if [ $find -eq 1 ]; then
                p=IMX6Solo-SABREAUTO
            elif [ $find -eq 2 ]; then
                p=IMX6DL-SABREAUTO
            fi
		fi
    fi

	find=`cat /proc/cpuinfo | grep "Hardware" | grep "6SoloLite" | grep "Armadillo2" | wc -l`;
    if [ $find -eq 1 ]
    then
        find=`cat /proc/cpuinfo | grep "Revision" | grep "60" | wc -l`;
		if [ $find -eq 1 ]; then
        	p=IMX6Sololite-ARM2
		fi
    fi

	find=`cat /proc/cpuinfo | grep "Hardware" | grep "6SoloLite" | grep "EVK" | wc -l`;
    if [ $find -eq 1 ]
    then
        find=`cat /proc/cpuinfo | grep "Revision" | grep "60" | wc -l`;
		if [ $find -eq 1 ]; then
        	p=IMX6SL-EVK
		fi
    fi


	find=`cat /proc/cpuinfo | grep "Hardware" | grep "6Quad" | grep "Sabre-Lite" | wc -l`;
    if [ $find -eq 1 ]
    then
        find=`cat /proc/cpuinfo | grep "Revision" | grep "63" | wc -l`;
        p=IMX6-SABRELITE
    fi

	find=`cat /proc/cpuinfo | grep "Hardware" | grep "i.MX6 Quad"| wc -l`;		
    if [ $find -eq 1 ]		
    then		
        find=`cat /proc/cpuinfo | grep "Revision" | grep "63" | wc -l`;		
        p=IMX6-SABRELITE		
    fi

    # L3.5.7 way
    find=`cat /proc/cpuinfo | grep "Hardware" | grep "Device Tree" | wc -l`;
    if [ $find -eq 1 ]
    then
        if grep imx6q-sabresd /proc/device-tree/compatible > /dev/null; then
            p=IMX6Q-Sabre-SD
        fi
        if grep imx6q-sabreauto /proc/device-tree/compatible > /dev/null; then
            p=IMX6-SABREAUTO
        fi
        if grep imx6dl-sabresd /proc/device-tree/compatible > /dev/null; then
            p=IMX6DL-Sabre-SD
        fi
        if grep imx6dl-sabreauto /proc/device-tree/compatible > /dev/null; then
            p=IMX6DL-SABREAUTO
        fi
    fi


    if [ "$p" = "IMX31-3STACK" ]
    then
        #echo "Platform MX31"
        RC=31
	elif [ "$p" = "IMX25-3STACK"  ]
	then
		RC=25
    elif [ "$p" = "IMX35-3STACK" ]
    then
        #echo  "Platform MX35" 
        RC=35
    elif [ "$p" = "IMX37-3STACK" ]
    then
        #echo  "Platform MX37" 
        RC=37
		elif [ "$p" = "IMX23EVK" ]
		then
		    RC=23
    elif [ "$p" = "IMX51-3STACK" ]
    then
        #echo  "Platform MX51" 
        RC=51
    elif [ "$p" = "IMX51-BABBAGE" ]
    then
        #echo  "Platform MX51 Babbage" 
        RC=41
    elif [ "$p" = "IMX53EVK" ] || [ "$p" = "IMX53LOCO" ] || [ "$p" = "IMX53SMD" ]
    then
        #echo  "Platform MX53 EVK" 
        RC=53
     elif [ "$p" = "SMTP378X" ]
    then
        #echo  "Platform SMTP378X" 
        let RC=378%256
    elif [ "$p" = "IMX28EVK" ]
    then
        #echo  "Platform MX28 EVK" 
        RC=28
    elif [ "$p" = "IMX50ARM2" ]
    then
        RC=50
    elif [ "$p" = "IMX50RDP" ] || [ "$p" = "IMX50-RDP3" ]
    then
        RC=50
    elif [ "$p" = "IMX6-SABREAUTO" ] || [ "$p" = "IMX6-SABRELITE" ] || [ "$p" = "IMX6ARM2"  ] || [ "$p" = "IMX6Q-Sabre-SD" ]
    then
        RC=63
    elif [ "$p" = "IMX6DL-ARM2" ] || [ "$p" = "IMX6Solo-SABREAUTO"  ] || [ "$p" = "IMX6DL-Sabre-SD"  ] || [ "$p" = "IMX6DL-SABREAUTO" ]
    then
        RC=61
	elif [ "$p" = "IMX6Sololite-ARM2" ] || [ "$p" = "IMX6SL-EVK"  ]
	then
		RC=60
    else
        #echo  "Platform not recognized!"
        RC=67
    fi

    return $RC
}


# main
RC=0

if [ -e /sys/devices/soc0/machine ]; then
    determine_platform_dt
else
    determine_platform
fi

echo "$p"

exit $RC

#############################
allcpu_info()
{
    cat << EOF
MX28EVK TO1.0 - 201011
root@localhost /staf/retail$ cat /proc/cpuinfo
Processor       : ARM926EJ-S rev 5 (v5l)
BogoMIPS        : 478.41
Features        : swp half thumb fastmult edsp java
CPU implementer : 0x41
CPU architecture: 5TEJ
CPU variant     : 0x0
CPU part        : 0x926
CPU revision    : 5

Hardware        : Freescale MX28EVK board
Revision        : 0000
Serial          : 0000000000000000

MX51 Babbage TO3.0 - 201011
root@freescale ~$ cat /proc/cpuinfo
Processor       : ARMv7 Processor rev 5 (v7l)
BogoMIPS        : 799.53
Features        : swp half thumb fastmult vfp edsp neon vfpv3
CPU implementer : 0x41
CPU architecture: 7
CPU variant     : 0x2
CPU part        : 0xc08
CPU revision    : 5

Hardware        : Freescale MX51 Babbage Board
Revision        : 51130
Serial          : 0000000000000000


MX53 EVK TO1.0 - 201012
Processor       : ARMv7 Processor rev 5 (v7l)
BogoMIPS        : 799.53
Features        : swp half thumb fastmult vfp edsp neon vfpv3
CPU implementer : 0x41
CPU architecture: 7
CPU variant     : 0x2
CPU part        : 0xc08
CPU revision    : 5

Hardware        : Freescale MX53 EVK Board
Revision        : 53010
Serial          : 0000000000000000


MX53 SMD TO2.1 - 201104
Processor       : ARMv7 Processor rev 5 (v7l)
BogoMIPS        : 999.42
Features        : swp half thumb fastmult vfp edsp neon vfpv3
CPU implementer : 0x41
CPU architecture: 7
CPU variant     : 0x2
CPU part        : 0xc08
CPU revision    : 5

Hardware        : Freescale MX53 SMD Board
Revision        : 53321
Serial          : 0000000000000000


MX53 LOCO Ripley Rev.A - 201108
Processor       : ARMv7 Processor rev 5 (v7l)
BogoMIPS        : 399.76
Features        : swp half thumb fastmult vfp edsp neon vfpv3
CPU implementer : 0x41
CPU architecture: 7
CPU variant     : 0x2
CPU part        : 0xc08
CPU revision    : 5

Hardware        : Freescale MX53 LOCO Board
Revision        : 53121
Serial          : 0000000000000000

MX6Q ARM2 board -201111
Processor       : ARMv7 Processor rev 10 (v7l)
processor       : 0
BogoMIPS        : 1985.74

processor       : 1
BogoMIPS        : 1992.29

processor       : 2
BogoMIPS        : 1992.29

processor       : 3
BogoMIPS        : 1992.29

Features        : swp half thumb fastmult vfp edsp neon vfpv3
CPU implementer : 0x41
CPU architecture: 7
CPU variant     : 0x2
CPU part        : 0xc09
CPU revision    : 10

Hardware        : Freescale i.MX 6Quad Armadillo2 Board
Revision        : 63000
Serial          : 0000000000000000

----MX6Solo ARD board -201210
Processor       : ARMv7 Processor rev 10 (v7l)
processor       : 0
BogoMIPS        : 1581.05

Features        : swp half thumb fastmult vfp edsp neon vfpv3
CPU implementer : 0x41
CPU architecture: 7
CPU variant     : 0x2
CPU part        : 0xc09
CPU revision    : 10

Hardware        : Freescale i.MX 6Quad/DualLite/Solo Sabre Auto Board
Revision        : 61211
Serial          : 866511d4d72d7859


----MX6DL ARD board -201210
Processor       : ARMv7 Processor rev 10 (v7l)
processor       : 0
BogoMIPS        : 1581.05

processor       : 1
BogoMIPS        : 1581.05

Features        : swp half thumb fastmult vfp edsp neon vfpv3
CPU implementer : 0x41
CPU architecture: 7
CPU variant     : 0x2
CPU part        : 0xc09
CPU revision    : 10

Hardware        : Freescale i.MX 6Quad/DualLite/Solo Sabre Auto Board
Revision        : 61211
Serial          : 656421d4d72d785a

----MX6 board with device tree -201301
root@freescale ~$ cat /proc/cpuinfo 
Processor       : ARMv7 Processor rev 10 (v7l)
processor       : 0
BogoMIPS        : 790.52

processor       : 1
BogoMIPS        : 790.52

processor       : 2
BogoMIPS        : 790.52

processor       : 3
BogoMIPS        : 790.52

Features        : swp half thumb fastmult vfp edsp neon vfpv3 tls 
CPU implementer : 0x41
CPU architecture: 7
CPU variant     : 0x2
CPU part        : 0xc09
CPU revision    : 10

Hardware        : Freescale i.MX6 (Device Tree)
Revision        : 0000
Serial          : 0000000000000000

# cat /proc/device-tree/model
Freescale i.MX6Q SABRE Smart Device Board

----MX6 board with device tree -201303
Processor       : ARMv7 Processor rev 10 (v7l)
processor       : 0
BogoMIPS        : 790.52

processor       : 1
BogoMIPS        : 790.52

processor       : 2
BogoMIPS        : 790.52

processor       : 3
BogoMIPS        : 790.52

Features        : swp half thumb fastmult vfp edsp neon vfpv3 tls
CPU implementer : 0x41
CPU architecture: 7
CPU variant     : 0x2
CPU part        : 0xc09
CPU revision    : 10

Hardware        : Freescale i.MX6 Quad/DualLite (Device Tree)
Revision        : 63312
Serial          : 161609d4d72d7838

---MX6Q ARD board with device tree - 3.5.7 1.0 alpha
Processor       : ARMv7 Processor rev 10 (v7l)
processor       : 0
BogoMIPS        : 1988.28

processor       : 1
BogoMIPS        : 1988.28

processor       : 2
BogoMIPS        : 1988.28

processor       : 3
BogoMIPS        : 1988.28

Features        : swp half thumb fastmult vfp edsp neon vfpv3 tls
CPU implementer : 0x41
CPU architecture: 7
CPU variant     : 0x2
CPU part        : 0xc09
CPU revision    : 10

Hardware        : Freescale i.MX6 Quad/DualLite (Device Tree)
Revision        : 63412
Serial          : 121449d4d72d7394

cat /proc/device-tree/model
Freescale i.MX6Q SABRE Automotive Infotainment Board

cat /proc/device-tree/compatible
fsl,imx6q-sabreautofsl,imx6q

---MX6Q SD board with device tree - 3.5.7 1.0 alpha
Processor       : ARMv7 Processor rev 10 (v7l)                                  
processor       : 0                                                             
BogoMIPS        : 790.52                                                        
                                                                                
processor       : 1                                                             
BogoMIPS        : 790.52                                                        
                                                                                
processor       : 2                                                             
BogoMIPS        : 790.52                                                        
                                                                                
processor       : 3                                                             
BogoMIPS        : 790.52                                                        
                                                                                
Features        : swp half thumb fastmult vfp edsp neon vfpv3 tls               
CPU implementer : 0x41                                                          
CPU architecture: 7                                                             
CPU variant     : 0x2                                                           
CPU part        : 0xc09                                                         
CPU revision    : 10                                                            
                                                                                
Hardware        : Freescale i.MX6 Quad/DualLite (Device Tree)                   
Revision        : 63412                                                         
Serial          : 1f0d21d4d72d7838                                              

# cat /proc/device-tree/compatible                           
fsl,imx6q-sabresdfsl,imx6q

# cat /proc/device-tree/model                                
Freescale i.MX6Q SABRE Smart Device Board

---MX6DL SMD board with device tree - 3.5.7 1.0 alpha
Processor       : ARMv7 Processor rev 10 (v7l)
processor       : 0
BogoMIPS        : 790.52

processor       : 1
BogoMIPS        : 790.52

Features        : swp half thumb fastmult vfp edsp neon vfpv3 tls
CPU implementer : 0x41
CPU architecture: 7
CPU variant     : 0x2
CPU part        : 0xc09
CPU revision    : 10

Hardware        : Freescale i.MX6 Quad/DualLite (Device Tree)
Revision        : 61411
Serial          : 080c51d4d72d7421

root@imx6dlsabresd:~# cat /proc/device-tree/compatible
fsl,imx6dl-sabresdfsl,imx6dl

root@imx6dlsabresd:~# cat /proc/device-tree/model
Freescale i.MX6 DualLite SABRE Smart Device Board


---MX6DL ARD board with device tree - 3.5.7 1.0 alpha
Processor       : ARMv7 Processor rev 10 (v7l)
processor       : 0
BogoMIPS        : 790.52

processor       : 1
BogoMIPS        : 790.52

Features        : swp half thumb fastmult vfp edsp neon vfpv3 tls 
CPU implementer : 0x41
CPU architecture: 7
CPU variant     : 0x2
CPU part        : 0xc09
CPU revision    : 10

Hardware        : Freescale i.MX6 Quad/DualLite (Device Tree)
Revision        : 61411
Serial          : 160119d4d72d783b

root@imx6dlsabreauto:~# cat /proc/device-tree/model
Freescale i.MX6 DualLite SABRE Automotive Infotainment Board

root@imx6dlsabreauto:~# cat /proc/device-tree/compatible
fsl,imx6dl-sabreautofsl,imx6dl

---MX6DL SD board with DT - 3.10 alpha
# cat /sys/devices/soc0/revision
1.1
# cat /sys/devices/soc0/family
Freescale i.MX
# cat /sys/devices/soc0/soc_id
i.MX6DL
# cat /sys/devices/soc0/machine
Freescale i.MX6 DualLite/Solo SABRE Automotive Board

---MX6Q ARD board with DT - 3.10 alpha
root@imx6qsabreauto:~# cat /sys/devices/soc0/machine
Freescale i.MX6 Quad SABRE Automotive Board
root@imx6qsabreauto:~# cat /sys/devices/soc0/family
Freescale i.MX
root@imx6qsabreauto:~# cat /sys/devices/soc0/soc_id
i.MX6Q
root@imx6qsabreauto:~# cat /sys/devices/soc0/revision
1.2

---MX6SL EVK board with DT - 3.10 alpha
Freescale i.MX6 SoloLite EVK Board
Freescale i.MX
i.MX6SL

EOF
}

