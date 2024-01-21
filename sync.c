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

/*
 * sync_refs and syncref are used to remove references to
 * deleted struct ents.  Note that the deleted structure must still
 * be hanging around before the call, but not referenced by an entry
 * in tbl. Thus the free_ent, fix_ent calls in main.c
 */

syncrefs ()
{
	register i, j;
	register struct ent *p;

	for (i=0; i<=maxrow; i++)
		for (j=0; j<=maxcol; j++)
			if ((p = tbl[i][j]) && (p->flags & is_value) && p->p.expr)
				sync1ref (p->p.expr);
}

static sync1ref (e)
register struct enode *e;
{
	register struct ent *p;

	if (! e)
		return;
	switch (e->op) {
	case VAR:
		if (e->e.v) {
			e->e.v = lookat (e->e.v->row, e->e.v->col);
			e->e.v->flags |= is_changed;
		}
	case CONST:
		break;
	case RADD:
	case RMUL:
	case RSUB:
	case RDIV:
		if (p = (struct ent *) e->e.o.right) {
			p = lookat (p->row, p->col);
			e->e.o.right = (struct enode *) p;
		}
		if (p = (struct ent *) e->e.o.left) {
			p = lookat (p->row, p->col);
			e->e.o.left = (struct enode *) p;
			p->flags |= is_changed;
		}
		break;
	default:
# ifdef OPTIM
		if (e->e.o.lrflag & LVAR) {
			if (p = (struct ent *) e->e.o.left) {
				p = lookat (p->row, p->col);
				e->e.o.left = (struct enode *) p;
				p->flags |= is_changed;
			}
		} else
# endif
		sync1ref (e->e.o.left);
# ifdef OPTIM
		if (e->e.o.lrflag & RVAR) {
			if (p = (struct ent *) e->e.o.right) {
				p = lookat (p->row, p->col);
				e->e.o.right = (struct enode *) p;
				p->flags |= is_changed;
			}
		} else
# endif
		sync1ref (e->e.o.right);
		break;
	}
}

unrefcol ()
{
	register r, c;
	register struct ent *p;

	for (r=0; r<=maxrow; r++)
		for (c=0; c<=maxcol; c++)
			if ((p = tbl[r][c]) && (p->flags & is_value) &&
			    p->p.expr && unref1col (p->p.expr))
				p->flags |= is_changed;
}

unrefrow ()
{
	register r, c;
	register struct ent *p;

	for (r=0; r<=maxrow; r++)
		for (c=0; c<=maxcol; c++)
			if((p = tbl[r][c]) && (p->flags & is_value) &&
			    p->p.expr && unref1row (p->p.expr))
				p->flags |= is_changed;
}

static unref1col (e)
register struct enode *e;
{
	register struct ent *l, *r;
	register ret = 0;

	if (! e)
		return (0);
	switch (e->op) {
	default:
# ifdef OPTIM
		if (e->e.o.lrflag & LVAR) {
			l = (struct ent *) e->e.o.left;
			if (l && (l->flags & is_deleted))
				e->e.o.left = 0;
			ret = 1;
		} else
# endif
		ret |= unref1col (e->e.o.left);
# ifdef OPTIM
		if (e->e.o.lrflag & RVAR) {
			r = (struct ent *) e->e.o.right;
			if (r && (r->flags & is_deleted))
				e->e.o.right = 0;
			ret = 1;
		} else
# endif
		ret |= unref1col (e->e.o.right);
		break;
	case VAR:
		if (e->e.v && (e->e.v->flags & is_deleted))
			e->e.v = 0;
		ret = 1;
	case CONST:
		break;
	case RADD:
	case RMUL:
	case RSUB:
	case RDIV:
		l = (struct ent *) e->e.o.left;
		if (! l) break;
		r = (struct ent *) e->e.o.right;
		if (r->flags & is_deleted)
			if (l->flags & is_deleted)
				e->e.o.right = e->e.o.left = 0;
			else
				e->e.o.right = (struct enode *) lookat (r->row, curcol-1);
		else if (l->flags & is_deleted)
			e->e.o.left = (struct enode *) lookat (l->row, curcol);
		ret = 1;
		break;
	}
	return (ret);
}

static unref1row (e)
register struct enode *e;
{
	register struct ent *l, *r;
	register ret = 0;

	if (! e)
		return (0);
	switch (e->op) {
	default:
# ifdef OPTIM
		if (e->e.o.lrflag & LVAR) {
			l = (struct ent *) e->e.o.left;
			if (l && (l->flags & is_deleted))
				e->e.o.left = 0;
			ret = 1;
		} else
# endif
		ret |= unref1row (e->e.o.left);
# ifdef OPTIM
		if (e->e.o.lrflag & RVAR) {
			l = (struct ent *) e->e.o.right;
			if (l && (l->flags & is_deleted))
				e->e.o.right = 0;
			ret = 1;
		} else
# endif
		ret |= unref1row (e->e.o.right);
		break;
	case VAR:
		if (e->e.v && (e->e.v->flags & is_deleted))
			e->e.v = 0;
		ret = 1;
	case CONST:
		break;
	case RADD:
	case RMUL:
	case RSUB:
	case RDIV:
		l = (struct ent *) e->e.o.left;
		if (! l) break;
		r = (struct ent *) e->e.o.right;
		if (r->flags & is_deleted)
			if (l->flags & is_deleted)
				e->e.o.right = e->e.o.left = 0;
			else
				e->e.o.right = (struct enode *) lookat (currow-1, r->col);
		else if (l->flags & is_deleted)
			e->e.o.left = (struct enode *) lookat (currow, l->col);
		ret = 1;
		break;
	}
	return (ret);
}
