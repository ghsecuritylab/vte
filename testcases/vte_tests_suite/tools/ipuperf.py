#!/usr/bin/python

##############################################################################
#Copyright (C) 2012 Freescale Semiconductor, Inc. All Rights Reserved.
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
# Andy Tian             08/03/2012       n/a        Initial ver.
#############################################################################

"""
#############################################################################
General Description:
Use this python program to abstract ipu performance data from running log;
Run it on your linux sever.
Must set VTE env before running it.
Input:
The log file can be the log generated by VTE auto test system or it can be
the log file saved when running it manually.
Use the '-f' option if you want to use the manually saved log.
If using the manually saved log, make sure the log file is pure and integrated,
the program will not do such check if -f option is used.
If it is vte auto log, the program will try to find the first integrated log
section or longest log section if no integrated log found.
Output:
There may be two output files under $LTPROOT/output named with xxx.ipusec and
xxx.ipudata, xxx is your user name.
.ipudata is the ipu performace data we need also we can get from sreen outpu;
.ipusec is only available if vte log is used.
#############################################################################
"""

import sys
import os

def usage():
	print("Usage: %s [-f] logfile\n" % sys.argv[0])
	print("Use the -f option when the logfile is maually saved file")
	sys.exit(1)

#check the VTE env
if os.environ.get("LTPROOT") == None:
	print("VTE env is not set!")
	print("Please set VTE env before running this program.")
	sys.exit(1)


logFile=""
user=os.environ["USER"]
ipuTmp=user+".tmp"
ipuPerf=user+".ipusec"
ipuData=user+".ipudata"
preLen=0
logStatus=0
sCount=0
manual=0

argc=len(sys.argv)

if argc == 2:
	if sys.argv[1] in ("-h","-help","--help"):
		usage()
	else:
		logFile=sys.argv[1]
elif argc == 3 and sys.argv[1] == "-f":
	logFile=sys.argv[2]
	manual=1
else:
	usage()



#define the perf data dictionary
#the value is a list;value[0]=total_fps, value[1]=times, value[2]=average_fps
perD={"Rotation 352x288 fps":[0,0,0], "Rotation 1280x720 fps":[0,0,0],
      "Rotation 1920x1080 fps":[0,0,0], "Rotation 640x480 fps":[0,0,0],
      "Resize 352x288 fps":[0,0,0], "Resize 1280x720 fps":[0,0,0],
      "Resize 1920x1080 fps":[0,0,0], "Resize 640x480 fps":[0,0,0],
      "Single CSC 352x288 fps":[0,0,0], "Single CSC 1280x720 fps":[0,0,0],
      "Single CSC 1920x1080 fps":[0,0,0], "Single CSC 640x480 fps":[0,0,0],
      "CSC Crop 352x288 fps":[0,0,0], "CSC Crop 1280x720 fps":[0,0,0],
      "CSC Crop 1920x1080 fps":[0,0,0], "CSC Crop 640x480 fps":[0,0,0],
      "CSC Resize 352x288 fps":[0,0,0], "CSC Resize 1280x720 fps":[0,0,0],
      "CSC Resize 1920x1080 fps":[0,0,0], "CSC Resize 640x480 fps":[0,0,0],
      "Redundant test":[0,0,0]
      }
#define the print order list for perD
pList=["Rotation 352x288 fps", "Rotation 1280x720 fps",
      "Rotation 1920x1080 fps", "Rotation 640x480 fps",
      "Resize 352x288 fps", "Resize 1280x720 fps",
      "Resize 1920x1080 fps", "Resize 640x480 fps",
      "Single CSC 352x288 fps", "Single CSC 1280x720 fps",
      "Single CSC 1920x1080 fps", "Single CSC 640x480 fps",
      "CSC Crop 352x288 fps", "CSC Crop 1280x720 fps",
      "CSC Crop 1920x1080 fps", "CSC Crop 640x480 fps",
      "CSC Resize 352x288 fps", "CSC Resize 1280x720 fps",
      "CSC Resize 1920x1080 fps", "CSC Resize 640x480 fps"
      ]


#find the abs path for the log file
targetDir=os.environ["LTPROOT"]+"/output/"
if logFile[0] == "/":
	#the given file is absolute path
	fpFile=logFile
else:
	fpFile=targetDir+logFile.split("/")[-1]

#change the directory to output
os.chdir(targetDir)

#try to open the given log file
try:
	fPoint1=open(fpFile,'r')
except IOError:
	print("%s can not be opened." % fpFile)
	print("Please make sure %s exists." % fpFile)
	sys.exit(1)

if manual == 0:
	#the log file from VTE auto system
	#abstract the first integrated ipu perf log or the longest
	line=fPoint1.readline()
	while line:
		#an ipu searching cycle
		if '''tag=TGE-LV-IPU_DEV-0070''' in line:
			fPoint2=open(ipuTmp,'w')
			lenth=0
			#save the ipu log section to the temporary file
			while '''<<<test_start>>>''' not in line and line != "":
				fPoint2.write(line)
				preLine=line
				lenth+=1
				line=fPoint1.readline()
			fPoint2.close()
			#replace our logfile with the integrated or longest log section
			if '''<<<test_end>>>''' in preLine:
				#get an integrated ipu perf log, rename the perf log
				#and stop searching
				#assign 1 to logStatus to indicate it is a integrated log
				logStatus=1
				os.rename(ipuTmp, ipuPerf)
				break
			#ipu perf log only partial, save the longest log
			elif lenth > preLen:
				#log is partial
				logStatus=2
				os.rename(ipuTmp, ipuPerf)
				preLen=lenth
		line=fPoint1.readline()
	fPoint1.close()

	#deal with logStatus
	if logStatus == 2:
		print("")
		print("{:*^65}".format("Warning!!!"))
		print("{:*^65}".format("Ipu performance log is not integrated in your given logfile!"))
		print("{:*^65}".format("Warning!!!"))
		print("")
	elif logStatus == 0:
		print("")
		print("{:*^65}".format("Error!!!"))
		print("{:*^65}".format("No ipu performance log found in your given logfile!"))
		print("{:*^65}".format("Make sure you assigned the right logfile!"))
		print("{:*^65}".format("Error!!!"))
		print("")
		sys.exit(1)

	#abstract the performance data we need
	fPoint3=open(ipuPerf,'r')
else:
	fPoint3=fPoint1
	logStatus=1

#abstract the ipu performance data
line=fPoint3.readline()
iPara=8*['0']
oPara=8*['0']
while line:
	if '''input:''' in line:
		line=fPoint3.readline()
		#get the input parameters
		while line:
			try:
				if "foramt:" in line:
					iPara[0]=line.split()[-1]
				elif "width:" in line:
					iPara[1]=line.split()[-1]
				elif "height:" in line:
					iPara[2]=line.split()[-1]
				elif "crop.w = " in line:
					iPara[4]=line.split()[-1]
				elif "crop.h = " in line:
					iPara[5]=line.split()[-1]
				elif "crop.pos.x = " in line:
					iPara[6]=line.split()[-1]
				elif "crop.pos.y = " in line:
					iPara[7]=line.split()[-1]
					break
				line=fPoint3.readline()
			except IndexError:
				#there is some format error in log file,the key becomes untrustable
				Key="Redundant test"
				break
	if '''output:''' in line:
		line=fPoint3.readline()
		#get the output parameters
		while line:
			try:
				if "foramt:" in line:
					oPara[0]=line.split()[-1]
				elif "width:" in line:
					oPara[1]=line.split()[-1]
				elif "height:" in line:
					oPara[2]=line.split()[-1]
				elif "roate:" in line:
					oPara[3]=line.split()[-1]
				elif "crop.w = " in line:
					oPara[4]=line.split()[-1]
				elif "crop.h = " in line:
					oPara[5]=line.split()[-1]
				elif "crop.pos.x = " in line:
					oPara[6]=line.split()[-1]
				elif "crop.pos.y = " in line:
					oPara[7]=line.split()[-1]
					break
				line=fPoint3.readline()
			except IndexError:
				#there is some format error in log file,the key becomes untrustable
				Key="Redundant test"
				break

		#get the key by comparing the iPara with oPara
		if iPara[0:3]+iPara[4:8] == oPara[0:3]+oPara[4:8] and oPara[3] != 0:
			#it is rotation only case
			key="Rotation "+iPara[1]+"x"+iPara[2]+" fps"
		elif iPara[0]==oPara[0] and oPara[3]=='0' and iPara[1:3] != oPara[1:3]:
			#it is resize only case
			key="Resize "+iPara[1]+"x"+iPara[2]+" fps"
		#elif iPara[1:4] == oPara[1:4] and iPara[0] != oPara[0]:
		elif iPara[0] != oPara[0] and oPara[1:8] == iPara[1:8]:
			#it is csc only case
			key="Single CSC "+iPara[1]+"x"+iPara[2]+" fps"
		elif iPara[0] != oPara[0]  and oPara[3] != '0' and iPara[1:3]+iPara[4:8]==oPara[1:3]+oPara[4:8]:
			#it is csc + rotate case, now no report request for it
			pass
		elif iPara[4:8] == ['64','64','32','32'] or oPara[4:8] == ['64','64','32','32']:
			#it is csc + crop case
			key="CSC Crop "+iPara[1]+"x"+iPara[2]+" fps"
		elif iPara[0:3] != oPara[0:3] and oPara[3]=='0':
			#it is csc + resize case
			key="CSC Resize "+iPara[1]+"x"+iPara[2]+" fps"
		else:
			Key="Redundant test"
	if "us, fps" in line:
		#ipu task successful, analyze the line and record the value
		try:
			fps=float(line.split()[-1])
		except ValueError:
			#there is format error in fps line, setting fps to 0 to ignore the result
			fps=0
		try:
			if fps > 1:
				#if fps less than 1, it is thought as invalid test data
				perD[key][0] += fps
				perD[key][1] += 1
			else:
				#count the number for very bad performance data
				sCount += 1
		except KeyError:
			#there is some format error in the log file by console interrupt
			#ignore the result for this time
			pass
	line=fPoint3.readline()
fPoint3.close()

#deal with the ipu perf data
fPoint4=open(ipuData,'w')
for key in pList:
	if perD[key][1] != 0:
		#get the average fps
		perD[key][2] = perD[key][0]/perD[key][1]
	fPoint4.write("%-30s%30f\n"% (key, perD[key][2]))
	print("%-30s%30f"% (key, perD[key][2]))
fPoint4.write("There are %d times whose ipu fps less than 1.\n" % (sCount))
print("There are %d times whose ipu fps less than 1." % (sCount))
if logStatus == 2:
	fPoint4.write("\n\n%s"%"Ipu performance log is not integrated in your given logfile!")
fPoint4.close()
if manual == 0:
	print("\nYour ipu log saved as %s" % (targetDir+ipuPerf))
print("Your ipu performance data saved as %s" % (targetDir+ipuData))
print("Suggest you copy the file/files to your local dir")