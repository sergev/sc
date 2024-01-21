CFLAGS  = -O -w-pia -DRUS -DMAKESUM

makesum.exe: mesg.c
	tcc $(CFLAGS) -emakesum mesg.c
	del mesg.obj