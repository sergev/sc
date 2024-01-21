/*
 *      SC - spreadsheet calculator, version 3.1.
 *
 *      (c) Copyright DEMOS, 1989
 *      All rights reserved.
 *
 *      Authors: Serg I. Ryshkov, Serg V. Vakulenko
 *
 *      Mon Jul 31 17:17:13 MSK 1989
 */

# include <setjmp.h>
# include "sc.h"

# ifdef __MSDOS__
#    define L 16
# else
#    define L sizeof(long)
# endif

# define C sizeof(char)
# define P sizeof(char*)

# define MEMROW (C + ((long) MAXCOLS+1) * P + L)
# define MEMCOL (2*C + (long) MAXROWS*P)

# ifndef LASTADDR
#    ifdef __MSDOS__
#	include <alloc.h>
#       define DATASIZE (coreleft () - 80 * 1024)
#    endif
#    ifdef LABTAM
#       define LASTADDR ulimit (3, 0L)
	extern long ulimit ();
#    endif
#    if defined (M_XENIX) || defined (i386)
#       define LASTADDR ulimit (3, 0L)
	extern long ulimit ();
#    endif
#    ifdef mc68
#       define LASTADDR (256*1024)
#    endif
#    ifdef pdp11
#       define LASTADDR (56*1024)
#    endif
#    ifdef VENIX
#       define LASTADDR (56*1024)
#    endif
#    if defined (vax) || defined (convex) || defined (__386BSD__) || defined (__FreeBSD__)
#       define MAXDATA (10*1024L*1024)
#       define DATASIZE ulimit ()
#       undef LASTADDR
#       include <sys/time.h>
#       include <sys/resource.h>
	long ulimit ()
	{
		struct rlimit lim;
		long maxlim;

		getrlimit (RLIMIT_DATA, &lim);
		maxlim = lim.rlim_max;
		if (maxlim > MAXDATA)
			maxlim = MAXDATA;
		return (maxlim);
	}
#    endif
# endif

long memfree, memavail;

extern jmp_buf mainloop;                /* defined in main.c */
extern State;                           /* defined in command.c */
extern Assign;                          /* defined in main.c */
extern enablejmp;                       /* is longjmp (mainloop) enabled ? */

# ifndef __MSDOS__
extern char *malloc (), *realloc ();
# endif

char *mallo (c)
unsigned c;
{
	register char *p;
	register long need;

	if (! memavail)
		initmem ();
	need = c + L;
	if (need>memfree || ! (p = malloc (c)))
		nomem ();
	memfree -= need;
	return (p);
}

static nomem ()
{
	if (enablejmp) {
		Assign = 0;
		State = 0;
		/* "Out of memory!" */
		cerror (MSG (36));
	}
	/* "sc: out of memory\n" */
	outerr (MSG (37));
	exit (1);
}

char *reallo (s, c, n)
char *s;
unsigned c, n;
{
	register char *p;
	register long need;

	if (! memavail)
		initmem ();
	need = n - c;
	if (need>memfree || ! (p = realloc (s, n)))
		nomem ();
	memfree -= need;
	return (p);
}

mfree (p, c)
char *p;
unsigned c;
{
	memfree += c + L;
	free (p);
}

static initmem ()
{
# ifdef DATASIZE
	memavail = DATASIZE;
# endif
# ifdef LASTADDR
	register char *p = 0;
	extern char *sbrk ();

	memavail = LASTADDR - (sbrk (0) - p);
# endif
	memfree = memavail;
}

chkmem (h, w)
register h, w;
{
	register long maxdelta;

	maxdelta = (long) w*h*P + h*MEMROW + w*MEMCOL;
	return (maxdelta <= memfree);
}

maxheight ()
{
	register max;
	register long maxdelta;

	maxdelta = memfree / MEMROW;
	max = MAXHEIGHT;
	if (maxdelta+MAXROWS < max)
		max = maxdelta + MAXROWS;
	return (max);
}

maxwidth ()
{
	register max;
	register long maxdelta;

	maxdelta = memfree / MEMCOL;
	max = MAXWIDTH;
	if (maxdelta+MAXCOLS < max)
		max = maxdelta + MAXCOLS;
	return (max);
}

