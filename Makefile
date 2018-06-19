DESCRIPTION = "Mashcontrol"
LICENSE = "GPLv3"
VERSION = 0.5

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

CC=gcc
CFLAGS += -Wall
CFLAGS += -lwiringPi
CFLAGS += -I.


mashcontrol: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJ) mashcontrol

# make variables:
#  @ contains target of each rule
#  ^ contains dependencies of each rule
#  < first item of dependency list
# use all variables with $
#
# one peculiarity of the make syntax, as opposed to say the bourne shell syntax, 
# is that only the first character following the dollar sign is considered to be 
# the variable name. If we want to use longer names, we have to parenthesize the 
# name before applying the dollar sign to extract its value
# http://nuclear.mutantstargoat.com/articles/make/
# http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/