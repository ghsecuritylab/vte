#Copyright (C) 2005-2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html

CC = $(CROSS_COMPILE)gcc

ifeq ($(CONFIG_VTE_IMX6),y)
CFLAGS += -DMX6
endif

ifeq ($(CONFIG_VTE_IMX5),y)
CFLAGS += -DMX5
endif
ifeq ($(CONFIG_VTE_IMX233),y)
CFLAGS += -DMX233
endif

TARGET = iim_test
#SRC = $(wildcard *.c)
SRC=iim_test.c
OBJ=$(patsubst %.c,%.o,$(SRC))

all: $(OBJ)
	echo $(CFLAGS)
	$(CC) -o $(TARGET) $(LDFLAGS) $(OBJ)
	
install:
	-ln -f $(TARGET) ../../bin/

clean:
	rm -f *.o
	rm -f $(TARGET)
	
$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $^
