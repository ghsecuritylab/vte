#Copyright (C) 2005-2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html

CFLAGS += -O2
LDFLAGS +=

SRC = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SRC))

TARGET = dumpmem

all: $(OBJS)
	$(CC) -o $(TARGET) $(LDFLAGS) $(OBJS)

install:
	-ln -f $(TARGET) ../../../bin/

clean:
	-rm -f $(TARGET) *.o

$(OBJS): $(SRC)


