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

# include <signal.h>
# include <setjmp.h>
# include "ctype.h"
# include "sc.h"

extern State;                           /* defined in command.c */
extern BlackWhite;
extern AutoCalc, Border;

extern char *mallo (), *strcpy (), *edit (), *getenv (), *dtoag (), *itoa ();
extern char *strcat ();

/* default table size */

int MAXROWS = DEFROWS;
int MAXCOLS = DEFCOLS;

int cprghtflag;
char curfile [100];
int Arg, Assign, lastmove;
jmp_buf mainloop;       /* label for allocs */
int enablejmp;          /* is longjmp (mainloop) enabled ? */

struct ent ***tbl;
struct ent *to_fix;
int currow, curcol;
int FullUpdate;
int maxrow, maxcol;
int linelim;
int changed;
int modflg;
char *fwidth;
int precision = DEFPREC;
char *hiddncol;
char *hiddnrow;
char line [100];
int narg;
int nowrkflg;

maketbl ()
{
	register i, j;

	tbl = (struct ent ***) mallo (sizeof (struct ent **) * MAXROWS);
	for (i=0; i<MAXROWS; i++) {
		tbl[i] = (struct ent **) mallo (sizeof (struct ent *) * MAXCOLS);
		for (j=0; j<MAXCOLS; j++)
			tbl[i][j] = 0;
	}

	fwidth = mallo ((unsigned) MAXCOLS);
	hiddncol = mallo ((unsigned) MAXCOLS);
	hiddnrow = mallo ((unsigned) MAXROWS);

	for (i=0; i<MAXCOLS; i++) {
		fwidth[i] = DEFWIDTH;
		hiddncol[i] = 0;
	}
	for (i=0; i<MAXROWS; i++) {
		hiddnrow[i] = 0;
	}
}

init (argc, argv)
char **argv;
{
	register char *p;

	while (argv++, --argc)
		if (**argv == '-') {
			if (! argv[0][1])
				nowrkflg = 1;
			for (p = *argv+1; *p; p++) {
				switch (*p) {
				case 'b':	/* turn border flag */
					Border ^= 1;
					break;
				case 'm':	/* turn black & white flag */
					BlackWhite ^= 1;
					break;
				case 'l':	/* turn language */
# ifdef RUS
					language ();
# endif
					break;
				case 'a':	/* turn autocalc flag */
					AutoCalc ^= 1;
					break;
				case 'c':
					if (*++p > '9' || *p < '0') {
						usage ();
						return (0);
					}
					MAXCOLS = atoi (p);
					if (MAXCOLS < 1)
						MAXCOLS = 1;
					if (MAXCOLS > MAXWIDTH)
						MAXCOLS = MAXWIDTH;
					while (*p <= '9' && *p >= '0') p++;
					p--;
					break;
				case 'r':
					if (*++p > '9' || *p < '0') {
						usage ();
						return (0);
					}
					MAXROWS = atoi (p);
					if (MAXROWS < 1)
						MAXROWS = 1;
					if (MAXROWS > MAXHEIGHT)
						MAXROWS = MAXHEIGHT;
					while (*p <= '9' && *p >= '0') p++;
					p--;
					break;
				default:
					usage ();
					return (0);
				}
			}
		} else if (*curfile) {
			usage ();
			return (0);
		} else {
			strcpy (curfile, *argv);
			fileext (curfile, EXTSC);
		}
	return (1);
}

static usage ()
{
	/* "use: sc [-] [-r#] [-c#] [-a] [-b] [-l] [-m] [file]\n" */
	outerr (MSG (34));
}

outerr (s)
char *s;
{
	write (2, s, (unsigned) strlen (s));
}

quit ()
{
	endterm ();
	exit (1);
}

main (argc, argv)
char **argv;
{
# ifdef RUS
	register char *p;

	rusout = (p = getenv ("MSG")) && *p == 'r';
# endif
	/* "Just a moment... " */
	outerr (MSG (134));
	if (!init (argc, argv)) /* set up MAXROWS, MAXCOLS, curfile */
		exit (1);

# ifndef __MSDOS__
	signal (SIGHUP, quit);
	signal (SIGINT, quit);
	signal (SIGQUIT, quit);
	signal (SIGILL, quit);
	signal (SIGTRAP, quit);
	signal (SIGIOT, quit);
	signal (SIGEMT, quit);
	signal (SIGFPE, quit);
	signal (SIGBUS, quit);
	signal (SIGSEGV, quit);
	signal (SIGSYS, quit);
	signal (SIGPIPE, quit);
	signal (SIGTERM, quit);
# endif
	maketbl ();             /* malloc tbl, fwidth, hiddn's */
	initterm ();            /* initialize terminal */
	clrerr ();
	setstate ();
	readfile (curfile, 1);
	modflg = 0;
	FullUpdate = 1;
	/* if out of memory, malloc jumps here */
	/* setjump returns 1 if ^Cancel, 2 if cerror () */
	switch (setjmp (mainloop)) {
	case 2:
		FullUpdate = 1;
	case 1:
		if (Assign)
			line [linelim = 0] = 0;
	default:	/* enable longjmp (mainloop) */
		enablejmp = 1;
	}
	Arg = 1;
	narg = 0;
	cprghtflag = 1;
	for (;;) {
		command (cgetch ());
		Arg = (narg>0 && narg <= MAXHEIGHT) ? narg : 1;
	}
}

cgetch ()
{
	register c;
	register struct ent *p;

	if (AutoCalc && (changed || FullUpdate))
		EvalAll (), changed = 0;
	else
		maketable ();           /* print table window */
	FullUpdate = 0;
	info ();                        /* print global table info */
	addhelp ();                     /* print help */
	for (;;) {
		addprm ();              /* print command line */
		c = argetch (0);
		switch (c) {
		case ctl ('g'):
			view ();
			goto restorescreen;
		case ctl ('p'):
			help ();
restorescreen:
			if (! Assign)
				setstate ();
			maketable ();
			info ();
			addhelp ();
			clrerr ();
			continue;
		}
		if (Assign)
			switch (c) {
			default:
				if (isprint (c)) {
					line [linelim++] = c;
					line [linelim] = 0;
					break;
				}
				VBeep ();
				break;
			case ctl ('h'):
				if (linelim > 0)
					--linelim;
				line [linelim] = 0;
				break;
			case ctl ('j'):
				if (linelim <= 0) {
					tabmove (lastmove);
					maketable ();
				} else {
					parse (lookat (currow, curcol), line);
					tabmove (lastmove);
					line [linelim = 0] = 0;
					if (AutoCalc)
						EvalAll (), changed = 0;
					else
						maketable ();	/* print table window */
					FullUpdate = 0;
					info ();	/* print global table info */
				}
				break;
			case ctl ('i'):
				if (linelim <= 0) {
					Assign = 0;
					setstate ();
					Arg = 1;
					narg = 0;
					addhelp ();
					clrerr ();
				}
				break;
			case ctl ('a'):   /* insert variable name */
				strcpy (line+linelim, coltoa (curcol));
				strcat (line+linelim, itoa (currow));
				linelim = strlen (line);
				break;
			case ctl ('f'):   /* insert variable formula */
				if (! (p = tbl[currow][curcol]) || ! (p->flags & is_value) || ! p->p.expr)
					break;
				if (! linelim) {
					line [linelim++] = '=';
					if (RIGHTALIGN (p->flags))
						line [linelim++] = '>';
					else if (LEFTALIGN (p->flags))
						line [linelim++] = '<';
					else if (CENTER (p->flags))
						line [linelim++] = '^';
				}
				editexp (currow, curcol);
				break;
			case ctl ('v'):   /* insert variable value */
				if (! (p = tbl[currow][curcol]) || ! (p->flags & is_value))
					break;
				if (! linelim) {
					line [linelim++] = '=';
					if (RIGHTALIGN (p->flags))
						line [linelim++] = '>';
					else if (LEFTALIGN (p->flags))
						line [linelim++] = '<';
					else if (CENTER (p->flags))
						line [linelim++] = '^';
				}
				strcpy (line+linelim, dtoag (p->d.value, 15));
				linelim = strlen (line);
				break;
			case ctl ('t'):   /* insert variable text */
				p = tbl[currow][curcol];
				if (p && !(p->flags & is_value)) {
					if (! linelim)
						if (DFLTALIGN (p->flags))
							line [linelim++] = '"';
						else if (RIGHTALIGN (p->flags))
							line [linelim++] = '>';
						else if (LEFTALIGN (p->flags))
							line [linelim++] = '<';
						else if (CENTER (p->flags))
							line [linelim++] = '^';
					strcpy (line+linelim, p->p.label);
					linelim = strlen (line);
				}
				break;
			case ctl ('e'):   /* edit */
				linelim = edit (line, line+linelim) - line;
				break;
			}
		else
			switch (c) {
			default:
				if (isdigit (c)) {
					c -= '0';
					if ((long) narg*10 + c <= MAXHEIGHT)
						narg = narg*10 + c;
					Arg = (narg>0 && narg <= MAXHEIGHT) ? narg : 1;
				} else if (isprint (c)) {
					if (isupper (c))
						c = tolower (c);
					return (c);
				} else
					VBeep ();
				break;
			case ctl ('h'):
				narg = Arg / 10;
				Arg = narg>0 ? narg : 1;
				addprm ();
				break;
			case ctl ('j'):
				backstate ();
				addhelp ();
				break;
			case ctl ('i'):
				Assign = 1;
				Arg = 1;
				narg = 0;
				linelim = 0;
				addhelp ();
				clrerr ();
				break;
			}
	}
}
