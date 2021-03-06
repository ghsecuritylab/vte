#!/bin/sh
# will check the CAMERA env as the deafult camera
# possbile CAMERA are ov3640, ov5640, ov5642

camera_module=


rmodule()
{
 if [ "$NO_REMOVE" = "Y" ]; then
 	echo "not remove $1"
 else
	modprobe -r $1 || return $?
    sleep 1
 fi

 return 0
}

check_platform_camera()
{
 #only check i2c camera
 find=0
 if [ `uname -r` \> "3.0.35" ]; then
	#on 3.5.7 and later kernel, i2c dir is /sys/class/i2c-dev/i2c-0, i2c-1, i2c-2
	bus_if=$(ls -d /sys/class/i2c-dev/i2c-*)
 else
	bus_if=$(ls -d /sys/devices/platform/imx-i2c.*)
 fi
 #if there are multi camera support then choice the given one or last one
 for j in $bus_if; do
    set -x
	if [ `uname -r` \> "3.0.35" ]; then
		names=$(cat $j/device/*/name |grep ov)
	else
		names=$(find $j -name name | xargs grep ov)
	fi

    set +x
	for i in $names ; do
		apd=
		camera_all=$(echo $i | cut -d ':' -f 2)
		camera=$(echo $camera_all | cut -d '_' -f 1)
		appends=$(echo $camera_all | grep '_' | cut -d '_' -f 2)
		if [ "$camera" = "ov564x" ]; then
			camera_module=ov5640_camera
			continue
		else
			if [ "$appends" ]; then
				apd=_${appends}
			fi
			if [ $find -eq 1  ]; then
				continue
			fi
			if [ ! -z "$CAMERA"  ];then
				if [ "$CAMERA" = ${camera}${apd} ]; then
					camera_module=$(echo $camera_module  ${camera}_camera${apd})
					find=1
				fi
			elif [ "$DUAL" = "2"  ];then
					camera_module=$(echo $camera_module  ${camera}_camera${apd})

			else
				sleep 1
				camera_module=$(echo $camera_module  ${camera}_camera${apd})
			fi
		fi
	done
 done

 if [ -z "$camera_module" ]; then
	return 1
 else
 	return 0
 fi
}

v4l_setup()
{
   #turn on the display
   echo 0 > /sys/class/graphics/fb0/blank
   sleep 1

   #keep cursor on
   echo -e "\033[9;0]" > /dev/tty0
   check_platform_camera
   for cm in $camera_module; do
        modprobe $cm
   done
   modprobe mxc_v4l2_capture || return 1
   
   sleep 1

   retry=5
   while [ ! -e /dev/video0 ]; do
	   sleep 1
	   if [ $retry -eq 0 ]; then
		   return 1;
	   fi
   
   retry=$(expr $retry - 1)
   done

   return 0
}

v4l_cleanup()
{
	CAMERA=
	check_platform_camera || RC=1

	if [ $RC -ne 0 ]; then
		echo "can not find support ovCameras for this platform"
		exit 1
	fi

	set -x
	for cm in $camera_module; do
		rmodule $cm
	done
	sleep 2
	rmodule mxc_v4l2_capture || return 1
	set +x
}


#main
RC=0

echo $CAMERA

if [ -z "$CAMERA" ]; then
	if [ "$DUAL" = "2"  ]  ; then
  		echo "dual camera support"
  	elif [ "$DUAL" = "3" ] ; then
		echo "will probe all registed camera"
	else
		pt=$(platfm.sh)
		case "$pt" in
		'IMX6DL-Sabre-SD'):
		;;
		'IMX6Q-Sabre-SD') :
		;;
		'*') :
  		CAMERA=ov5640
		;;
		esac
	fi
fi



case "$1" in
'setup') :
  v4l_setup || RC=2
  ;;
'cleanup') :
  v4l_cleanup || RC=3
   ;;
'*') :
   ;;
esac

if [ $RC  -ne 0  ];then
  echo "Fail to $1 v4l"
  exit $RC
else
  echo "OK to $1 v4l"
  if echo $camera_module | grep ov5642_camera && [ "$1" = "setup" ]
  then
	  #special for ov5642 which only support some modes on 6q Lite platform
	exit 56
  else
	exit $RC
  fi
fi
