HFLAGS  = -Thp

all:    insv.lp insu.lp inssm.lp ins85.lp insx2.lp insx3.lp reply.lp licence.lp

clean:
	rm -f *.hp *.b z*

address.hp: address.me
	hroff -Thpjr12 address.me | hpflt >address.hp

.me.hp:
	hroff -me $(HFLAGS) $? | hpflt >$@

.me.lp:
	hroff -me $? >$@

.man.hp:
	hroff -man $(HFLAGS) hp.nr $? | hpflt >$@

.man.lp:
	hroff -man $? >$@

.SUFFIXES: .me .hp .lp .man
