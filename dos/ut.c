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
# include "graph.h"

extern getnum;
extern savedrow, savedcol;      /* defined in command.c */

extern char *mallo (), *reallo (), *strcpy ();

struct ent *lookat (row, col)
{
	register struct ent **p;

	if (! chkref (row, col))
		return (0);
	p = &tbl[row][col];
	if (! *p) {
		*p = (struct ent*) mallo ((unsigned) sizeof (struct ent));
		if (row > maxrow)
			maxrow = row;
		if (col > maxcol)
			maxcol = col;
		(*p)->flags = is_value | is_changed | is_error;
		(*p)->prec = precision;
		(*p)->p.expr = 0;
		(*p)->row = row;
		(*p)->col = col;
		(*p)->d.value = 0.0;
	}
	return (*p);
}

chkref (row, col)
register row, col;
{
	if (row<0 || col<0)
		return (0);
	if (row >= MAXROWS) {
		if (col >= MAXCOLS) {
			if (! chkmem (row+1-MAXROWS, col+1-MAXCOLS))
				return (0);
			newheight (row+1);
			newwidth (col+1);
		} else {
			if (row >= maxheight ())
				return (0);
			newheight (row+1);
		}
	} else if (col >= MAXCOLS) {
		if (col >= maxwidth ())
			return (0);
		newwidth (col+1);
	}
	return (1);
}

clearent (v)
register struct ent *v;
{
	if (! v)
		return;
	v->d.value = 0.0;
	if (v->flags & is_value) {
		if (v->p.expr)
			efree (v->p.expr);
		v->p.expr = 0;
	} else if (v->p.label) {
		mfree (v->p.label, (unsigned) strlen (v->p.label) + 1);
		v->p.label = 0;
	}
	v->flags |= is_changed;
	v->flags &= ~(is_value | is_error);
	changed++;
	modflg++;
}

efree (e)
register struct enode *e;
{
	if (! e)
		return;
	switch (e->op) {
	case VAR:
	case CONST:
	case RADD:
	case RMUL:
	case RSUB:
	case RDIV:
		break;
	default:
# ifdef OPTIM
		if (! (e->e.o.lrflag & LVAR))
# endif
		efree (e->e.o.left);
# ifdef OPTIM
		if (! (e->e.o.lrflag & RVAR))
# endif
		efree (e->e.o.right);
	}
	mfree ((char *) e, sizeof (struct enode));
}

static struct enode *copye (e, Rdelta, Cdelta)
register struct enode *e;
{
	register struct enode *ret;
	register rd = Rdelta, cd = Cdelta;

	if (! e)
		return (0);
	ret = (struct enode *) mallo ((unsigned) sizeof (struct enode));
	ret->op = e->op;
	switch (ret->op) {
	case VAR:
		ret->e.v = lookat (e->e.v->row + rd, e->e.v->col + cd);
		break;
	case CONST:
		ret->e.k = e->e.k;
		break;
	case RADD:
	case RMUL:
	case RSUB:
	case RDIV:
		ret->e.o.right = (struct enode *) lookat (
			((struct ent *) e->e.o.right) -> row + rd,
			((struct ent *) e->e.o.right) -> col + cd);
		ret->e.o.left = (struct enode *) lookat (
			((struct ent *) e->e.o.left) -> row + rd,
			((struct ent *) e->e.o.left) -> col + cd);
		break;
	case FIXED:
		cd = rd = 0;
	default:
# ifdef OPTIM
		ret->e.o.lrflag = e->e.o.lrflag;
		if (e->e.o.lrflag & LVAR) {
			register struct ent *p = (struct ent *) e->e.o.left;

			if (p)
				if (e->e.o.lrflag & LFIXED)
					ret->e.o.left = (struct enode *) lookat (
						p->row, p->col);
				else
					ret->e.o.left = (struct enode *) lookat (
						p->row + rd, p->col + cd);
			else
				ret->e.o.left = 0;
		} else
# endif
		ret->e.o.left = copye (e->e.o.left, rd, cd);
# ifdef OPTIM
		if (e->e.o.lrflag & RVAR) {
			register struct ent *p = (struct ent *) e->e.o.right;

			if (p)
				if (e->e.o.lrflag & RFIXED)
					ret->e.o.right = (struct enode *) lookat (
						p->row, p->col);
				else
					ret->e.o.right = (struct enode *) lookat (
						p->row + rd, p->col + cd);
			else
				ret->e.o.right = 0;
		} else
# endif
		ret->e.o.right = copye (e->e.o.right, rd, cd);
		break;
	}
	return (ret);
}

/*
 * This structure is used to keep ent structs around before they
 * are deleted to allow the sync_refs routine a chance to fix the
 * variable references.
 * We also use it as a last-deleted buffer for the 'p' command.
 */

free_ent (p)
register struct ent *p;
{
	p->next = to_fix;
	to_fix = p;
	p->flags |= is_deleted;
}

flush_saved()
{
	register struct ent *p, *q;

	if (! (p = to_fix))
		return;
	while (p) {
		int oldcha = changed;

		clearent (p);
		changed = oldcha;
		q = p->next;
		mfree ((char *) p, sizeof (struct ent));
		p = q;
	}
	to_fix = 0;
}

copy1 (p, n, shiftrow, shiftcol)
register struct ent *p, *n;
{
	n->flags = p->flags | is_changed;
	n->prec = p->prec;
	n->d.value = p->d.value;
	if (p->flags & is_value)
		n->p.expr = copye (p->p.expr, shiftrow, shiftcol);
	else if (p->p.label) {
		n->p.label = mallo ((unsigned) (strlen (p->p.label) + 1));
		strcpy (n->p.label, p->p.label);
	}
}

newheight (h)
register h;
{
	register i, j;

	for (i=h; i<MAXROWS; ++i)
		mfree ((char *) tbl [i], sizeof (struct ent *) * MAXCOLS);
	tbl = (struct ent ***) reallo ((char *) tbl,
		sizeof (struct ent **) * MAXROWS,
		sizeof (struct ent **) * h);
	for (i=MAXROWS; i<h; ++i) {
		tbl [i] = (struct ent **) mallo (sizeof (struct ent *) * MAXCOLS);
		for (j=0; j<MAXCOLS; ++j)
			tbl[i][j] = 0;
	}
	hiddnrow = reallo (hiddnrow, (unsigned) MAXROWS, (unsigned) h);
	for (i=MAXROWS; i<h; ++i)
		hiddnrow [i] = 0;
	MAXROWS = h;
	if (currow >= MAXROWS)
		currow = MAXROWS-1;
	if (savedrow >= MAXROWS)
		savedrow = savedcol = 0;
	++FullUpdate;
}

newwidth (w)
register w;
{
	register i, j;

	for (i=0; i<MAXROWS; ++i) {
		tbl [i] = (struct ent **) reallo ((char *) tbl [i],
			sizeof (struct ent *) * MAXCOLS,
			sizeof (struct ent *) * w);
		for (j=MAXCOLS; j<w; ++j)
			tbl[i][j] = 0;
	}
	fwidth = reallo (fwidth, (unsigned) MAXCOLS, (unsigned) w);
	hiddncol = reallo (hiddncol, (unsigned) MAXCOLS, (unsigned) w);
	for (i=MAXCOLS; i<w; ++i) {
		fwidth [i] = DEFWIDTH;
		hiddncol [i] = 0;
	}
	MAXCOLS = w;
	if (curcol >= MAXCOLS)
		curcol = MAXCOLS-1;
	if (savedcol >= MAXCOLS)
		savedrow = savedcol = 0;
	++FullUpdate;
}

expandheight ()
{
	register max;

	max = maxheight ();
	/* "Current height is %d, legal range is %d-%d." */
	message (MSG (56), MAXROWS, maxrow+1, max);
	/* "Enter new table height: " */
	getnumber (MSG (57), maxrow+1, max);
	clrerr ();
	newheight (getnum);
}

expandwidth ()
{
	register max;

	max = maxwidth ();
	/* "Current width is %d, legal range is %d-%d." */
	message (MSG (58), MAXCOLS, maxcol+1, max);
	/* "Enter new table width: " */
	getnumber (MSG (59), maxcol+1, max);
	clrerr ();
	newwidth (getnum);
}

# define ADJUST(a, b, c) { if (a >= b) a += c; if (a < 0) a = 0; }

shiftref (r, nr, c, nc)
{
	if (nr) {
		ADJUST (savedrow, r, nr);
		ADJUST (graph.axisA.minr, r, nr);
		ADJUST (graph.axisA.maxr, r, nr);
		ADJUST (graph.axisB.minr, r, nr);
		ADJUST (graph.axisB.maxr, r, nr);
		ADJUST (graph.axisC.minr, r, nr);
		ADJUST (graph.axisC.maxr, r, nr);
		ADJUST (graph.axisD.minr, r, nr);
		ADJUST (graph.axisD.maxr, r, nr);
	}
	if (nc) {
		ADJUST (savedcol, c, nc);
		ADJUST (graph.axisA.minc, c, nc);
		ADJUST (graph.axisA.maxc, c, nc);
		ADJUST (graph.axisB.minc, c, nc);
		ADJUST (graph.axisB.maxc, c, nc);
		ADJUST (graph.axisC.minc, c, nc);
		ADJUST (graph.axisC.maxc, c, nc);
		ADJUST (graph.axisD.minc, c, nc);
		ADJUST (graph.axisD.maxc, c, nc);
	}
}
