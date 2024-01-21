# Makefile $Revision: 6.16 $
#
# 1) Select the proper EXDIR (path), MANDIR, MANEXT, LIBDIR, SIGVOID,
#	RE_COMP/REGCMP, and DFLT_PAGER. Most of the other things aren't
#	normally changed (see the comments with each)
# 2) Select the proper machine/compiler/OS section of code
# 3) make install
# 4) If you have the command 'file' that uses /etc/magic add the line:
#	38	string		Spreadsheet	sc file


# Specify the name of the program.
# All documentation and installation keys on this value.
# 
name=sc
NAME=SC

# This is where the install step puts it.
#EXDIR=/site/bin
EXDIR=/usr/local/bin

# This is where the man page goes.
#MANDIR=/usr/local/src/man/man1	# reno
#MANEXT=1			# reno
MANDIR=/usr/local/man/man1
MANEXT=1

# This is where the library file (tutorial) goes.
#LIBDIR=/usr/local/share/$(name) # reno
LIBDIR=/usr/local/lib/$(name)

# Set SIMPLE for lex.c if you don't want arrow keys or lex.c blows up
SIMPLE=
#SIMPLE=-DSIMPLE

# Set BROKENCURSES if your curses has the nl/nonl bug
# if it does and you don't set BROKENCURSES, the display will
# be staggered across the screen. Also try IDLOKBAD below.
BROKENCURSES=
#BROKENCURSES=-DBROKENCURSES

# Set DOBACKUPS if you would like a backup copy of a source file on a save
#DOBACKUPS=
DOBACKUPS=-DDOBACKUPS

# Set INTERNATIONAL if you need 8 bit characters.  You should
# not set this if you are running 5.3.0.  I think it is OK in 5.3.1.
INTERNATIONAL=-DINTERNATIONAL
#INTERNATIONAL=

# Set SIGVOID if signal routines are type void.
# use: SIGVOID=-DSIGVOID for:
#	System 5.3, SunOS 4.X, VMS, BSD4.4 (reno), and ANSI C Compliant systems
# use: SIGVOID=		for:
#  BSD systems (excluding reno, BSD4.4), and the UNIXPC 'cc'
SIGVOID=-DSIGVOID
#SIGVOID=

# Set IEEE_MATH if you need setsticky() calls in your signal handlers
#
#IEEE_MATH=-DIEEE_MATH
IEEE_MATH=

# Set RINT=-DRINT if you do not have rint() in math.h
# Set RINT=	on/with (they have rint):
#	SunOS 4.0.3c compiler
#	BSD4.4 (reno)
#RINT=
RINT=-DRINT

# Set RE_COMP if you have the re_comp/re_exec regular expression routines
# (most BSD based systems do).
#RE_COMP=-DRE_COMP
RE_COMP=

# Set REGCMP if you have the regcmp/regex regular expression routines
# (most System V based systems do)
#REGCMP=-DREGCMP
REGCMP=

# This is the name of a pager like "more".
# "pg" may be appropriate for SYSV.
DFLT_PAGER=-DDFLT_PAGER=\"less\"
#DFLT_PAGER=-DDFLT_PAGER=\"more\"	# generic && reno

# this is the name to save back ups in
SAVE=-DSAVENAME=\"$(NAME).b\"

# path to crypt
#CRYPT=-DCRYPT_PATH=\"/bin/crypt\"
CRYPT=-DCRYPT_PATH=\"/usr/local/bin/crypt\"

# flags for lint
LINTFLAGS=-abchxv

# *** SPECIAL NOTES ***
# For ULTRIX: define the BSD4.2 section and SIGVOID above
#	tdw@cl.cam.ac.uk tested on Ultrix 3.1C-0
# HP-UX 7.0: Do NOT use -O
#	(known broken, try sc's boolean operators if you wish)
#
# **** SYSV curses bugs... ****
# Try setting IDLOKBAD to fix (with an empty spreadsheet):
#	a) Redrawing the bottom half of the screen when you
#	 	move between row 9 <-> 10
#	b) the highlighted row labels being trash when you
#		move between row 9 <-> 10
#	c) On an xterm on Esix Rev. D+ from eating lines
#		 -goto (or move) a few lines (or more) past the bottom
#		 of the screen, goto (or move) to the top line on the
#		 screen, move upward and the current line is deleted, the
#		 others move up even when they should not, check by
#		 noticing the rows become 2, 3, 40, 41, 42... (etc).
#	Known systems/terminfos w/ curses problems:
#	{Esix Rev. D+, AT&T SysV3.2.1}:at386-m,xterm, HP-UX7.0:(not sure)
IDLOKBAD=-DIDLOKBAD
#IDLOKBAD=

# IF you have problems w/ your yacc and don't have, say, bison...
# look just below for '### YACC PROBLEMS ###' and reverse the define to
# use the included, pre-yacc'd copy. First try below.
YACC=yacc
#YACC=bison -y

#### SYSTEM DEFINES ####

# Use this for system AIX V3.1
#CFLAGS= -O -DSYSV2 -DCHTYPE=int -DNLS
#LDFLAGS=
#LIB=-lm -lPW -lcurses

# Use this for system V.2
#CFLAGS= -O -DSYSV2 
#LDFLAGS=
#LIB=-lm -lPW -lcurses
# with gcc on a Sequent also use:
#CC=att gcc
#CFLAGS=  -DSYSV2 -g -pipe -traditional

# Use this for system V.3
#CFLAGS=  -DSYSV3 -O
LDFLAGS= -s
LIB=-lm -lcurses #-lPW
# with gcc also use:
CC=gcc
CFLAGS=  -DSYSV3 -O #-pipe
#YACC=bison -y

# Use this for system V.4
#CFLAGS=  -DSYSV4 -DSYSV3 -O
#LDFLAGS= -s
#LIB=-lm -lcurses -lgen
# with gcc also use:
#CC=gcc
#CFLAGS=  -DSYSV3 -O -pipe

# Microport
#CFLAGS= -DSYSV2 -O -DUPORT -Ml
#LDFLAGS=-Ml
#LIB=-lm -lcurses -lPW

# Use this for BSD 4.2
#CFLAGS= -O -DBSD42
#LDFLAGS=
#LIB=-lm -lcurses -ltermcap

# Use this for Sequent boxes
#CC=atscc
#CFLAGS=-O -DBSD42
#LDFLAGS= 
#LIB=-lm -lcurses  -ltermcap
#PSCLIB=-lseq
# with gcc also use:
#CC=gcc
#CFLAGS= -O -DBSD42 -pipe

# Use this for BSD 4.3
#CFLAGS= -O -DBSD43
#LDFLAGS=
#LIB=-lm -lcurses -ltermcap

# Use this for SunOS 4.X if you have the System V package installed.
# This will link with the System V curses which is preferable to the
# BSD curses (especially helps scrolling on slow (9600bps or less)
# serial lines).
#
# Be sure to define SIGVOID and RE_COMP above.
# 
#CC=/usr/5bin/cc
#CFLAGS= -O -DSYSV3 
#LDFLAGS=
#LIB=-lm -lcurses 

# Use this for system III (XENIX)
#CFLAGS= -O -DSYSIII
#LDFLAGS= -i
#LIB=-lm -lcurses -ltermcap

# Use this for XENIX Version 2.3
#CFLAGS= -O -SSYSIII -DXENIX2_3
#LDFLAGS= -i
#LIB=-lm -lcurses -ltermcap

# Use this for VENIX
#CFLAGS= -DVENIX -DBSD42 -DV7
#LDFLAGS= -z -i 
#LIB=-lm -lcurses -ltermcap

# For SCO Unix V rel. 3.2.0
#       -compile using rcc, cc does not cope with gram.c
#       -edit /usr/include/curses.h, rcc does not understand #error
#       -link: make CC=cc, rcc's loader gets unresolved __cclass, __range
#               (rather strange,?)
#CC=rcc
#CC=cc
#CC=gcc -fstrength-reduce
#SIGVOID=-DSIGVOID
#CFLAGS= -O -DSYSV3
#LDFLAGS=
#LIB=-lm -lcurses -ltinfo -lPW
#YACC=yacc -Sm10000

# Use this for SCO Unix 3.2.2 and ODT 1.1
#CC=cc
#CFLAGS= -O -DSYSV3
#LDFLAGS=
#LIB=-lm -lcurses -lPW -lmalloc -lc_s
#YACC=yacc -Sm10000

# All of the source files
SRC=Makefile cmds.c crypt.c eres.sed format.c gram.y help.c interp.c \
	lex.c psc.c range.c sc.c sc.h screen.c sres.sed version.c \
	vi.c vmtbl.c xmalloc.c

# The objects
OBJS=cmds.o crypt.o format.o gram.o help.o interp.o lex.o range.o sc.o \
	screen.o version.o vi.o vmtbl.o xmalloc.o

# The documents in the Archive
DOCS=README CHANGES sc.doc psc.doc tutorial.sc VMS_NOTES

all:	$(name) p$(name) $(name)qref

$(name):$(PAR) 	$(OBJS)
	$(CC) ${LDFLAGS} ${OBJS} ${LIB} -o $(name)

### YACC PROBLEMS ###
# IF you have problems w/ your yacc and don't have, say, bison...
# for systems that don't allow you to increase the number of terminals
# (mostly AT&T) (this will include a Berkeley yacc built gram.c and
# y.tab.h) use:
#gram.c:	mygram.c
#	cp mygram.c gram.c
#y.tab.h:	
#	cp myy.tab.h y.tab.h 
#
# Otherwise (most systems)
gram.c:	gram.y
	$(YACC) -d gram.y; mv y.tab.c gram.c
y.tab.h:	gram.y


p$(name):	psc.c pvmtbl.o pxmalloc.o
	$(CC) ${LDFLAGS} -o p$(name) psc.c pvmtbl.o pxmalloc.o ${PSCLIB}

qhelp.c: help.c
	-rm -f qhelp.c
	ln help.c qhelp.c

$(name)qref:	qhelp.c
	$(CC) $(LDFLAGS) -DQREF -DSCNAME=\"$(name)\" -o $(name)qref qhelp.c

pvmtbl.c: vmtbl.c
	-rm -f pvmtbl.c
	ln vmtbl.c pvmtbl.c

pvmtbl.o: sc.h pvmtbl.c
	$(CC) ${CFLAGS} -c -DPSC pvmtbl.c

pxmalloc.c: xmalloc.c
	-rm -f pxmalloc.c
	ln xmalloc.c pxmalloc.c

pxmalloc.o: sc.h pxmalloc.c
	$(CC) ${CFLAGS} -c -DPSC pxmalloc.c

lex.o:	sc.h y.tab.h gram.o lex.c
	$(CC) ${CFLAGS} ${SIMPLE} ${IEEE_MATH} ${SIGVOID} -c lex.c

sc.o:	sc.h sc.c
	$(CC) ${CFLAGS} ${DFLT_PAGER} ${SIGVOID} ${SAVE} -c sc.c

screen.o:	sc.h screen.c
	$(CC) ${CFLAGS} ${BROKENCURSES} ${INTERNATIONAL} ${SIGVOID} -c screen.c

interp.o:	interp.c sc.h
	$(CC) ${CFLAGS} ${IEEE_MATH} ${SIGVOID} ${RINT} ${RE_COMP} ${REGCMP} -c interp.c

gram.o:	sc.h y.tab.h

cmds.o: cmds.c sc.h
	$(CC) ${CFLAGS} ${DOBACKUPS} -c cmds.c

crypt.o: crypt.c sc.h
	$(CC) ${CFLAGS} ${CRYPT} ${DOBACKUPS} -c crypt.c

range.o: range.c sc.h

help.o: help.c sc.h

format.o: format.c

vi.o: vi.c sc.h

gram.o:	sc.h y.tab.h gram.c
	$(CC) ${CFLAGS} -c gram.c
	sed<gram.y >experres.h -f eres.sed;sed < gram.y > statres.h -f sres.sed

clean:
	rm -f *.o *res.h y.tab.h $(name) p$(name) debug core gram.c $(name).1 \
	$(name).man p$(name).man p$(name).1 y.output $(name)qref

shar: ${SRC} ${DOCS}
	shar -c -m 64000 -f shar ${DOCS} ${SRC}

sshar: ${SRC}
	shar -c -m 1000000 -f shar ${SRC}

lint: sc.h sc.c lex.c gram.c interp.c cmds.c crypt.c range.c help.c \
 	vi.c version.c xmalloc.c format.c vmtbl.c
	lint ${LINTFLAGS} ${CFLAGS} ${SIMPLE} sc.c lex.c gram.c interp.c \
 	cmds.c crypt.c range.c help.c vi.c version.c xmalloc.c format.c \
 	vmtbl.c -lcurses -lm 
	make lintqref

lintqref: help.c
	lint ${LINTFLAGS} ${CFLAGS} ${SIMPLE} -DQREF help.c

lintpsc: psc.c vmtbl.c
	lint ${LINTFLAGS} ${CFLAGS} ${SIMPLE} -DPSC psc.c vmtbl.c

inspect: sc.h sc.c lex.c gram.c interp.c cmds.c crypt.c
	/bruces/ianj/bin/i386/inspect -abv -t 8 sc.c lex.c gram.c interp.c \
	cmds.c crypt.c range.c xmalloc.c help.c vi.c

print: sc.h gram.y sc.c lex.c interp.c cmds.c crypt.c 
	prc sc.h gram.y sc.c lex.c interp.c cmds.c crypt.c | lpr

$(name).1:	sc.doc
	sed -e s/pname/$(name)/g -e s/PNAME/$(NAME)/g \
	   -e s%#LIBDIR#%$(LIBDIR)%g sc.doc >  $(name).1

$(name).man:	$(name).1
	nroff -man $(name).1 > $(name).man

laser:	$(name).1
	itpf -x -Pim2 -man $(name).1

p$(name).1:	psc.doc
	sed -e s/pname/$(name)/g -e s/PNAME/$(NAME)/g psc.doc >  p$(name).1

p$(name).man:	p$(name).1
	nroff -man p$(name).1 > p$(name).man

install: $(EXDIR)/$(name) $(EXDIR)/$(name)qref $(EXDIR)/p$(name) \
	 $(LIBDIR)/tutorial $(MANDIR)/$(name).$(MANEXT)

$(EXDIR)/$(name): $(name)
	cp $(name) $(EXDIR)
	strip $(EXDIR)/$(name)

$(EXDIR)/$(name)qref: $(name)qref
	cp $(name)qref $(EXDIR)
	strip $(EXDIR)/$(name)qref

$(EXDIR)/p$(name): p$(name)
	cp p$(name) $(EXDIR)
	strip $(EXDIR)/p$(name)

$(LIBDIR)/tutorial: tutorial.sc $(LIBDIR)
	cp tutorial.sc $(LIBDIR)/tutorial.$(name)

$(LIBDIR):
	mkdir $(LIBDIR)

$(MANDIR)/$(name).$(MANEXT): $(name).1
	cp $(name).1 $(MANDIR)/$(name).$(MANEXT)

diffs: ${SRC}
	for i in ${SRC} ${DOCS} ;\
		do \
		rcsdiff -c -r6.11 $$i ;\
		done

# THA 10/14/90  Added code to make a patchfile
patchfile: ${SRC} ${DOCS}
	rm -f patchfile
	for i in ${SRC} ${DOCS} ;\
		do \
		diffc $$i /users/toma/sc/sc6.12/$$i >> patchfile ;\
		done
