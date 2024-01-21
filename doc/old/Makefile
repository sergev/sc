SHELL   = /bin/sh

# MAN is one of "r" (russian) or "e" (english)

MAN     = r
MANDIR  = /usr/man/man1

SRCTBL  = tab2.me tri.me matrix.me newton.me matrix2.me diffequ2.me gaz.me
SRCME   = title.me macro.me call.me whatis.me cmd.me guide3.me guide5.me

all:    guide.lp $(MAN).lp

guide.lp: $(SRCTBL) $(SRCME)
	nroff -me $(SRCME) | col >guide.lp

guide.hp: hp.nr $(SRCTBL) $(SRCME)
	hroff -Thpsp -me hp.nr $(SRCME) | hpflt >guide.hp

hp.nr:
	echo .pl 60 >hp.nr
	echo .ll 6i >>hp.nr

install:
	cp $(MAN).man $(MANDIR)/sc.1

clean:
	rm -rf *.b z* *.lp *.hp hp.nr

.SUFFIXES: .sc .tbl .me .lp .man .hp

.me.lp:
	nroff -me $< >$@

.me.hp:
	hroff -Thpsp -me -rn1 $< | hpflt >$@

.man.lp:
	nroff -man $< >$@

.man.hp:
	hroff -Thpsp -man -rn1 $< | hpflt >$@

.tbl.me:
	tbl $< >$@

.sc.tbl:
	:mktbl $< >$@

.sc.me:
	:mktbl $< tmp.tbl
	tbl tmp.tbl >$@
	rm tmp.tbl

matrix2.tbl: matrix.sc
	:mkmat2 matrix.sc matrix2.tbl

diffequ2.tbl: diffequ.sc
	:mkdiff2 diffequ.sc diffequ2.tbl
