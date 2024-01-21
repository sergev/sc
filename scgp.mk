#
#       SC - spreadsheet calculator, version 3.1.
#
#       (c) Copyright DEMOS, 1989
#       All rights reserved.
#
#       Authors: Serg I. Ryshkov, Serg V. Vakulenko
#
#       Mon Jul 31 17:17:13 MSK 1989
#

#
#       Makefile for SC under UNIX/XENIX/UTIX/DEMOS
#       using "gplot" Utility for graphics
#
#       Use "make -f scgp.mk".
#

SHELL   = /bin/sh
INSDIR  = /usr/local

# -DNOHELP		- "no help available"
# -DHELPFILE=\"...\"	- name of help file; default in core
# -DSTDHELPFILE		- name of help file is /usr/lib/sc.help
# -DLASTADDR=XXX        - last available memory address; default 128K
# -DRUS			- bilanguage messages
# -DKEYPAD		- send "ks" to initialise arrow keys

CFLAGS  = -O -DRUS -DUSEGPLOT
LDFLAGS = -i

OBJ     = gram.o main.o decomp.o cmds.o file.o eval.o term.o graph.o\
		scr.o cap.o mem.o help.o sync.o ut.o command.o mesg.o

sc:     $(OBJ) Makefile
	${CC} ${LDFLAGS} -o sc $(OBJ) -lm

mkhelp: help.c mesg.c
	${CC} ${CFLAGS} -DMAKEHELPFILE -c help.c
	${CC} ${CFLAGS} -DMAKEHELPFILE -c mesg.c
	${CC} ${LDFLAGS} help.o mesg.o -o mkhelp

graph.o gram.o main.o decomp.o cmds.o file.o eval.o term.o cap.o mem.o help.o sync.o ut.o command.o: sc.h
graph.o command.o file.o ut.o: graph.h
term.o scr.o help.o: scr.h
command.o: state.h
scr.o cap.o: cap.h

clean:
	rm -f *.o core a.out *.b z* sc mkhelp

install: mkhelp sc
	cp sc $(INSDIR)
	strip ${INSDIR}/sc
	mkhelp >/usr/lib/sc.help

lint:   gram.c main.c decomp.c cmds.c file.c eval.c term.c scr.c\
		   cap.c mem.c help.c sync.c ut.c command.c mesg.c graph.h
	lint -p gram.c main.c decomp.c cmds.c file.c eval.c term.c scr.c\
		   cap.c mem.c help.c sync.c ut.c command.c mesg.c graph.c -lm >lint
