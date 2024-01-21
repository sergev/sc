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

# include <math.h>
# include "sc.h"

# define dtr(x) ((x)*(PI/180.0))
# define rtd(x) ((x)*(180.0/PI))

/* # define EVALDEBUG */

int EvalErr;
static struct ent *Stack;

extern double modf ();
static double evalvar ();

static double dosum (minr, minc, maxr, maxc)
{
	double			v;
	register		r, c;
	register struct ent*	p;

	v = 0;
	for(r = minr; r <= maxr; r++)
		for(c = minc; c <= maxc; c++)
			if ((p = tbl[r][c]) && (p->flags & is_value) && !(p->flags & is_error))
				v += p->d.value;
	return v;
}

static double doprod (minr, minc, maxr, maxc)
{
	double			v;
	register		r, c;
	register struct ent*	p;

	v = 1;
	for(r = minr; r <= maxr; r++)
		for(c = minc; c <= maxc; c++)
			if ((p = tbl[r][c]) && (p->flags & is_value) && !(p->flags & is_error))
				v = v * p->d.value;
	return v;
}

static double donumb (minr, minc, maxr, maxc)
{
	register		r, c, count;
	register struct ent*	p;

	count = 0;
	for(r = minr; r <= maxr; r++)
		for(c = minc; c <= maxc; c++)
			if ((p = tbl[r][c]) && (p->flags & is_value) && !(p->flags & is_error))
				count++;
	return (count);
}

static double doavg (minr, minc, maxr, maxc)
{
	double			v;
	register		r, c, count;
	register struct ent*	p;

	v = 0;
	count = 0;
	for(r = minr; r <= maxr; r++)
		for(c = minc; c <= maxc; c++)
			if ((p = tbl[r][c]) && (p->flags & is_value) && !(p->flags & is_error)) {
				v += p->d.value;
				count++;
			}

	if(count == 0) {
		EvalErr = 1;
		return ((double) 0);
	}
	return (v / (double)count);
}

static double ftrunc (x)
double x;
{
	double y;

	modf (x, &y);
	return (y);
}

static double fint (x, dn)
double x, dn;
{
	register n = dn;
	register double power = 1.0;

	if (n > 15)
		return (x);
	if (n < -15)
		return (0.0);
	if (! n)
		return (ftrunc (x));
	while (n > 0)
		power = power * 0.1, --n;
	while (n < 0)
		power = power * 10.0, ++n;
	return (ftrunc (x / power) * power);
}

static double round (x, dn)
double x, dn;
{
	register n = dn;
	register double power = 1.0;

	if (n > 15)
		return (x);
	if (n < -15)
		return (0.0);
	if (! n)
		return (ftrunc (x + 0.5));
	while (n > 0)
		power = power * 0.1, --n;
	while (n < 0)
		power = power * 10.0, ++n;
	return (ftrunc (x / power + 0.5) * power);
}

double eval (e)
register struct enode *e;
{
	register double a, b, c;

	if (!e || EvalErr)
		return (0.0);
	switch (e->op) {
	case CONST:     return (e->e.k);
	case VAR:       return (evalvar ((struct enode *) e->e.v));
	case RADD:
	case RMUL:
	case RSUB:
	case RDIV:
		{
			register maxr, maxc;
			register minr, minc;

			if (! e->e.o.right || ! e->e.o.left) {
				EvalErr = 1;
				return (0.0);
			}
			maxr = ((struct ent *) e->e.o.right) -> row;
			maxc = ((struct ent *) e->e.o.right) -> col;
			minr = ((struct ent *) e->e.o.left) -> row;
			minc = ((struct ent *) e->e.o.left) -> col;
			switch (e->op) {
			case RADD:
				return (dosum (minr, minc, maxr, maxc));
			case RMUL:
				return (doprod (minr, minc, maxr, maxc));
			case RDIV:
				return (donumb (minr, minc, maxr, maxc));
			case RSUB:
				return (doavg (minr, minc, maxr, maxc));
			}
		}
	case ROUND:     case INT:
		if (e->e.o.right)
			goto binop;
# ifdef OPTIM
		if (e->e.o.lrflag & LVAR)
			a = evalvar (e->e.o.left);
		else
# endif
		a = eval (e->e.o.left);
		if (EvalErr)
			return (0.0);
		b = 0.0;
		break;
	case QUEST:
# ifdef OPTIM
		if (e->e.o.lrflag & LVAR)
			c = evalvar (e->e.o.left);
		else
# endif
		c = eval (e->e.o.left);
		if (EvalErr)
			return (0.0);
		e = e->e.o.right;
	case ADD:       case SUB:       case MUL:       case DIV:
	case POW:       case LT:        case EQ:        case GT:
	case LE:        case NE:        case GE:        case AND:
	case OR:        case MAX:       case HYPOT:     case MIN:
binop:
# ifdef OPTIM
		if (e->e.o.lrflag & LVAR)
			a = evalvar (e->e.o.left);
		else
# endif
		a = eval (e->e.o.left);
		if (EvalErr)
			return (0.0);
	case FIXED:     case UMINUS:    case ROOT:      case NOT:
	case SQUARE:    case ACOS:      case ASIN:      case ATAN:
	case CEIL:      case COS:       case EXP:       case FABS:
	case FLOOR:     case LOG:       case LOG10:     case SIN:
	case SQRT:      case TAN:       case DTR:       case RTD:
# ifdef OPTIM
		if (e->e.o.lrflag & RVAR)
			b = evalvar (e->e.o.right);
		else
# endif
		b = eval (e->e.o.right);
		if (EvalErr)
			return (0.0);
	}
	switch (e->op) {
	case ADD:       return (a + b);
	case SUB:       return (a - b);
	case MUL:       return (a * b);
	case LT:        return (a < b);
	case EQ:        return (a == b);
	case GT:        return (a > b);
	case LE:        return (a <= b);
	case NE:        return (a != b);
	case GE:        return (a >= b);
	case AND:       return (a && b);
	case OR:        return (a || b);
	case COLON:     return (c ? a : b);
	case UMINUS:    return (- b);
	case FIXED:     return (b);
	case NOT:       return (! b);
	case SQUARE:    return (b * b);
	case ATAN:      return (atan (b));
	case CEIL:      return (ceil (b));
	case COS:       return (cos (b));
	case EXP:       return (exp (b));
	case FABS:      return (fabs (b));
	case FLOOR:     return (floor (b));
	case ROUND:     return (round (a, b));
	case INT:       return (fint (a, b));
	case HYPOT:     return (hypot (a, b));
	case TAN:       return (tan (b));
	case DTR:       return (dtr (b));
	case RTD:       return (rtd (b));
	case SIN:       return (sin (b));
	case MIN:       return (a<b ? a : b);
	case MAX:       return (a>b ? a : b);
	case DIV:
		if (b)
			return (a / b);
		break;
	case POW:
		if (a >= 0)
			return (pow (a, b));
		break;
	case ROOT:
		if (b >= 0)
			return (pow (b, 0.5));
		break;
	case ACOS:	 
		if (b >= -1 && b <= 1)
			return (acos (b));
		break;
	case ASIN:	 
		if (b >= -1 && b <= 1)
			return (asin (b));
		break;
	case LOG:
		if (b > 0)
			return (log (b));
		break;
	case LOG10:	 
		if (b > 0)
			return (log10 (b));
		break;
	case SQRT:
		if (b >= 0)
			return (sqrt (b));
		break;
	}
	EvalErr = 1;
	return ((double) 0.0);  /* safety net */
}

static delayrefs (e)
register struct enode *e;
{
	if (e) switch (e->op) {
	case VAR:       delayvar (e->e.v);
	case CONST:     return;
	case RADD:
	case RMUL:
	case RSUB:
	case RDIV:
		if (e->e.o.right && e->e.o.left)
			delayarea ((struct ent *) e->e.o.left,
				(struct ent *) e->e.o.right);
		break;
	case ROUND:     case INT:
		if (e->e.o.right)
			goto binop;
# ifdef OPTIM
		if (e->e.o.lrflag & LVAR)
			delayvar ((struct ent *) e->e.o.left);
		else
# endif
		delayrefs (e->e.o.left);
		break;
	case QUEST:
# ifdef OPTIM
		if (e->e.o.lrflag & LVAR)
			delayvar ((struct ent *) e->e.o.left);
		else
# endif
		delayrefs (e->e.o.left);
		e = e->e.o.right;
	case ADD:       case SUB:       case MUL:       case DIV:
	case POW:       case LT:        case EQ:        case GT:
	case LE:        case NE:        case GE:        case AND:
	case OR:        case MAX:       case HYPOT:     case MIN:
binop:
# ifdef OPTIM
		if (e->e.o.lrflag & LVAR)
			delayvar ((struct ent *) e->e.o.left);
		else
# endif
		delayrefs (e->e.o.left);
	case FIXED:     case UMINUS:    case ROOT:      case NOT:
	case SQUARE:    case ACOS:      case ASIN:      case ATAN:
	case CEIL:      case COS:       case EXP:       case FABS:
	case FLOOR:     case LOG:       case LOG10:     case SIN:
	case SQRT:      case TAN:       case DTR:       case RTD:
# ifdef OPTIM
		if (e->e.o.lrflag & RVAR)
			delayvar ((struct ent *) e->e.o.right);
		else
# endif
		delayrefs (e->e.o.right);
	}
}

static untag ()
{
	register i, j;
	register struct ent *p;

	for (i=0; i<=maxrow; i++)
		for (j=0; j<=maxcol; j++)
			if (p = tbl[i][j]) {
				if (p->flags & is_error)
					p->flags |= is_changed;
				if (! (p->flags & is_value) || p->p.expr)
					p->flags &= ~is_error;
				p->flags &= ~(is_locked | is_computed | is_delayed);
			}
}

EvalAll ()
{
	register i, j;
	register struct ent *p, *q;

	untag ();
	for (i=0; i<=maxrow; i++)
		for (j=0; j<=maxcol; j++) {
			if ((p = tbl[i][j]) && (p->flags & is_value) &&
			    p->p.expr && !(p->flags & is_computed)) {
				Stack = 0;
				queuecell (p);
				p->flags |= is_locked;
				delayrefs (p->p.expr);
				while (q = Stack) {
# ifdef EVALDEBUG
					printf ("pop %s%d\n", coltoa (q->col), q->row);
# endif
					Stack = Stack->next;
					if (Stack)
						Stack->d.prev = 0;
					if (q->flags & is_locked) {
						q->flags &= ~is_locked;
						q->flags |= is_computed;
						evalcell (q);
					} else {
						q->flags &= ~is_delayed;
						q->flags |= is_locked;
						queuecell (q);
						delayrefs (q->p.expr);
					}
				}
			}
		}
	maketable ();   /* update all changed cells */
	for (i=0; i<=maxrow; i++)
		for (j=0; j<=maxcol; j++)
			if (p = tbl[i][j])
				p->flags &= ~(is_computed | is_changed);
}

static double evalvar (e)
struct enode *e;
{
	register struct ent *p = (struct ent *) e;

	if (EvalErr || !p || !(p->flags & is_value) || (p->flags & is_error)) {
		EvalErr = 1;
		return (0.0);
	}
	return (p->d.value);
}

static queuecell (p)
register struct ent *p;
{
# ifdef EVALDEBUG
	printf ("queue %s%d\n", coltoa (p->col), p->row);
# endif
	p->d.prev = 0;
	if (Stack)
		Stack->d.prev = p;
	p->next = Stack;
	Stack = p;
}

static delayvar (p)
register struct ent *p;
{
	if (!p || !(p->flags & is_value) || !p->p.expr || (p->flags & is_error))
		return;
	switch (p->flags & (is_locked | is_computed | is_delayed)) {
	case is_delayed:	/* move p to the top of the Stack */
# ifdef EVALDEBUG
	printf ("delay delayed %s%d\n", coltoa (p->col), p->row);
# endif
		if (p == Stack)
			break;
		if (p->next)
			p->next->d.prev = p->d.prev;
		if (p->d.prev)
			p->d.prev->next = p->next;
	case 0:			/* put p on the Stack */
# ifdef EVALDEBUG
	printf ("delay 0 %s%d\n", coltoa (p->col), p->row);
# endif
		queuecell (p);
		p->flags |= is_delayed;
	case is_computed:	/* nothing to do; cell is already computed */
		break;
	case is_locked:		/* loop reference */
# ifdef EVALDEBUG
	printf ("delay locked %s%d\n", coltoa (p->col), p->row);
# endif
		p->flags |= is_error;
	}
}

static delayarea (left, right)
struct ent *left, *right;
{
	register r, c;
	register struct ent *p;
	register maxr, maxc, minr, minc;

	maxr = right->row;
	maxc = right->col;
	minr = left->row;
	minc = left->col;
	for (r=minr; r<=maxr; r++)
		for (c=minc; c<=maxc; c++)
			if (p = tbl[r][c])
				delayvar (p);
}

static evalcell (p)
register struct ent *p;
{
	register double v;

	if (p->flags & is_error)
		EvalErr = 1;
	else {
		EvalErr = 0;
		v = eval (p->p.expr);
	}
	if (EvalErr) {
		p->flags |= is_changed | is_error;
		p->d.value = 0.0;
	} else if (v != p->d.value) {
		p->flags |= is_changed;
		p->d.value = v;
	}
}
