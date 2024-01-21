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

# include "sc.h"

extern getrow, getcol, getnum;          /* defined in command.c */

extern char *mallo(), *strcpy (), *dtoag (), *sprint ();
extern double atod ();

deleterow (n)
register n;
{
	register struct ent **p, **q;
	register r, c, i;

	if (n > maxrow-currow+1)
		n = maxrow-currow+1;
	if (n < 1)
		return;
	shiftref (currow, -n, 0, 0);
	flush_saved ();                         /* clear saved buffer */
	erase_area (currow, 0, currow+n-1, maxcol);     /* save cells to be deleted */
	for (i=currow; i<MAXROWS-n; i++)
		hiddnrow [i] = hiddnrow [i + n];
	for (i=1; i<=n; i++)
		hiddnrow [MAXROWS-i] = 0;
	for (r=currow; r<=maxrow-n; ++r)
		for (c=maxcol+1, p= &tbl[r][0], q= &tbl[r+n][0]; --c>=0; ++p, ++q)
			if (*p = *q)
				(*p)->row -= n;
	for (i=1; i<=n; i++) {
		p = &tbl[maxrow+1-i][0];
		for (c=maxcol+1; --c >= 0;)
			*p++ = 0;
	}
	maxrow -= n;
	if (maxrow < 0)
		maxrow = 0;
	unrefrow ();           /* adjust references to deleted rows */
	++FullUpdate;
	++modflg;
}

deletecol (n)
register n;
{
	register r, c;
	register struct ent **p;

	if (n > maxcol-curcol+1)
		n = maxcol-curcol+1;
	if (n < 1)
		return;
	shiftref (0, 0, curcol, -n);
	flush_saved ();
	erase_area (0, curcol, maxrow, curcol+n-1);
	for (c=curcol; c<MAXCOLS-n; c++) {
		fwidth [c] = fwidth [c + n];
		hiddncol [c] = hiddncol [c + n];
	}
	for (c=1; c<=n; c++) {
		fwidth [MAXCOLS-c] = DEFWIDTH;
		hiddncol [MAXCOLS-c] = 0;
	}
	for (r=0; r<=maxrow; r++) {
		p = &tbl[r][curcol];
		for (c=maxcol-curcol+1-n; --c >= 0; ++p)
			if (p[0] = p[n])
				p[0]->col -= n;
		for (c=n; --c >= 0; )
			*p++ = 0;
	}
	maxcol -= n;
	if (maxcol < 0)
		maxcol = 0;
	unrefcol ();
	++FullUpdate;
	++modflg;
}

duprow (arg)
{
	register row, col;
	register struct ent *p;

	if (currow > maxrow)
		return;
	if (maxrow+arg >= MAXROWS)
		/* "The table can't be any bigger" */
		cerror (MSG (3));
	openrow (currow+1, arg);
	syncrefs ();
	for (col=0; col<=maxcol; ++col)
		if (p = tbl[currow][col])
			for (row=1; row<=arg; ++row)
				copy1 (p, lookat (currow+row, col), row, 0);
	currow += arg;
}

dupcol (arg)
{
	register row, col;
	register struct ent *p;

	if (curcol > maxcol)
		return;
	if (maxcol+arg >= MAXCOLS)
		/* "The table can't be any wider" */
		cerror (MSG (4));
	opencol (curcol+1, arg);
	syncrefs ();
	for (row=0; row<=maxrow; ++row)
		if (p = tbl[row][curcol])
			for (col=1; col<=arg; ++col)
				copy1 (p, lookat (row, curcol+col), 0, col);
	curcol += arg;
}

openrow (rs, n)
{
	register r, c;
	register struct ent **p, **q;

	if (rs+n >= maxheight ())
		/* "The table can't be any longer" */
		cerror (MSG (5));
	maxrow += n;
	if (maxrow >= MAXROWS)
		newheight (maxrow+1);
	shiftref (rs, n, 0, 0);
	for (r=maxrow; r>=rs+n; --r)
		hiddnrow [r] = hiddnrow [r-n];
	for (r=maxrow; r>=rs+n; --r)
		for (c=maxcol+1, p= &tbl[r][0], q= &tbl[r-n][0]; --c>=0; p++, q++)
			if (*p = *q)
				(*p)->row += n;
	for (r=rs; r<rs+n; ++r) {
		p = &tbl[r][0];
		for (c=maxcol+1; --c>=0;)
			*p++ = 0;
	}
	++FullUpdate;
	++modflg;
}

opencol (cs, n)
{
	register r, c;
	register struct ent **p;

	if (cs+n >= maxwidth ())
		/* "The table can't be any wider" */
		cerror (MSG (4));
	maxcol += n;
	if (maxcol >= MAXCOLS)
		newwidth (maxcol+1);
	shiftref (0, 0, cs, n);
	for (c=maxcol; c>=cs+n; --c) {
		fwidth[c] = fwidth[c - n];
		hiddncol[c]  = hiddncol[c - n];
	}
	for (r=0; r<=maxrow; ++r) {
		p = &tbl[r][maxcol];
		for (c=maxcol-n-cs+1; --c>=0; --p)
			if (p[0] = p[-n])
				p[0]->col += n;
		for (c=0; c<n; ++c)
			*p-- = 0;
	}
	++FullUpdate;
	++modflg;
}

erase_area (sr, sc, er, ec)
{
	register r, c;
	register struct ent **p;

	if (sr > er) { r = sr; sr = er; er = r; }
	if (sc > ec) { c = sc; sc = ec; ec = c; }
	if (sr < 0) sr = 0;
	if (sc < 0) sc = 0;
	if (er >= MAXROWS) er = MAXROWS-1;
	if (ec >= MAXCOLS) ec = MAXCOLS-1;

	for (r=sr; r<=er; r++) {
		for (c=sc; c<=ec; c++) {
			p = &tbl[r][c];
			if (*p) {
				free_ent (*p);
				*p = 0;
			}
		}
	}
}

setformat (sr, sc, er, ec, fmt)
{
	register r, c;
	register struct ent *p;

	if (sr > er) { r = sr; sr = er; er = r; }
	if (sc > ec) { c = sc; sc = ec; ec = c; }
	if (sr < 0) sr = 0;
	if (sc < 0) sc = 0;
	if (er >= MAXROWS) er = MAXROWS-1;
	if (ec >= MAXCOLS) ec = MAXCOLS-1;

	for (r=sr; r<=er; r++) {
		for (c=sc; c<=ec; c++) {
			p = tbl[r][c];
			if (p) {
				p->flags &= ~is_center;
				p->flags |= fmt | is_changed;
				modflg = 1;
			}
		}
	}
}

setfixed (sr, sc, er, ec, prec)
{
	register r, c;
	register struct ent *p;

	if (sr > er) { r = sr; sr = er; er = r; }
	if (sc > ec) { c = sc; sc = ec; ec = c; }
	if (sr < 0) sr = 0;
	if (sc < 0) sc = 0;
	if (er >= MAXROWS) er = MAXROWS-1;
	if (ec >= MAXCOLS) ec = MAXCOLS-1;

	for (r=sr; r<=er; r++) {
		for (c=sc; c<=ec; c++) {
			p = tbl[r][c];
			if (p) {
				p->prec = prec;
				p->flags |= is_changed;
				modflg = 1;
			}
		}
	}
}

valueize_area (sr, sc, er, ec)
{
	register r, c;
	register struct ent *p;

	if (sr > er) { r = sr; sr = er; er = r; }
	if (sc > ec) { c = sc; sc = ec; ec = c; }
	for (r=sr; r<=er; r++) {
		for (c=sc; c<=ec; c++) {
			p = tbl[r][c];
			if (p && (p->flags & is_value) && p->p.expr) {
				efree (p->p.expr);
				p->p.expr = 0;
				p->flags |= is_changed;
			}
		}
	}
	changed = 1;
}

pullcells (to_insert)
{
	register struct ent *p;
	register struct ent *n;
	register deltar, deltac;
	int minrow, mincol;
	int maxr, maxc;
	int numrows, numcols;

	if (! to_fix)
		return;
	minrow = MAXROWS;
	mincol = MAXCOLS;
	maxr = 0;
	maxc = 0;
	for (p=to_fix; p; p=p->next) {
		if (p->row < minrow)
			minrow = p->row;
		if (p->row > maxr)
			maxr = p->row;
		if (p->col < mincol)
			mincol = p->col;
		if (p->col > maxc)
			maxc = p->col;
	}
	numrows = maxr - minrow + 1;
	numcols = maxc - mincol + 1;
	deltar = currow - minrow;
	deltac = curcol - mincol;
	if (to_insert == 'r') {
		openrow (currow, numrows);
		deltac = 0;
	} else if (to_insert == 'c') {
		opencol (curcol, numcols);
		deltar = 0;
	}
	++FullUpdate;
	++modflg;
	for (p=to_fix; p; p=p->next) {
		n = lookat (p->row + deltar, p->col + deltac);
		clearent (n);
		copy1 (p, n, deltar, deltac);
		n->flags &= ~is_deleted;
	}
}

colshow ()
{
	register i, j, first;

	for (i=0; i<MAXCOLS; ++i)
		if (hiddncol[i])
			break;
	if (i >= MAXCOLS)
		return;
	for (j=i; j<MAXCOLS; ++j)
		if (!hiddncol[j])
			break;
	--j;
	/* "First hidden range is %s-%s." */
	message (MSG (6), coltoa (i), coltoa (j));
	/* "Enter first column: " */
	getcolumn (MSG (7));
	first = getcol;
	/* "Enter last column: " */
	getcolumn (MSG (8));
	while (first <= getcol)
		hiddncol [first++] = 0;
	++FullUpdate;
	++modflg;
	clrerr ();
}

rowshow ()
{
	register i, j, first;

	for (i=0; i<MAXROWS; ++i)
		if (hiddnrow[i])
			break;
	if (i >= MAXROWS)
		return;
	for (j=i; j<MAXROWS; ++j)
		if (!hiddnrow[j]) {
			break;
		}
	--j;
	/* "First hidden range is %d-%d." */
	message (MSG (9), i, j);
	/* "Enter first row: " */
	getroww (MSG (10));
	first = getrow;
	/* "Enter last row: " */
	getroww (MSG (11));
	while (first <= getrow)
		hiddnrow [first++] = 0;
	++FullUpdate;
	++modflg;
	clrerr ();
}

hiderow (arg)
register arg;
{
	register r;

	for (r=currow; arg>0; ++r) {
		if (r > MAXROWS - 2) {
			/* "You can't hide the last row" */
			error (MSG (12));
			break;
		}
		if (! hiddnrow[r]) {
			hiddnrow[r] = 1;
			--arg;
		}
	}
	++FullUpdate;
	++modflg;
}

hidecol (arg)
register arg;
{
	register c;

	for (c=curcol; arg>0; ++c) {
		if (c > MAXCOLS - 2) {
			/* "You can't hide the last col" */
			error (MSG (13));
			break;
		}
		if (! hiddncol[c]) {
			hiddncol[c] = 1;
			--arg;
		}
	}
	++FullUpdate;
	++modflg;
}

copy (maxr, maxc, minr, minc)
{
	register r, c;
	register struct ent *p, *n;
	register deltar, deltac;
	char lin [40];

	if (minr > maxr) { r = maxr; maxr = minr; minr = r; }
	if (minc > maxc) { c = maxc; maxc = minc; minc = c; }
	/* "Copy area %s%d-%s%d to: " */
	sprint (lin, MSG (14), coltoa (minc), minr, coltoa (maxc), maxr);
	getcell (lin);
	if (getrow+maxr-minr >= MAXROWS || getcol+maxc-minc >= MAXCOLS)
		/* "The table can't be any bigger" */
		cerror (MSG (3));
	deltar = getrow - minr;
	deltac = getcol - minc;
	for (r=minr; r<=maxr; ++r)
		for (c=minc; c<=maxc; ++c) {
			if (n = tbl [r+deltar] [c+deltac]) {
				clearent (n);
				mfree ((char *) n, sizeof (struct ent));
				tbl [r+deltar] [c+deltac] = 0;
			}
			if (p = tbl[r][c])
				copy1 (p, lookat (r+deltar, c+deltac), deltar, deltac);
		}
	++FullUpdate;
	++modflg;
}

fill (maxr, maxc, minr, minc)
{
	register r, c;
	register struct ent *n;
	double start, inc;
	char lin [100];

	if (minr > maxr) { r = maxr; maxr = minr; minr = r; }
	if (minc > maxc) { c = maxc; maxc = minc; minc = c; }
	/* "Fill area %s%d-%s%d by: " */
	sprint (lin, MSG (15), coltoa (minc), minr, coltoa (maxc), maxr);
	getstr (lin, lin, (char *) 0);
	start = atod (lin);
	/* "Fill area %s%d-%s%d by %s, step: " */
	sprint (lin, MSG (16), coltoa (minc), minr, coltoa (maxc), maxr, dtoag (start, 15));
	getstr (lin, lin, (char *) 0);
	inc = atod (lin);
	for (r=minr; r<=maxr; ++r)
		for (c=minc; c<=maxc; ++c) {
			n = lookat (r, c);
			clearent (n);
			n->d.value = start;
			start += inc;
			n->flags |= is_changed | is_value;
		}
	/* "Area %s%d-%s%d filled by %s, step %s." */
	strcpy (lin, dtoag (start, 15));
	error (MSG (17), coltoa (minc), minr, coltoa (maxc), maxr, lin, dtoag (inc, 15));
	++FullUpdate;
	++modflg;
}
