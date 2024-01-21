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

SC      = ..
CFLAGS  = -O
LDFLAGS = -lm

OBJ     = driver.o tphp.o

plthp:  $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o plthp

tphp.o: $(SC)/tphp.c
	$(CC) -c $(CFLAGS) $(SC)/tphp.c
#       mv $(SC)/tphp.o tphp.o

clean:
	rm -f *.b *.o core a.out plthp
