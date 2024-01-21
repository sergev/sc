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
# include "ctype.h"
# include "sc.h"
# include "scr.h"
# include "key.h"

# ifdef __MSDOS__
# define VERTLINE	179
# else
# define VERTLINE       '|'
# endif /* MSDOS */

# define COPYRIGHT MSG (146)

int strow, stcol;
static rows, cols;
static UpdateLine [50];                 /* assume COLS < 50 */
static IsStndout;                       /* is there standout on terminal ? */
static errflg;                          /* is there error message */
static BOX *curbox;

extern jmp_buf mainloop;                /* defined in main.c */
extern Arg, narg;			/* defined in main.c */
extern Assign;                          /* defined in main.c */
extern lastmove;                        /* defined in main.c */
extern long memfree, memavail;          /* defined in mem.c */
extern char *dline;                     /* defined in decomp.c */
extern int dlim;                        /* defined in decomp.c */
extern savedrow, savedcol;              /* defined in command.c */
extern getrow, getcol, getnum;          /* defined in command.c */
extern AutoCalc, Border;
extern cprghtflag;

extern char *helpstr (), *dtoag (), *itoaw (), *itoa ();
extern char *doprint (), *strcpy ();

static struct KeyMap keymap [] = {
	"f1",           0,              cntrl ('p'),
	"f2",           0,              cntrl ('a'),
	"f3",           0,              cntrl ('t'),
	"f4",           0,              cntrl ('f'),
	"f5",           0,              cntrl ('v'),
	"f6",           0,              cntrl ('c'),    /* not implemented yet */
	"f7",           0,              cntrl ('c'),    /* not implemented yet */
	"f8",           0,              cntrl ('c'),    /* not implemented yet */
	"f9",           0,              cntrl ('c'),    /* not implemented yet */
	"f0",           0,              cntrl ('g'),
	"kl",           0,              cntrl ('l'),
	"kr",           0,              cntrl ('r'),
	"ku",           0,              cntrl ('u'),
	"kd",           0,              cntrl ('d'),
	"kh",           0,              cntrl ('y'),
	"k1",           0,              cntrl ('z'),
	"kH",           0,              cntrl ('z'),
	"kN",           0,              cntrl ('b'),
	"kP",           0,              cntrl ('k'),
	"k0",           0,              cntrl ('u'),
	"k.",           0,              cntrl ('d'),
	0,              "\177",         cntrl ('h'),
	0,              "\015",         cntrl ('j'),
	0,              0,              0,
};

copyright ()
{
	register char *p;
	register sum;

	for (sum=0, p=COPYRIGHT; *p; ++p)
		sum += *p & 0377;
	if (sum != 5664 && sum != 6876 && sum != 8363)
		exit (0);
	VPutString (COPYRIGHT);
}

clrerr ()
{
	VMove (ERRROW, 0);
	if (Border) {
		VSetDim ();
		if (Assign)
			/* "n^Ame ^Formula ^Value ^Text ^Edit ^Left ^Right ^Up ^Down ^Xclear ^Cancel" */
			VPutString (MSG (38));
		else
			/* "^Left ^Right ^Up ^Down ^Xclear ^Cancel" */
			VPutString (MSG (39));
		VSetNormal ();
	}
	VClearLine ();
	errflg = 0;
}

/* VARARGS1 */

printw (fmt, a)
char *fmt;
{
	char buf [512];

	doprint (buf, fmt, &a);
	VPutString (buf);
}

/* VARARGS1 */

dowprint (fmt, a)
char *fmt;
int *a;
{
	char buf [512];

	doprint (buf, fmt, a);
	VPutString (buf);
}

/* VARARGS1 */

message (s, a)
char *s;
{
	VMove (ERRROW, 0);
	dowprint (s, &a);
	VClearLine ();
	errflg = 0;
}

/* VARARGS1 */

cerror (s, a)
char *s;
{
	VMove (ERRROW, 0);
	dowprint (s, &a);
	VClearLine ();
	errflg = 1;
	longjmp (mainloop, 2);
}

/* VARARGS1 */

error (s, a)
char *s;
{
	VMove (ERRROW, 0);
	dowprint (s, &a);
	VClearLine ();
	errflg = 1;
}

initterm ()
{
	char buf [2048];
	extern VFlush ();

	if (! CapInit (buf) || ! VInit ()) {
		/* "sc: cannot initialize terminal\n" */
		outerr (MSG (35));
		exit (1);
	}
	KeyInit (keymap, VFlush);
	VOpen ();
	IsStndout = VStandOut ();
	VStandEnd ();
}

endterm ()
{
	VMove (LINES-1, 0);
	VClearLine ();
	VSync ();
	VClose ();
}

static ok ()
{
	for (;;)
		switch (argetch (1)) {
		default:
			VBeep ();
			continue;
		CASE ('Y', 'Д'):
		CASE ('y', 'д'):
			return (1);
		CASE ('N', 'Н'):
		CASE ('n', 'н'):
			return (0);
		}
}

modcheck ()             /* ask and save file before exiting */
{                       /* return 1 if exit, 0 if continue session */
	register ret;

	/* "Ответьте д(а) или н(ет." */
	/* "Answer y(es) or n(o)." */
# ifdef RUS
	message (MSG (rusin ? 60 : 40));
# else
	message (MSG (40));
# endif
	VMove (HELROW, 0);
	VClearLine ();
	if (! modified ()) {
		/* "\3Exit from spreadsheet calculator? \2" */
		VPutString (MSG (41));
		ret = ok ();
	} else if (! *curfile) {
		/* "\3Save table in workfile? \2" */
		VPutString (MSG (42));
		if (ok ())
			writefile (WORKFILE);
		ret = 1;
	} else {
		/* "\3Save file \"%s\" ? \2" */
		printw (MSG (43), curfile);
		if (ok ())
			writefile (curfile);
		ret = 1;
	}
	clrerr ();
	return (ret);
}

scrdown ()
{
	register r, n;
	int oldstrow = strow;

	for (;;) {
		rows = 0;
		for (n=strow, r=TOPROW; r<BOTROW && n<MAXROWS; ++n, ++rows)
			if (! hiddnrow [n])
				++r;
		if (currow >= strow + rows)
			++strow;
		else
			break;
	}
	if (strow <= oldstrow)
		return;
	n = 0;
	for (r=oldstrow; r<strow; ++r)
		if (! hiddnrow [r])
			++n;
	if (n >= BOTROW-TOPROW)
		++FullUpdate;
	else
		for (; n>0; --n) {
			UpdateLine [BOTROW-n] = 1;
			VDelLine (TOPROW);
			VInsLine (BOTROW-1);
		}
}

scrup ()
{
	register r, n;

	if (currow >= strow)
		return;
	n = 0;
	for (r=currow; r<strow; ++r)
		if (! hiddnrow [r])
			++n;
	if (n >= BOTROW-TOPROW)
		++FullUpdate;
	else
		for (; n>0; --n) {
			UpdateLine [TOPROW+n-1] = 1;
			VDelLine (BOTROW-1);
			VInsLine (TOPROW);
		}
	strow = currow;
}

scrlr ()
{
	register c, n;

	if (curcol < stcol)
		stcol = curcol, FullUpdate++;
	for (;;) {
		cols = 0;
		for (n=stcol, c=RESCOL; c<COLS-1 && n<MAXCOLS; ++n, ++cols)
			if (! hiddncol[n])
				c += fwidth[n];
		if (c >= COLS-1)
			--cols;
		if (curcol >= stcol + cols)
			++stcol, ++FullUpdate;
		else
			break;
	}
}

maketable ()
{
	static oldccol = -1, oldcrow = -1;
	register struct ent **p;
	register row, col, r;
	register maxc, maxr;

	for (row=0; row<50; ++row)
		UpdateLine [row] = 0;

	/* restore current cell */

	if (!FullUpdate && curbox)
		VUngetBox (curbox);

	/* skip hidden rows/cols */

	while (hiddnrow [currow]) /* You can't hide the last row or col */
		currow++;
	while (hiddncol [curcol])
		curcol++;

	/* check if we must move window left, up, right, down and move it */

	if (FullUpdate || currow != oldcrow) {
		scrup ();
		scrdown ();
		oldcrow = currow;
	}
	if (FullUpdate || curcol != oldccol) {
		scrlr ();
		oldccol = curcol;
	}

	maxc = stcol + cols - 1;
	maxr = strow + rows - 1;

	/* print column names */

	if (FullUpdate) {
		VMove (NUMROW, 0);
		VClearLine ();
		if (Border) {
			VStandOut ();
			VSetDim ();
			for (col=0; col<RESCOL-1; ++col)
				VPutChar (' ');
			VPutChar (VERTLINE);
			for (col=stcol; col<=maxc; ++col) {
				if (hiddncol [col] || fwidth[col] == 0)
					continue;
				if (fwidth[col] == 1) {
					VPutChar (' ');
					continue;
				}
				VPutChar (VERTLINE);
				if (fwidth[col] == 3 && col < 26)
					VPutString (coltoa (col));
				else if (fwidth[col] > 3)
					center (coltoa (col), fwidth[col]-2);
				VPutChar (VERTLINE);
			}
			VSetNormal ();
			VStandEnd ();
		}
	}

	/* scan table and print all updated entries */

	VSync ();
	for (row=strow, r=TOPROW; row<=maxr; row++) {
		register c = RESCOL;

		if (hiddnrow [row])
			continue;
		if (FullUpdate || UpdateLine [r]) {
			VMove (r, 0);
			if (Border) {
				VStandOut ();
				VSetDim ();
				VPutString (itoaw (row, RESCOL-1));
				if (IsStndout)
					VPutChar (' ');
				else
					VPutChar (VERTLINE);
				VSetNormal ();
				VStandEnd ();
			}
			VClearLine ();
		}
		for (p = &tbl[row][col = stcol]; col <= maxc; col++, p++) {
			if (hiddncol[col])
				continue;
			if (*p && (((*p)->flags & is_changed) ||
				FullUpdate || UpdateLine[r]))
			{
				VMove (r, c);
				if (!FullUpdate && !UpdateLine[r])
					VClearLine ();
				printcell (*p);
				(*p)->flags &= ~is_changed;
				UpdateLine[r] = 1;
			}
			c += fwidth[col];
		}
		if (FullUpdate || UpdateLine [r])
			VSyncLine (r);
		r++;
	}

	/* clear rest of screen */

	if (FullUpdate)
		for (; r<BOTROW; ++r) {
			VMove (r, 0);
			VClearLine ();
		}

	tabcursor ();           /* make table cursor */
	showme ();              /* print status of current cell */
}

static center (s, n)
register char *s;
{
	register l, r;

	n -= strlen (s);
	l = n / 2;
	r = n - l;
	while (l-- > 0)
		VPutChar (' ');
	VPutString (s);
	while (r-- > 0)
		VPutChar (' ');
}

argetch (fix)
{
	register c;
	CURSOR csav;

	VSync ();
	csav = VGetCursor ();
	for (;;) {
		c = KeyGet ();
		if (errflg) {
			clrerr ();
			VSetCursor (csav);
		}
		switch (c) {
		case ctl ('x'):
			VRedraw ();
			break;
		case ctl ('y'):
		case ctl ('z'):
		case ctl ('k'):
		case ctl ('b'):
		case ctl ('l'):
		case ctl ('r'):
		case ctl ('u'):
		case ctl ('d'):
			tabmove (c);
			maketable ();
			addprm ();
			if (fix)
				VSetCursor (csav);
			VSync ();
			FullUpdate = 0;
			break;
		case ctl ('c'):
		case ctl ('['):
			clrerr ();
			longjmp (mainloop, 1);
		default:
			return (c);
		}
	}
}

tabcursor ()
{
	register lastmy, lastmx;
	register row, col;

	lastmy = TOPROW;
	for (row=strow; row<currow; ++row)
		if (! hiddnrow [row])
			++lastmy;
	lastmx = RESCOL;
	for (col=stcol; col<curcol; ++col)
		if (! hiddncol [col])
			lastmx += fwidth [col];
	if (curbox)
		VFreeBox (curbox);
	curbox = VGetBox (lastmy, lastmx, 1, fwidth [curcol]);
	VSetBold ();
	if (IsStndout) {
		VStandOut ();
		VPrintBox (curbox);
		VStandEnd ();
	} else if (fwidth[curcol]) {
		VMPutChar (lastmy, lastmx, '[');
		VMPutChar (lastmy, lastmx + fwidth[curcol] - 1, ']');
	}
	VSetNormal ();
}

addprm ()
{
	VMove (PRMROW, 0);
	VClearLine ();
	if (Assign) {
		VSetDim ();
		VPutChar (lmove ());
		VSetBold ();
# ifdef __MSDOS__
		VPutChar (16);
# else
		VPutChar ('>');
# endif
		VSetNormal ();
	} else {
		VSetDim ();
		VPutString (itoa (Arg));
		VSetBold ();
		VPutChar ('/');
	}
	if (linelim) {
		line [linelim] = 0;
		VPutString (line);
	}
	VSetNormal ();
}

static printcell (p)
register struct ent *p;
{
	char lin [80];

	if (! fwidth [p->col])
		return;
	strcell (lin, p);
	if ((p->flags & is_value) && (p->flags & is_error)) {
		VSetBold ();
		VPutString (lin);
		VSetNormal ();
	} else
		VPutString (lin);
}

addhelp ()
{
	VMove (HELROW, 0);
	VClearLine ();
	if (Assign)
		return;
	VPutString (helpstr ());
}

show (s)
char *s;
{
	VMove (PRMROW, 0);
	VSetDim ();
	VPutString (itoa (Arg));
	VSetBold ();
	VPutChar ('/');
	line [linelim] = 0;
	VPutString (line);
	VPutString (s);
	VSetNormal ();
	VClearLine ();
	VSync ();
}

static lmove ()
{
	switch (lastmove) {
# ifdef __MSDOS__
	case ctl('l'):    return (27);
	case ctl('r'):    return (26);
	case ctl('d'):    return (25);
	case ctl('u'):    return (24);
# else
	case ctl('l'):    return ('L');
	case ctl('r'):    return ('R');
	case ctl('d'):    return ('D');
	case ctl('u'):    return ('U');
# endif
	default:        return (' ');
	}
}

info ()
{
	VMove (INFROW, 0);
	if (Border) {
		VSetDim ();
		/* "table %s%d, " */
		printw (MSG (44), coltoa (MAXCOLS-1), MAXROWS-1);
		/* "last %s%d, " */
		printw (MSG (45), coltoa (maxcol), maxrow);
		/* "marked %s%d, " */
		printw (MSG (46), coltoa (savedcol), savedrow);
		/* "memory used %ld, " */
		printw (MSG (48), memavail - memfree);
		/* "free %ld" */
		printw (MSG (49), memfree);
		if (AutoCalc)
			/* ", autocalc" */
			VPutString (MSG (47));
		VSetNormal ();
	}
	VClearLine ();
}

static char *fmttoa (flg)
{
	if (LEFTALIGN (flg))
		return ("=<");
	if (RIGHTALIGN (flg))
		return ("=>");
	if (CENTER (flg))
		return ("=^");
	return ("=");
}

showme ()
{
	register struct ent *q = tbl[currow][curcol];

	VMove (STAROW, 0);
	if (cprghtflag) {
		cprghtflag = 0;
		VSetDim ();
		copyright ();
		VSetNormal ();
	} else if (Border) {
		printw ("\3%s%d \1(%d.%d)\2 ", coltoa (curcol), currow,
			fwidth [curcol], q ? q->prec : precision);
		if (q) {
			if (q->flags & is_value)
				if (q->p.expr) {
					char lin [100];

					dline = lin;
					dlim = 0;
					decompile (q->p.expr, 0);
					lin [dlim] = 0;
					/* "formula %s %s" */
					printw (MSG (50), fmttoa (q->flags), lin);
				} else
					/* "value %s %s" */
					printw (MSG (51), fmttoa (q->flags),
						dtoag (q->d.value, 15));
			else
				/* "text %s \"%s\"" */
				printw (MSG (52), fmttoa (q->flags), q->p.label);
		} else
			/* "empty" */
			VPutString (MSG (53));
	}
	VClearLine ();
}

getcell (s)
char *s;
{
	register c, newcol;
	register long newrow;

	VMove (HELROW, 0);
	VSetBold ();
	VPutString (s);
	VSetNormal ();
	VClearLine ();
	getcol = getrow = -1;
	for (;;) {
		switch (c = argetch (1)) {
		case ctl ('a'):
			if (getcol >= 0 || getrow >= 0)
				goto bad;
			getcol = curcol;
			getrow = currow;
			printw ("%s%d", coltoa (getcol), getrow);
			break;
		case ctl ('j'):
			if (getcol>=0 && getrow>=0)
				return;
bad:                    VBeep ();
			break;
		case ctl ('h'):
			if (getrow >= 10)
				getrow /= 10;
			else if (getrow >= 0)
				getrow = -1;
			else if (getcol > 25)
				getcol = getcol/26 - 1;
			else if (getcol >= 0)
				getcol = -1;
			else
				break;
			VPutString ("\b \b");
			break;
		default:
			if (isupper (c))
				c = tolower (c);
			if (isalpha (c) && getrow<0) {
				if (getcol<0)
					newcol = c - 'a';
				else
					newcol = (getcol+1) * 26 + c - 'a';
				if (newcol >= MAXCOLS)
					goto bad;
				getcol = newcol;
			} else if (isdigit (c) && getcol>=0) {
				if (getrow < 0)
					newrow = c - '0';
				else {
					if (getrow == 0)
						VPutChar ('\b');
					newrow = getrow*10L + c - '0';
				}
				if (newrow >= MAXROWS)
					goto bad;
				getrow = newrow;
			} else
				goto bad;
			VPutChar (c);
			break;
		}
	}
}

getcolumn (s)
char *s;
{
	register c, newcol;

	VMove (HELROW, 0);
	VSetBold ();
	VPutString (s);
	VSetNormal ();
	VClearLine ();
	getcol = -1;
	for (;;) {
		switch (c = argetch (1)) {
		case ctl ('a'):
			if (getcol >= 0)
				goto bad;
			getcol = curcol;
			VPutString (coltoa (getcol));
			break;
		case ctl ('j'):
			if (getcol>=0)
				return;
bad:                    VBeep ();
			break;
		case ctl ('h'):
			if (getcol > 25)
				getcol = getcol/26 - 1;
			else if (getcol >= 0)
				getcol = -1;
			else
				break;
			VPutString ("\b \b");
			break;
		default:
			if (isupper (c))
				c = tolower (c);
			if (isalpha (c)) {
				if (getcol<0)
					newcol = c - 'a';
				else
					newcol = (getcol+1) * 26 + c - 'a';
				if (newcol >= MAXCOLS)
					goto bad;
				getcol = newcol;
			} else
				goto bad;
			VPutChar (c);
			break;
		}
	}
}

getroww (s)
char *s;
{
	register c;
	register long newrow;

	VMove (HELROW, 0);
	VSetBold ();
	VPutString (s);
	VSetNormal ();
	VClearLine ();
	getrow = -1;
	for (;;) {
		switch (c = argetch (1)) {
		case ctl ('a'):
			if (getrow >= 0)
				goto bad;
			getrow = currow;
			printw ("%d", getrow);
			break;
		case ctl ('j'):
			if (getrow>=0)
				return;
bad:                    VBeep ();
			break;
		case ctl ('h'):
			if (getrow >= 10)
				getrow /= 10;
			else if (getrow >= 0)
				getrow = -1;
			else
				break;
			VPutString ("\b \b");
			break;
		default:
			if (isdigit (c)) {
				if (getrow < 0)
					newrow = c - '0';
				else {
					if (getrow == 0)
						VPutChar ('\b');
					newrow = getrow*10L + c - '0';
				}
				if (newrow >= MAXROWS)
					goto bad;
				getrow = newrow;
			} else
				goto bad;
			VPutChar (c);
			break;
		}
	}
}

getnumber (s, min, max)
char *s;
{
	register c;
	register newv;

	VMove (HELROW, 0);
	VSetBold ();
	VPutString (s);
	VSetNormal ();
	VClearLine ();
	getnum = -1;
	for (;;) {
		switch (c = argetch (1)) {
		case ctl ('j'):
			if (getnum >= min)
				return;
bad:                    VBeep ();
			break;
		case ctl ('h'):
			if (getnum >= 10)
				getnum /= 10;
			else if (getnum >= 0)
				getnum = -1;
			else
				break;
			VPutString ("\b \b");
			break;
		default:
			if (isdigit (c)) {
				if (getnum < 0)
					newv = c - '0';
				else {
					if (getnum == 0)
						VPutChar ('\b');
					newv = getnum*10L + c - '0';
				}
				if (newv > max)
					goto bad;
				getnum = newv;
			} else
				goto bad;
			VPutChar (c);
			break;
		}
	}
}

char *edit (b, e)
char *b, *e;
{
	register c;
	register char *q = e, *s;
	char dbuf [80];
	char *d = dbuf;
	CURSOR csav;

	csav = VGetCursor ();
	/* "^Left ^Right ^Delete ^Undelete ^Xclear ^Cancel Backspace Tab Return Edit" */
	message (MSG (55));
	VSetCursor (csav);
	*d = 0;
	for (;;) {
		VSync ();
		switch (c = KeyGet ()) {
		case ctl ('i'):
			if (q == e)
				for (q=s=b; s<e; ++s)
					VPutChar ('\b');
			else {
				if (q == b)
					VPutString (b);
				else
					VPutString (q);
				q = e;
			}
			break;
		case ctl ('l'):
			if (q <= b)
				continue;
			--q;
			VPutChar ('\b');
			break;
		case ctl ('r'):
			if (q >= e)
				continue;
			VPutChar (*q++);
			break;
		case ctl ('d'):
			if (e <= q)
				continue;
			if (++d > dbuf+79)
				d = dbuf;
			*d = *q;
			for (s=q+1; s<=e; ++s)
				s[-1] = s[0];
			--e;
			VDelChar ();
			break;
		case ctl ('c'):
		case ctl ('['):
			clrerr ();
			longjmp (mainloop, 1);
		case ctl ('x'):
			VRedraw ();
		case ctl ('e'):
			break;
		case ctl ('j'):
			*e = 0;
			if (q < e)
				VPutString (q);
			clrerr ();
			return (e);
		case ctl ('h'):
			if (q <= b)
				continue;
			for (s=q; s<=e; ++s)
				s[-1] = s[0];
			--q;
			--e;
			VPutChar ('\b');
			VDelChar ();
			break;
		case ctl ('u'):
			if (! *d)
				continue;
			c = *d;
			*d = 0;
			if (--d < dbuf)
				d = dbuf+79;
			VInsChar ();
			for (s=e; s>=q; --s)
				s[1] = s[0];
			VPutChar (*q = c);
			VPutChar ('\b');
			++e;
			break;
		default:
			if (e < b+80 && isprint (c)) {
				VInsChar ();
				for (s=e; s>=q; --s)
					s[1] = s[0];
				VPutChar (*q++ = c);
				++e;
			} else
				VBeep ();
			break;
		}
	}
}

getstr (s, p, def)
char *s, *p, *def;
{
	register c;
	register char *q;

	VMove (HELROW, 0);
	VSetBold ();
	VPutString (s);
	VSetNormal ();
	VClearLine ();
	q = p;
	if (def && *def) {
		VPutString (def);
		if (p != def)
			strcpy (p, def);
		q += strlen (p);
	}
	for (;;) {
		switch (c = argetch (1)) {
		case ctl ('j'):
			*q = 0;
			if (q > p)
				return;
bad:                    VBeep ();
			break;
		case ctl ('h'):
			if (q > p) {
				--q;
				VPutString ("\b \b");
			}
			break;
		case ctl ('e'):
			q = edit (p, q);
			break;
		default:
			if (q > p+80 || ! isprint (c))
				goto bad;
			VPutChar (*q++ = c);
			break;
		}
	}
}

tabmove (c)
{
	register margin;

	switch (c) {
	case ctl ('y'):
		while (--Arg >= 0) {
			if (curcol > stcol || stcol <= 0)
				curcol = stcol;
			else {
				for (c=RESCOL; c<COLS-1 && curcol>=0; --curcol) {
					if (! hiddncol [curcol])
						c += fwidth [curcol];
				}
				++curcol;
			}
		}
		++FullUpdate;
		break;
	case ctl ('z'):
		while (--Arg >= 0) {
			for (margin=stcol, c=RESCOL; c<COLS-1 && margin<MAXCOLS; ++margin)
				if (! hiddncol [margin])
					c += fwidth [margin];
			--margin;
			if (c >= COLS-1)
				--margin;
			if (curcol < margin || margin >= MAXCOLS-1)
				curcol = margin;
			else {
				stcol = margin+1;
				for (curcol=stcol, c=RESCOL; c<COLS-1; ++curcol) {
					if (curcol >= MAXCOLS) {
						++curcol;
						for (--stcol; c<COLS-1 && stcol>=0; --stcol)
							c += fwidth [stcol];
						++stcol;
						if (c >= COLS-1)
							++stcol;
						break;
					} else if (! hiddncol [curcol])
						c += fwidth [curcol];
				}
				curcol -= 2;
			}
		}
		++FullUpdate;
		break;
	case ctl ('k'):
		while (--Arg >= 0) {
			if (currow > strow || strow <= 0)
				currow = strow;
			else {
				for (c=0; c<=BOTROW-TOPROW && currow>=0; --currow) {
					if (! hiddnrow [currow])
						++c;
				}
				++currow;
			}
		}
		++FullUpdate;
		break;
	case ctl ('b'):
		while (--Arg >= 0) {
			for (margin=strow, c=0; c<=BOTROW-TOPROW && margin<MAXROWS; ++margin)
				if (! hiddnrow [margin])
					++c;
			--margin;
			if (c > BOTROW-TOPROW)
				--margin;
			if (currow < margin || margin >= MAXROWS-1)
				currow = margin;
			else {
				strow = margin+1;
				for (currow=strow, c=0; c<=BOTROW-TOPROW; ++currow) {
					if (currow >= MAXROWS) {
						++currow;
						for (--strow; c<=BOTROW-TOPROW && strow>=0; --strow)
							++c;
						++strow;
						if (c > BOTROW-TOPROW)
							++strow;
						break;
					} else if (! hiddnrow [currow])
						++c;
				}
				currow -= 2;
			}
		}
		++FullUpdate;
		break;
	case ctl ('l'):
		lastmove = c;
		while (--Arg >= 0) {
			if (curcol)
				curcol--;
			while (hiddncol[curcol] && curcol)
				curcol--;
		}
		break;
	case ctl ('r'):
		lastmove = c;
		while (--Arg >= 0) {
			if (curcol < MAXCOLS - 1)
				curcol++;
			while (hiddncol[curcol] && (curcol<MAXCOLS-1))
				curcol++;
		}
		break;
	case ctl ('d'):
		lastmove = c;
		while (--Arg >= 0) {
			if (currow < MAXROWS - 1)
				currow++;
			while (hiddnrow[currow] && (currow < MAXROWS - 1))
				currow++;
		}
		break;
	case ctl ('u'):
		lastmove = c;
		while (--Arg >= 0) {
			if (currow)
				currow--;
			while (hiddnrow[currow] && currow)
				currow--;
		}
		break;
	}
	narg = 0;
	Arg = 1;
}
