#
#	Makefile for PLTEPS under MS-DOS with Turbo-C 2.0 compiler.
#
#	Use "make -fsc.mak".
#

LD	= tcc
CC	= tcc
SC	= ..
MODEL	= l
CFLAGS  = -O -m$(MODEL) -w-pia
LDFLAGS	= -m$(MODEL)

OBJ     = driver.obj tphp.obj

plteps.exe: $(OBJ)
	$(LD) $(LDFLAGS) -eplthp *.obj

tphp.obj: $(SC)\tphp.c
	tcc -c $(CFLAGS) -otphp $(SC)\tphp.c

.c.obj:
	$(CC) -c $(CFLAGS) $<
