#
#	Makefile for SC under MS-DOS with Turbo-C 2.0 compiler.
#
#	Use "make -fsc.mak".
#

# -DNOHELP		- "no help available"
# -DHELPFILE=\"...\"	- name of help file; default in core
# -DSTDHELPFILE		- name of help file is /usr/lib/sc.help
# -DLASTADDR=XXX	- last available memory address; default 128K
# -DRUS			- bilanguage messages
# -DKEYPAD		- send "ks" to initialise arrow keys

MODEL	= l
CFLAGS  = -O -m$(MODEL) -w-pia -DRUS
LDFLAGS	= -m$(MODEL)
LD	= tcc
CC	= tcc
LIB	= d:\tc\lib

OBJ     = gram.obj main.obj decomp.obj cmds.obj file.obj eval.obj term.obj graph.obj scr.obj cap.obj mem.obj help.obj sync.obj ut.obj command.obj mesg.obj gamma.obj draw.obj tplot.obj pplot.obj eps.obj cga.obj egavga.obj herc.obj drawpie.obj drawxy.obj drawut.obj cvt.obj key.obj

sc.exe: $(OBJ)
	$(LD) $(LDFLAGS) -esc *.obj $(LIB)\graphics.lib

cga.obj: cga.bgi
	bgiobj cga

egavga.obj: egavga.bgi
	bgiobj egavga

herc.obj: herc.bgi
	bgiobj herc

graph.obj: sc.h graph.h

gram.obj: sc.h

main.obj: sc.h

decomp.obj: sc.h

cmds.obj: sc.h

file.obj: sc.h graph.h

eval.obj: sc.h

term.obj: sc.h scr.h

cap.obj: sc.h cap.h

mem.obj: sc.h

help.obj: sc.h scr.h

sync.obj: sc.h

ut.obj: sc.h graph.h

command.obj: sc.h graph.h state.h

scr.obj: scr.h cap.h

draw.obj drawut.obj drawpie.obj drawxy.obj: draw.h dglob.h

tplot.c: tpmsdos.c tp85.c

.c.obj:
	tcc -c $(CFLAGS) $<
