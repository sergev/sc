#
#       SC - spreadsheet calculator, varsion 3.0.
#
#       (c) Copyright DEMOS, 1989
#       All rights reserved.
#
#       Authors: Serg I. Ryshkov, Serg V. Vakulenko
#
#       Mon Apr 03 10:28:41 MSK 1989
#

SHELL	= /bin/sh

SC      = ../src
CFLAGS  = -O

OBJ     = driver.o eps.o

plteps: $(OBJ)
	$(CC) $(LDFLAGS) driver.o eps.o -o plteps

eps.o: $(SC)/eps.c
	$(CC) -c $(CFLAGS) $(SC)/eps.c
#       mv $(SC)/eps.o eps.o

clean:
	rm -f *.b *.o core a.out plteps
