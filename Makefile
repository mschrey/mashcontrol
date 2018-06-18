DESCRIPTION = "Mashcontrol"
LICENSE = "GPLv3"
VERSION = 0.5

CC=gcc
CCFLAGS += -Wall
CCFLAGS += -lwiringPi

default: all

all: mashcontrol

mashcontrol: mashcontrol.o 
	$(CC) -o mashcontrol $(CCFLAGS) $(LDFLAGS)

clean:
	rm -f *.o mashcontrol