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

LD	= tcc
CC	= tcc
LIB	= d:\tc\lib
SC	= ..
MODEL	= l
CFLAGS  = -O -m$(MODEL) -w-pia -DRUS -DDEMO -I$(SC)
LDFLAGS	= -m$(MODEL)

OBJ     = gram.obj main.obj decomp.obj cmds.obj file.obj eval.obj term.obj graph.obj scr.obj cap.obj mem.obj help.obj sync.obj ut.obj command.obj mesg.obj gamma.obj draw.obj tplot.obj pplot.obj eps.obj\
		$(SC)\cga.obj $(SC)\egavga.obj $(SC)\herc.obj drawpie.obj drawxy.obj drawut.obj cvt.obj

scdemo.exe: $(OBJ)
	$(LD) $(LDFLAGS) -escdemo *.obj $(SC)\cga.obj $(SC)\egavga.obj $(SC)\herc.obj $(LIB)\graphics.lib

$(SC)\cga.obj: $(SC)\cga.bgi
	bgiobj $(SC)\cga

$(SC)\egavga.obj: $(SC)\egavga.bgi
	bgiobj $(SC)\egavga

$(SC)\herc.obj: $(SC)\herc.bgi
	bgiobj $(SC)\herc

graph.obj: $(SC)\graph.c $(SC)\sc.h $(SC)\graph.h
	tcc -c $(CFLAGS) -ograph $(SC)\graph.c

gram.obj: $(SC)\gram.c $(SC)\sc.h
	tcc -c $(CFLAGS) -ogram $(SC)\gram.c

main.obj: $(SC)\main.c $(SC)\sc.h
	tcc -c $(CFLAGS) -omain $(SC)\main.c

decomp.obj: $(SC)\decomp.c $(SC)\sc.h
	tcc -c $(CFLAGS) -odecomp $(SC)\decomp.c

cmds.obj: $(SC)\cmds.c $(SC)\sc.h
	tcc -c $(CFLAGS) -ocmds $(SC)\cmds.c

file.obj: $(SC)\file.c $(SC)\sc.h $(SC)\graph.h
	tcc -c $(CFLAGS) -ofile $(SC)\file.c

eval.obj: $(SC)\eval.c $(SC)\sc.h
	tcc -c $(CFLAGS) -oeval $(SC)\eval.c

term.obj: $(SC)\term.c $(SC)\sc.h $(SC)\scr.h
	tcc -c $(CFLAGS) -oterm $(SC)\term.c

cap.obj: $(SC)\cap.c $(SC)\sc.h $(SC)\cap.h
	tcc -c $(CFLAGS) -ocap $(SC)\cap.c

mem.obj: $(SC)\mem.c $(SC)\sc.h
	tcc -c $(CFLAGS) -omem $(SC)\mem.c

help.obj: $(SC)\help.c $(SC)\sc.h $(SC)\scr.h
	tcc -c $(CFLAGS) -ohelp $(SC)\help.c

sync.obj: $(SC)\sync.c $(SC)\sc.h
	tcc -c $(CFLAGS) -osync $(SC)\sync.c

ut.obj: $(SC)\ut.c $(SC)\sc.h $(SC)\graph.h
	tcc -c $(CFLAGS) -out $(SC)\ut.c

command.obj: $(SC)\command.c $(SC)\sc.h $(SC)\graph.h $(SC)\state.h
	tcc -c $(CFLAGS) -ocommand $(SC)\command.c

scr.obj: $(SC)\scr.c $(SC)\scr.h $(SC)\cap.h
	tcc -c $(CFLAGS) -oscr $(SC)\scr.c

draw.obj: $(SC)\draw.c $(SC)\draw.h $(SC)\dglob.h
	tcc -c $(CFLAGS) -odraw $(SC)\draw.c

drawpie.obj: $(SC)\drawpie.c $(SC)\draw.h $(SC)\dglob.h
	tcc -c $(CFLAGS) -odrawpie $(SC)\drawpie.c

drawxy.obj: $(SC)\drawxy.c $(SC)\draw.h $(SC)\dglob.h
	tcc -c $(CFLAGS) -odrawxy $(SC)\drawxy.c

drawut.obj: $(SC)\drawut.c $(SC)\draw.h $(SC)\dglob.h
	tcc -c $(CFLAGS) -odrawut $(SC)\drawut.c

mesg.obj: $(SC)\mesg.c
	tcc -c $(CFLAGS) -omesg $(SC)\mesg.c

gamma.obj: $(SC)\gamma.c
	tcc -c $(CFLAGS) -ogamma $(SC)\gamma.c

tplot.obj: $(SC)\tplot.c $(SC)\tpmsdos.c $(SC)\tp85.c
	tcc -c $(CFLAGS) -otplot $(SC)\tplot.c

pplot.obj: $(SC)\pplot.c
	tcc -c $(CFLAGS) -opplot $(SC)\pplot.c

eps.obj: $(SC)\eps.c
	tcc -c $(CFLAGS) -oeps $(SC)\eps.c
