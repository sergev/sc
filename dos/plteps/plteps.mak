#
#	Makefile for PLTEPS under MS-DOS with Turbo-C 2.0 compiler.
#
#	Use "make -fsc.mak".
#

LD	= tcc
CC	= tcc
SC	= ..
MODEL	= s
CFLAGS  = -O -m$(MODEL) -w-pia
LDFLAGS	= -m$(MODEL)

OBJ     = driver.obj eps.obj

plteps.exe: $(OBJ)
	$(LD) $(LDFLAGS) -eplteps *.obj

eps.obj: $(SC)\eps.c
	tcc -c $(CFLAGS) -oeps $(SC)\eps.c

.c.obj:
	$(CC) -c $(CFLAGS) $<
