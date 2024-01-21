HFLAGS  = -Thp
NROFF   = nroff

all:    ref.lp sc.lp

clean:
	rm -f *.hp *.b z* *.lp .,*

.me.hp:
	$(NROFF) -me $(HFLAGS) $< | hpflt > $@

.me.lp:
	$(NROFF) -me $< > $@

.tbl.lp:
	tbl $< | $(NROFF) -me > $@

.man.hp:
	$(NROFF) -man $(HFLAGS) hp.nr $< | hpflt > $@

.man.lp:
	$(NROFF) -man $< > $@

.SUFFIXES: .me .hp .lp .man .tbl
