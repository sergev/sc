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

# include <stdio.h>
# include "ctype.h"
# include "sc.h"
# include "graph.h"

# define TRIGHT()	(wasr = 1, putc ('r', f))
# define TLEFT()	(wasr = 0, putc ('l', f))
# define TCENTER()	(wasr = 1, putc ('c', f))
# define TSPAN()	putc (wasr ? (wasr = 0, 'l') : 's', f)

static oldscol, oldsrow;
static struct graph oldgraph;

extern char *dline;                     /* defined in decomp.c */
extern int dlim;                        /* defined in decomp.c */
extern nowrkflg;                        /* defined in main.c */
extern savedrow, savedcol;              /* defined in command.c */
extern Border;

extern char *strcpy (), *dtoaf (), *dtoag (), *itoaw ();

modified ()
{
	if (modflg)
		return (1);
	if (savedcol != oldscol || savedrow != oldsrow)
		return (1);
	if (graph.shape != oldgraph.shape)
		return (1);
	if (modaxis (&graph.axisA, &oldgraph.axisA))
		return (1);
	if (modaxis (&graph.axisB, &oldgraph.axisB))
		return (1);
	if (modaxis (&graph.axisC, &oldgraph.axisC))
		return (1);
	if (modaxis (&graph.axisD, &oldgraph.axisD))
		return (1);
	return (0);
}

modaxis (a, b)
register struct axis *a, *b;
{
	if (a->minr != b->minr)
		return (1);
	if (a->minc != b->minc)
		return (1);
	if (a->maxr != b->maxr)
		return (1);
	if (a->maxc != b->maxc)
		return (1);
	return (0);
}

char *filename (s, ex, def)
register char *s, *ex;
char *def;
{
	register char *p;
	static char lin [100];

	/* "Suffix is \"%s\" by default." */
	message (MSG (30), ex);
	if (def) {
		strcpy (lin, def);
		for (p=lin; *p; ++p);
		for (--p; p>=lin && *p!='.'; --p);
		if (p >= lin)
			*p = 0;
		getstr (s, lin, lin);
	} else
		getstr (s, lin, (char *) 0);
	clrerr ();
	fileext (lin, ex);
	return (lin);
}

fileext (p, ex)		/* add extension ex to file name p if none */
register char *p, *ex;
{
	register char *s;

	for (s=0; *p; ++p)
		if (*p == '.')
			s = p;
	if (!s || *s != '.') {
		*p++ = '.';
		strcpy (p, ex);
	}
}

printfile (fname)
char *fname;
{
	FILE *f;
	char pline [512];
	int plinelim;
	register row, col;
	register struct ent **p;
	register c;

	if (! (f = fopen (fname, "w")))
		/* "Can't create %s" */
		cerror (MSG (31), fname);

	if (Border) {
		plinelim = 0;
		for (col=0; col<RESCOL-1; ++col)
			pline [plinelim++] = ' ';
		pline [plinelim++] = '|';
		c = plinelim;
		for (col=0; col<=maxcol; ++col) {
			if (hiddncol [col] || !fwidth[col])
				continue;
			if (fwidth[col] == 1) {
				pline [plinelim++] = ' ';
				++c;
				continue;
			}
			pline [plinelim++] = '|';
			if (fwidth[col] == 3 && col < 26)
				strcpy (pline+plinelim, coltoa (col));
			else if (fwidth[col] > 3)
				stralign (pline+plinelim, coltoa (col),
					fwidth[col]-2, is_center, 0);
			else
				pline [plinelim] = 0;
			plinelim += strlen (pline + plinelim);
			c += fwidth [col] - 1;
			while (plinelim < c)
				pline [plinelim++] = ' ';
			plinelim = c;
			pline [plinelim++] = '|';
			++c;
		}
		pline [plinelim++] = '\n';
		pline [plinelim] = 0;
		fputs (pline, f);
	}

	for (row=0; row<=maxrow; ++row) {
		if (hiddnrow [row])
			continue;
		if (Border) {
			strcpy (pline, itoaw (row, RESCOL-1));
			pline [RESCOL-1] = '|';
			pline [RESCOL] = 0;
			c = plinelim = RESCOL;
		} else
			c = plinelim = 0;
		for (p= &tbl[row][col=0]; col<=maxcol; ++col, ++p) {
			if (hiddncol [col] || !fwidth [col])
				continue;
			if (*p) {
				while (plinelim < c)
					pline [plinelim++] = ' ';
				plinelim = c;
				strcell (pline + plinelim, *p);
				plinelim += strlen (pline + plinelim);
			}
			c += fwidth [col];
		}
		for (c=0; c<plinelim && pline[c]; ++c)
			putc (pline [c], f);
		putc ('\n', f);
	}
	fclose(f);
}

strcell (lin, p)
char *lin;
register struct ent *p;
{
	/* print content of cell p into lin.
	/* lin may be longer than column width
	/* if it contains string longer */

	register char *s;
	char cell [80];

	if (p->flags & is_value) {
		if (p->flags & is_error)
			s = BADREFERENCE;
		else
			strcpy (s = cell, dtoaf (p->d.value, p->prec));
	} else
		s = p->p.label;
	stralign (lin, s, fwidth[p->col], p->flags & is_center, p->flags & is_value);
}

static stralign (d, s, width, align, isval)
register char *d, *s;
{
	register i, len;

	len = strlen (s);
	switch (align) {
	case 0:
		if (isval)
			goto rightflush;
	case is_leftflush:
leftflush:
		strcpy (d, s);
		break;
	case is_rightflush:
rightflush:
		i = width - len;
		if (i < 0)
			goto leftflush;
		goto lalign;
	case is_center:
		i = width - len;
		if (i < 0)
			goto leftflush;
		++i;
		i /= 2;
lalign:
		while (--i >= 0)
			*d++ = ' ';
		strcpy (d, s);
		break;
	}
}

tblprintfile (fname)
char *fname;
{
	FILE *f;
	register row, col;
	register struct ent **p;
	char coldelim = DEFCOLDELIM;
	register wasr;

	if (! (f = fopen (fname, "w")))
		/* "Can't create %s" */
		cerror (MSG (31), fname);

	/* print table */

	fprintf (f, ".TS\nbox, center, tab(%c);\n", coldelim);
	if (Border) {
		fprintf (f, "r0");
		for (col=0; col<=maxcol; col++) {
			if (hiddncol [col] || !fwidth [col])
				continue;
			putc ('|', f);
			fprintf (f, "c0w%d", fwidth[col]);
		}
		putc ('|', f);  /* adjust right margin of box */
		putc ('\n', f);
	}
	for (row=0; row<=maxrow; row++) {
		if (hiddnrow [row])
			continue;
		if (Border)
			fprintf (f, "r0|");
		wasr = 1;
		for (p= &tbl[row][col=0]; col<=maxcol; ++col, ++p) {
			if (hiddncol [col] || !fwidth [col])
				continue;
			/* no span after rightalign ! */
			if (*p) {
				if ((*p)->flags & is_value || (*p)->p.label) {
					if (DFLTALIGN ((*p)->flags))
						if ((*p)->flags & is_value)
							TRIGHT ();
						else
							TLEFT ();
					else if (RIGHTALIGN ((*p)->flags))
						TRIGHT ();
					else if (LEFTALIGN ((*p)->flags))
						TLEFT ();
					else if (CENTER ((*p)->flags))
						TCENTER ();
				} else
					TSPAN ();
			} else
				TSPAN ();
			fprintf (f, "0w%d ", fwidth [col]);
		}
		putc ('\n', f);
	}
	fprintf (f, ".\n");

	if (Border) {
		fprintf (f, "\\&");
		for (col=0; col<=maxcol; col++) {
			if (hiddncol [col] || !fwidth [col])
				continue;
			putc (coldelim, f);
			if (fwidth[col] > 1 || col < 26)
				fprintf (f, coltoa (col));
		}
		fprintf (f, "\n_\n");
	}
	for (row=0; row<=maxrow; ++row) {
		if (hiddnrow [row])
			continue;
		if (Border)
			fprintf (f, "%d %c", row, coldelim);
		wasr = 1;
		for (p= &tbl[row][col=0]; col<=maxcol; ++col, ++p) {
			if (hiddncol [col] || !fwidth [col])
				continue;
			/* no delimiters on spanned field ! */
			if (*p) {
				char *s;

				if ((*p)->flags & is_value) {
					fprintf (f, dtoaf ((*p)->d.value,
						(*p)->prec));
					putc (coldelim, f);
					wasr = RIGHTALIGN ((*p)->flags) != 0;
				} else if (s = (*p)->p.label) {
					if (*s && !s[1])
						fprintf (f, "\\&");
					fprintf (f, "%s%c", s, coldelim);
					wasr = RIGHTALIGN ((*p)->flags) != 0;
				} else if (wasr)
					wasr = 0, putc (coldelim, f);
			} else if (wasr)
				wasr = 0, putc (coldelim, f);
		}
		putc ('\n', f);
	}
	fprintf (f, ".TE\n");
	fclose (f);
}

writefile (fname)
char *fname;
{
# ifdef DEMO
	fname = fname;
# else
	register FILE *f;
	register struct ent **p;
	register r, c;
	char save [32];

	if (! *fname)
		fname = &curfile[0];
	strcpy (save, fname);
	f = fopen (save, "w");
	if (! f)
		/* "Can't create %s" */
		cerror (MSG (31), save);

	if (graph.shape)
		fprintf (f, "gs%d\n", graph.shape);
	if (graph.axisA.valid)
		fprintf (f, "gab%s%d\ngae%s%d\n",
			coltoa (graph.axisA.minc), graph.axisA.minr,
			coltoa (graph.axisA.maxc), graph.axisA.maxr);
	if (graph.axisB.valid)
		fprintf (f, "gbb%s%d\ngbe%s%d\n",
			coltoa (graph.axisB.minc), graph.axisB.minr,
			coltoa (graph.axisB.maxc), graph.axisB.maxr);
	if (graph.axisC.valid)
		fprintf (f, "gcb%s%d\ngce%s%d\n",
			coltoa (graph.axisC.minc), graph.axisC.minr,
			coltoa (graph.axisC.maxc), graph.axisC.maxr);
	if (graph.axisD.valid)
		fprintf (f, "gdb%s%d\ngde%s%d\n",
			coltoa (graph.axisD.minc), graph.axisD.minr,
			coltoa (graph.axisD.maxc), graph.axisD.maxr);
	if (savedcol || savedrow)
		fprintf (f, "m%s%d\n", coltoa (savedcol), savedrow);
	if (precision != DEFPREC)
		fprintf (f, "d%d\n", precision);
	for (c=0; c<MAXCOLS; c++) {
		if (fwidth [c] != DEFWIDTH)
			fprintf (f, "w%s%d\n", coltoa (c), fwidth [c]);
		if (hiddncol [c])
			fprintf (f, "h%s\n", coltoa (c));
	}
	for (r=0; r<=maxrow; r++) {
		if (hiddnrow [r])
			fprintf (f, "h%d\n", r);
	}
	for (r=0; r<=maxrow; r++) {
		p = &tbl[r][0];
		for (c=0; c<=maxcol; c++, p++)
			if (*p) {
				fprintf (f, "l%s%d", coltoa (c), r);
				if ((*p)->flags & is_value)
					putc ('=', f);
				else if (DFLTALIGN ((*p)->flags))
					putc ('"', f);
				if (RIGHTALIGN ((*p)->flags))
					putc ('>', f);
				else if (LEFTALIGN ((*p)->flags))
					putc ('<', f);
				else if (CENTER ((*p)->flags))
					putc ('^', f);
				if ((*p)->flags & is_value) {
					if ((*p)->p.expr) {
						char lin [100];

						dline = lin;
						dlim = 0;
						decompile ((*p)->p.expr, 0);
						lin [dlim] = 0;
						fprintf (f, "%s\n", lin);
					} else {
						fprintf (f, dtoag ((*p)->d.value, 15));
						putc ('\n', f);
					}
				} else
					fprintf (f, "%s\n", (*p)->p.label);
				if ((*p)->prec != precision)
					fprintf (f, "p%s%d=%d\n", coltoa (c), r, (*p)->prec);
			}
	}
	fclose (f);
	strcpy (curfile, save);

	modflg = 0;
	/* "File '%s' written." */
	error (MSG (32), curfile);
# endif
}

static erasedb ()
{
	register r, c;

	precision = DEFPREC;
	for (c=0; c<MAXCOLS; c++)
		fwidth [c] = DEFWIDTH;
	for (r=0; r<=maxrow; r++) {
		register struct ent **p = &tbl[r][0];

		for (c=0; c++ <= maxcol; p++)
			if (*p) {
				if ((*p)->flags & is_value) {
					if ((*p)->p.expr)
						efree ((*p)->p.expr);
				} else if ((*p)->p.label)
					mfree ((*p)->p.label, (unsigned) strlen ((*p)->p.label) + 1);
				mfree ((char *) *p, sizeof (struct ent));
				*p = 0;
			}
	}
	maxrow = maxcol = 0;
	savedrow = savedcol = 0;
	graph.shape = 0;
	graph.axisA.valid = graph.axisB.valid = 0;
	graph.axisC.valid = graph.axisD.valid = 0;
	++FullUpdate;
}

readfile (fname, eraseflg)
char *fname;
{
	register FILE *f;
	int c, r;
	register char *s;
	register struct axis *a;
	register struct ent *p;
	char save [100];
	char lin [100];

	if (! *fname)
		fname = &curfile[0];
	if (*fname)
		strcpy (save, fname);
	else if (nowrkflg)
		return;
	else
		strcpy (save, WORKFILE);
	if (modified () && eraseflg && strcmp (save, curfile) && ! modcheck ())
		return;
	if (! (f = fopen (save, "r"))) {
		if (*fname)
			/* "Can't read %s" */
			error (MSG (33), save);
		return;
	}
	if (eraseflg)
		erasedb ();
	while (getlin (lin, (int) sizeof (lin), f)) {
		switch (lin [0]) {
		case 'g':               /* graph */
			if (! eraseflg)
				continue;
			switch (lin [1]) {
			case 'a':               /* axis A */
				a = &graph.axisA;
laxis:
				switch (lin [2]) {
				case 'b':               /* begin */
					if (getcolrow (lin+3, &c, &r)) {
						a->minr = r;
						a->minc = c;
						a->valid = 1;
					}
					break;
				case 'e':               /* end */
					if (getcolrow (lin+3, &c, &r)) {
						a->maxr = r;
						a->maxc = c;
						a->valid = 1;
					}
					break;
				}
				break;
			case 'b':               /* axis B */
				a = &graph.axisB;
				goto laxis;
			case 'c':               /* axis C */
				a = &graph.axisC;
				goto laxis;
			case 'd':               /* axis C */
				a = &graph.axisD;
				goto laxis;
			case 's':               /* shape */
				graph.shape = atoi (lin+2);
				break;
			}
			break;
		case 'h':               /* hidden */
			if (! eraseflg)
				continue;
			if (isdigit (lin [1])) {
				if (get1row (lin+1, &r))
					hiddnrow [r] = 1;
			} else {
				if (get1col (lin+1, &c))
					hiddncol [c] = 1;
			}
			break;
		case 'w':               /* column width */
			if (! eraseflg)
				continue;
			if (getncol (lin+1, &c, &r))
				fwidth [c] = r;
			break;
		case 'd':               /* default precision */
			if (! eraseflg)
				continue;
			precision = atoi (lin+1);
			if (precision < 0)
				precision = 0;
			if (precision > 72)
				precision = 72;
			break;
		case 'p':               /* precision */
			if (getcolrow (lin+1, &c, &r)) {
				for (s=lin+1; isalnum (*s); ++s);
				p = lookat (r, c);
				if (*s == '=')
					p->prec = atoi (s+1);
			}
			break;
		case 'm':               /* mark */
			if (! eraseflg)
				continue;
			if (getcolrow (lin+1, &c, &r)) {
				savedcol = c;
				savedrow = r;
			}
			break;
		case 'l':               /* "let" */
			if (getcolrow (lin+1, &c, &r)) {
				for (s=lin+1; isalnum (*s); ++s);
				parse (lookat (r, c), s);
			}
			break;
		}
	}
	fclose (f);
	if (eraseflg)
		strcpy (curfile, save);
	modflg = ! eraseflg;
	if (eraseflg) {
		oldscol = savedcol;
		oldsrow = savedrow;
		oldgraph = graph;
	}
	++FullUpdate;
}

static get1row (s, rp)
register char *s;
register *rp;
{
	return (chkref (*rp = atoi (s), 0));
}

static get1col (s, cp)
register char *s;
register *cp;
{
	if (isupper (*s))
		*s = tolower (*s);
	if (*s < 'a' || *s > 'z')
		return (0);
	*cp = *s++ - 'a';
	if (isupper (*s))
		*s = tolower (*s);
	if (*s >= 'a' && *s <= 'z')
		*cp = (*cp + 1) * 26 + *s++ - 'a';
	return (chkref (0, *cp));
}

static getncol (s, cp, rp)
register char *s;
register *cp, *rp;
{
	if (isupper (*s))
		*s = tolower (*s);
	if (*s < 'a' || *s > 'z')
		return (0);
	*cp = *s++ - 'a';
	if (isupper (*s))
		*s = tolower (*s);
	if (*s >= 'a' && *s <= 'z')
		*cp = (*cp + 1) * 26 + *s++ - 'a';
	*rp = atoi (s);
	if (*rp < 0) *rp = 0;
	if (*rp >= 72) *rp = 72;
	return (chkref (0, *cp));
}

static getcolrow (s, cp, rp)
register char *s;
register *cp, *rp;
{
	if (isupper (*s))
		*s = tolower (*s);
	if (*s < 'a' || *s > 'z')
		return (0);
	*cp = *s++ - 'a';
	if (isupper (*s))
		*s = tolower (*s);
	if (*s >= 'a' && *s <= 'z')
		*cp = (*cp + 1) * 26 + *s++ - 'a';
	*rp = atoi (s);
	return (chkref (*rp, *cp));
}

static getlin (lin, n, f)
register FILE *f;
register char *lin;
register n;
{
	register i, c;

	for (i=0; i<n-1; ++i) {
		do
			if ((c = getc (f)) == EOF)
				return (0);
		while (c == '\t');
		if (c == '\n')
			break;
		lin [i] = c;
	}
	lin [i] = 0;
	return (1);
}
