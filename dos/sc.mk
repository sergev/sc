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
#       Makefile for SC under UNIX/XENIX/UTIX/DEMOS/VENIX.
#
#       Use "make -f sc.mk".
#

SHELL   = /bin/sh
INSDIR  = /usr/bin
INSLIB  = /usr/lib
LIBS    = -lm

# -DMESGFILE=\"$(INSLIB)/\"     - keep messages in files, directory is "XXX"
# -DNOHELP                      - "no help available"
# -DHELPFILE=\"$(INSLIB)/sc.help\"      - name of help file; default in core
# -DSTDHELPFILE                 - name of help file is /usr/lib/sc.help
# -DLASTADDR=XXX                - last available memory address; default 128K
# -DRUS                         - bilanguage messages
# -DKEYPAD                      - send "ks" to initialise arrow keys
# -DTERMIO                      - use termio.h instead of sgtty.h

# For labtab NS 32332 under Unix System V.3
# CFLAGS  = -DLABTAM -DTERMIO
# LDFLAGS = -i

# For COMPAQ 386 under XENIX-386
# CFLAGS  = -O -DRUS -DTERMIO
# LDFLAGS =
# LIBS    = -lm -lccgi

# For IBM PC AT under XENIX-286
# CFLAGS  = -Ml -O -DTERMIO
# LDFLAGS = -Ml -F 2000
# LIBS    = -lm -lccgi

# For Aniral Utec under UTIX
# CFLAGS  = -O -DRUS
# LDFLAGS =

# For El-85 under DEMOS
# CFLAGS  = -O -DRUS -DHELPFILE=\"$(INSLIB)/sc.help\" -DE85DEMOS -DMESGFILE=\"$(INSLIB)/\"
# LDFLAGS = -Q

# For SM-4 under DEMOS etc
# CFLAGS  = -O -DRUS -DHELPFILE=\"$(INSLIB)/sc.help\" -DMESGFILE=\"$(INSLIB)/\"
# LDFLAGS = -Q

# For IBM PC XT under VENIX
# CFLAGS  = -z -DRUS -DVENIX -DHELPFILE=\"$(INSLIB)/sc.help\" -DMESGFILE=\"$(INSLIB)/\"
# LDFLAGS = -z -i

# For VAX-11 under DEMOS-32 etc
# CFLAGS  = -O -DRUS -DKEYPAD
# LDFLAGS = -n

HDRS	      = cap.h\
		dglob.h\
		draw.h\
		graph.h\
		key.h\
		sc.h\
		scr.h\
		state.h

LINKER	      = cc

MAKEFILE      = Makefile

OBJS	      = cap.o\
		cmds.o\
		command.o\
		cvt.o\
		decomp.o\
		draw.o\
		drawpie.o\
		drawut.o\
		drawxy.o\
		eps.o\
		eval.o\
		file.o\
		gamma.o\
		gram.o\
		graph.o\
		help.o\
		key.o\
		main.o\
		mem.o\
		mesg.o\
		pplot.o\
		scr.o\
		sync.o\
		term.o\
		tphp.o\
		tplot.o\
		tty.o\
		ut.o

PRINT	      = pr

PROGRAM	      = sc

SRCS	      = cap.c\
		cmds.c\
		command.c\
		cvt.c\
		decomp.c\
		draw.c\
		drawpie.c\
		drawut.c\
		drawxy.c\
		eps.c\
		eval.c\
		file.c\
		gamma.c\
		gram.c\
		graph.c\
		help.c\
		key.c\
		main.c\
		mem.c\
		mesg.c\
		pplot.c\
		scr.c\
		sync.c\
		term.c\
		tp85.c\
		tpcgi.c\
		tphp.c\
		tplot.c\
		tpmsdos.c\
		tty.c\
		ut.c

all:		$(PROGRAM)

$(PROGRAM):     $(OBJS)
		$(LINKER) $(LDFLAGS) $(OBJS) $(LIBS) -o $(PROGRAM)

clean:;         rm -f *.o core a.out *.b z* $(PROGRAM) mkhelp sc.?msg sc.help mk?msg mk????.c

depend:;        mkmf -f $(MAKEFILE) PROGRAM=$(PROGRAM) DEST=$(DEST)

index:;         ctags -wx $(HDRS) $(SRCS)

print:;         $(PRINT) $(HDRS) $(SRCS)

program:        $(PROGRAM)

tags:           $(HDRS) $(SRCS); ctags $(HDRS) $(SRCS)

lint:           $(SRCS); lint -p $(SRC) -lm >lint

update:		$(DEST)/$(PROGRAM)

mkhelp.c:       help.c mesg.c
		cat help.c mesg.c >mkhelp.c

mkhelp:         mkhelp.c
		${CC} $(LDFLAGS) ${CFLAGS} -DMAKEHELPFILE mkhelp.c -o mkhelp

mkrmsg.c:       mesg.c
		cp mesg.c mkrmsg.c

mkrmsg:         mkrmsg.c
		${CC} $(LDFLAGS) ${CFLAGS} -DMAKERMESG mkrmsg.c -o mkrmsg

mklmsg.c:       mesg.c
		cp mesg.c mklmsg.c

mklmsg:         mklmsg.c
		${CC} $(LDFLAGS) ${CFLAGS} -DMAKELMESG mklmsg.c -o mklmsg

install:	$(PROGRAM)
		install -s $(PROGRAM) $(DEST)

install:        $(PROGRAM) mkhelp mkrmsg mklmsg
		cp $(PROGRAM) $(INSDIR)
		strip ${INSDIR}/$(PROGRAM)
		mkhelp >$(INSLIB)/sc.help
		mklmsg >$(INSLIB)/sc.lmsg
		mkrmsg >$(INSLIB)/sc.rmsg

refresh:
		cp $(PROGRAM) $(INSDIR)
		strip ${INSDIR}/$(PROGRAM)
		mkhelp >$(INSLIB)/sc.help
		mklmsg >$(INSLIB)/sc.lmsg
		mkrmsg >$(INSLIB)/sc.rmsg

$(DEST)/$(PROGRAM): $(SRCS) $(HDRS) $(EXTHDRS)
		make -f $(MAKEFILE) DEST=$(DEST) install
###
cap.o: cap.h
cmds.o: sc.h
command.o: sc.h state.h graph.h
decomp.o: sc.h
draw.o: draw.h dglob.h
drawpie.o: draw.h dglob.h
drawut.o: draw.h dglob.h
drawxy.o: draw.h dglob.h
eval.o: sc.h
file.o: sc.h graph.h
gram.o: sc.h
graph.o: sc.h graph.h draw.h
help.o: sc.h scr.h
key.o: key.h cap.h
main.o: sc.h
mem.o: sc.h
scr.o: scr.h key.h cap.h
sync.o: sc.h
term.o: sc.h scr.h
tplot.o: tpmsdos.c tp85.c tpcgi.c
ut.o: sc.h graph.h
