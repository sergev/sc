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

# include "ctype.h"
# include "sc.h"

# define ENULL ((struct enode *) 0)

# define K_ACOS         1
# define K_ASIN         2
# define K_ATAN         3
# define K_AVG          4
# define K_CEIL         5
# define K_COS          6
# define K_DTR          7
# define K_E            8
# define K_EXP          9
# define K_FABS         10
# define K_FIXED        11
# define K_FLOOR        12
# define K_HYPOT        13
# define K_LN           14
# define K_LOG          15
# define K_MAX          16
# define K_MIN          17
# define K_PI           18
# define K_POW          19
# define K_PROD         20
# define K_INT          21
# define K_ROUND        22
# define K_RTD          23
# define K_SIN          24
# define K_SQRT         25
# define K_SUM          26
# define K_TAN          27
# define K_GAMMA        28
# define K_VAR          0101
# define K_FNUM         0102
# define K_LE           0103
# define K_GE           0104
# define K_NE           0105
# define K_PP           0106
# define K_MM           0107
# define K_SS           0110
# define K_DD           0111

static double fvalue;
static varrow, varcol;
static ungetflag, ungetlex;
static char *lptr;

extern EvalErr;                 /* defined in eval.c */

struct reserved {
	char *resname;
	int resval;
} restab [] = {
	"ACOS",         K_ACOS,
	"ASIN",         K_ASIN,
	"ATAN",         K_ATAN,
	"AVG",          K_AVG,
	"CEIL",         K_CEIL,
	"COS",          K_COS,
	"DTR",          K_DTR,
	"E",            K_E,
	"EXP",          K_EXP,
	"FABS",         K_FABS,
	"FIXED",        K_FIXED,
	"FLOOR",        K_FLOOR,
	"HYPOT",        K_HYPOT,
	"INT",          K_INT,
	"LN",           K_LN,
	"LOG",          K_LOG,
	"MAX",          K_MAX,
	"MIN",          K_MIN,
	"PI",           K_PI,
	"POW",          K_POW,
	"PROD",         K_PROD,
	"ROUND",        K_ROUND,
	"RTD",          K_RTD,
	"SIN",          K_SIN,
	"SQRT",         K_SQRT,
	"SUM",          K_SUM,
	"TAN",          K_TAN,
	"GAMMA",        K_GAMMA,
	0,              0,
};

static struct enode *getterm ();
static struct enode *getexpr (), *get0expr (), *get1expr (), *get2expr ();
static struct enode *get3expr (), *get4expr (), *get5expr (), *get6expr ();
static struct enode *optim ();
static struct enode *new (), *new_var (), *new_const ();
extern double atod ();
extern double eval ();                  /* defined in eval.c */
extern char *mallo (), *strcpy ();

static getname (s)
register char *s;
{
	register struct reserved *p;

	for (p=restab; p->resname; ++p)
		if (! strcmp (s, p->resname))
			return (p->resval);
	return (0);
}

static getvar (s)
register char *s;
{
	register c, r;

	if (*s < 'A' || *s > 'Z')
		return (0);
	c = *s++ - 'A';
	if (*s >= 'A' && *s <= 'Z')
		c = (c + 1) * 26 + *s++ - 'A';
	r = atoi (s);
	if (r<0 || r>16383)
		return (0);
	varrow = r;
	varcol = c;
	return (1);
}

static ungetcha (c)
{
	ungetlex = c;
	ungetflag = 1;
}

static getcha ()
{
	register c;
	char name [8];
	register char *p;

	if (ungetflag) {
		ungetflag = 0;
		return (ungetlex);
	}
loop:
	if (! (c = *lptr))
		return (-1);
	++lptr;
	switch (c) {
	default:
		if (! isalpha (c))
			return (c);
		for (p=name; isalpha (c); ++p) {
			if (p > &name [6])
				return (0);
			*p = islower (c) ? toupper (c) : c;
			c = *lptr++;
		}
		if (isdigit (c)) {
			for (; isdigit (c); ++p) {
				if (p > &name [6])
					return (0);
				*p = c;
				c = *lptr++;
			}
			*p = 0;
			--lptr;
			if (! getvar (name))
				return (0);
			return (K_VAR);
		}
		*p = 0;
		--lptr;
		return (getname (name));
	case '\n':
		return (-1);
	case ' ':
	case '\t':
		goto loop;
	case '!':
		if (*lptr == '=') {
			++lptr;
			return (K_NE);
		}
		return (c);
	case '>':
		if (*lptr == '=') {
			++lptr;
			return (K_GE);
		}
		return (c);
	case '<':
		if (*lptr == '=') {
			++lptr;
			return (K_LE);
		}
		return (c);
	case '+':
		if (*lptr == '+') {
			++lptr;
			return (K_PP);
		}
		return (c);
	case '-':
		if (*lptr == '-') {
			++lptr;
			return (K_MM);
		}
		return (c);
	case '*':
		if (*lptr == '*') {
			++lptr;
			return (K_SS);
		}
		return (c);
	case '/':
		if (*lptr == '/') {
			++lptr;
			return (K_DD);
		}
		return (c);
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	case '.':
		fvalue = atod (lptr-1);
		while (isdigit (c))
			c = *lptr++;
		if (c == '.')
			c = *lptr++;
		while (isdigit (c))
			c = *lptr++;
		if (c=='e' || c=='E') {
			c = *lptr++;
			if (c=='+' || c=='-')
				c = *lptr++;
			while (isdigit (c))
				c = *lptr++;
		}
		--lptr;
		return (K_FNUM);
	}
}

static struct enode *getterm ()
{
	register struct enode *l, *r, *ret;
	register c, f;
	struct ent *lref;

	switch (getcha ()) {
	default:
		return (0);
	case K_ACOS:
		c = ACOS;
fun1:           if (getcha () != '(' || ! (l = get0expr ()))
			return (0);
		if (getcha () != ')') {
freeleft:               efree (l);
			return (0);
		}
		ret = new (c, ENULL, l);
		break;
	case K_ASIN:    c = ASIN;       goto fun1;
	case K_ATAN:    c = ATAN;       goto fun1;
	case K_CEIL:    c = CEIL;       goto fun1;
	case K_COS:     c = COS;        goto fun1;
	case K_EXP:     c = EXP;        goto fun1;
	case K_FABS:    c = FABS;       goto fun1;
	case K_FLOOR:   c = FLOOR;      goto fun1;
	case K_LN:      c = LOG;        goto fun1;
	case K_LOG:     c = LOG10;      goto fun1;
	case K_SIN:     c = SIN;        goto fun1;
	case K_SQRT:    c = SQRT;       goto fun1;
	case K_TAN:     c = TAN;        goto fun1;
	case K_GAMMA:   c = GAMMA;      goto fun1;
	case K_DTR:     c = DTR;        goto fun1;
	case K_RTD:     c = RTD;        goto fun1;
	case K_ROUND:
		c = ROUND;
opt2:           if (getcha () != '(' || ! (l = get0expr ()))
			return (0);
		if ((f = getcha ()) == ',') {
			if (! (r = get0expr ()))
				goto freeleft;
			if (getcha () != ')') {
				efree (r);
				goto freeleft;
			}
		} else if (f == ')')
			r = 0;
		else
			goto freeleft;
		ret = new (c, l, r);
		break;
	case K_INT:     c = INT;        goto opt2;
	case K_HYPOT:
		c = HYPOT;
fun2:           if (getcha () != '(' || ! (l = get0expr ()))
			return (0);
		if (getcha () != ',' || ! (r = get0expr ()))
			goto freeleft;
		if (getcha () != ')') {
			efree (r);
			goto freeleft;
		}
		ret = new (c, l, r);
		break;
	case K_POW:     c = POW;        goto fun2;
	case K_MAX:     f = MAX;        goto maxmin;
	case K_MIN:     f = MIN;
maxmin:         if (getcha () != '(' || ! (l = get0expr ()))
			return (0);
		while ((c = getcha ()) == ',') {
			if (! (r = get0expr ()))
				goto freeleft;
			l = new (f, l, r);
		}
		if (c != ')')
			goto freeleft;
		ret = l;
		break;
	case '+':
		return (getterm ());
	case '$':
		c = FIXED;
unop:           if (! (l = getterm ()))
			return (0);
		return (new (c, ENULL, l));
	case '~':       c = ROOT;       goto unop;
	case '!':       c = NOT;        goto unop;
	case '-':       c = UMINUS;     goto unop;
	case '(':
		if (! (l = get0expr ()))
			return (0);
		if (getcha () != ')')
			goto freeleft;
		ret = l;
		break;
	case K_FNUM:
fcon:           ret = new_const (CONST, fvalue);
		break;
	case K_PI:      fvalue = PI;    goto fcon;
	case K_E:       fvalue = E;     goto fcon;
	case K_VAR:
		lref = lookat (varrow, varcol);
		switch (c = getcha ()) {
			int minc, minr, maxc, maxr;
		default:
			ungetcha (c);
			ret = new_var (VAR, lref);
			break;
		case K_PP:
			c = RADD;
arop:
			minc = varcol;
			minr = varrow;
			if (getcha () != K_VAR)
				goto freeleft;
			if (varcol < minc)
				maxc = minc, minc = varcol;
			else
				maxc = varcol;
			if (varrow < minr)
				maxr = minr, minr = varrow;
			else
				maxr = varrow;
			ret = new (c,
				(struct enode *) lookat (minr, minc),
				(struct enode *) lookat (maxr, maxc));
			break;
		case K_SS:      c = RMUL;       goto arop;
		case K_MM:      c = RSUB;       goto arop;
		case K_DD:      c = RDIV;       goto arop;
		}
		break;
	}
	while ((c = getcha ()) == '\'')
		ret = new (SQUARE, ENULL, ret);
	ungetcha (c);
	return (ret);
}

static struct enode *get6expr ()
{
	register struct enode *l, *r;
	register c;

	if (! (l = getterm ()))
		return (0);
loop:   switch (c = getcha ()) {
	default:
		ungetcha (c);
	case -1:
		return (l);
	case '^':
		c = POW;
		if (! (r = getterm ())) {
			efree (l);
			return (0);
		}
		l = new (c, l, r);
		goto loop;
	}
}

static struct enode *get5expr ()
{
	register struct enode *l, *r;
	register c;

	if (! (l = get6expr ()))
		return (0);
loop:   switch (c = getcha ()) {
	default:
		ungetcha (c);
	case -1:
		return (l);
	case '*':       c = MUL;        goto binop;
	case '/':       c = DIV;
binop:          if (! (r = get6expr ())) {
			efree (l);
			return (0);
		}
		l = new (c, l, r);
		goto loop;
	}
}

static struct enode *get4expr ()
{
	register struct enode *l, *r;
	register c;

	if (! (l = get5expr ()))
		return (0);
loop:   switch (c = getcha ()) {
	default:
		ungetcha (c);
	case -1:
		return (l);
	case '+':       c = ADD;        goto binop;
	case '-':       c = SUB;
binop:          if (! (r = get5expr ())) {
			efree (l);
			return (0);
		}
		l = new (c, l, r);
		goto loop;
	}
}

static struct enode *get3expr ()
{
	register struct enode *l, *r;
	register c;

	if (! (l = get4expr ()))
		return (0);
loop:   switch (c = getcha ()) {
	default:
		ungetcha (c);
	case -1:
		return (l);
	case '&':
		c = AND;
		if (! (r = get4expr ())) {
			efree (l);
			return (0);
		}
		l = new (c, l, r);
		goto loop;
	}
}

static struct enode *get2expr ()
{
	register struct enode *l, *r;
	register c;

	if (! (l = get3expr ()))
		return (0);
loop:   switch (c = getcha ()) {
	default:
		ungetcha (c);
	case -1:
		return (l);
	case '|':
		c = OR;
		if (! (r = get3expr ())) {
			efree (l);
			return (0);
		}
		l = new (c, l, r);
		goto loop;
	}
}

static struct enode *get1expr ()
{
	register struct enode *l, *r;
	register c;

	if (! (l = get2expr ()))
		return (0);
loop:   switch (c = getcha ()) {
	default:
		ungetcha (c);
	case -1:
		return (l);
	case '=':       c = EQ;         goto binop;
	case '>':       c = GT;         goto binop;
	case '<':       c = LT;         goto binop;
	case K_GE:      c = GE;         goto binop;
	case K_LE:      c = LE;         goto binop;
	case K_NE:      c = NE;
binop:          if (! (r = get2expr ())) {
			efree (l);
			return (0);
		}
		l = new (c, l, r);
		goto loop;
	}
}

static struct enode *get0expr ()
{
	register struct enode *l, *r, *r2;
	register c;

	if (! (l = get1expr ()))
		return (0);
	switch (c = getcha ()) {
	default:
		ungetcha (c);
	case -1:
		return (l);
	case '?':
		if (! (r = get0expr ()))
			goto freeleft;
		if (getcha () != ':' || ! (r2 = get0expr ())) {
			efree (r);
freeleft:               efree (l);
			return (0);
		}
		return (new (QUEST, l, new (COLON, r, r2)));
	}
}

static isterm ()
{
	register c;

	switch (getcha ()) {
	default:
		return (0);
	case K_ASIN:    case K_ATAN:    case K_CEIL:    case K_COS:
	case K_EXP:     case K_FABS:    case K_FLOOR:   case K_LN:
	case K_LOG:     case K_SIN:     case K_SQRT:    case K_TAN:
	case K_DTR:     case K_RTD:     case K_ACOS:    case K_GAMMA:
		if (getcha () != '(' || ! is0expr () || getcha () != ')')
			return (0);
		break;
	case K_MAX:     case K_MIN:
		if (getcha () != '(' || ! is0expr ())
			return (0);
		while ((c = getcha ()) == ',')
			if (! is0expr ())
				return (0);
		return (c == ')');
	case K_HYPOT:   case K_POW:
		if (getcha () != '(' || ! is0expr ())
			return (0);
		if (getcha () != ',' || ! is0expr () || getcha () != ')')
			return (0);
		break;
	case K_ROUND:   case K_INT:
		if (getcha () != '(' || ! is0expr ())
			return (0);
		if ((c = getcha ()) == ',') {
			if (! is0expr () || getcha () != ')')
				return (0);
		} else if (c != ')')
			return (0);
		break;
	case '+':
	case '~':       case '!':       case '-':       case '$':
		return (isterm ());
	case '(':
		if (! is0expr () || getcha () != ')')
			return (0);
		break;
	case K_FNUM:    case K_PI:      case K_E:
		break;
	case K_VAR:
		switch (c = getcha ()) {
		default:
			ungetcha (c);
			break;
		case K_SS:      case K_MM:      case K_DD:      case K_PP:
			if (getcha () != K_VAR)
				return (0);
			break;
		}
		break;
	}
	while ((c = getcha ()) == '\'');
	ungetcha (c);
	return (1);
}

is1expr ()
{
	register c;

	if (! isterm ())
		return (0);
	switch (c = getcha ()) {
	default:
		ungetcha (c);
	case -1:
		return (1);
	case '+':       case '-':       case '*':       case '/':
	case '^':       case '=':       case '&':       case '|':
	case '>':       case '<':       case K_GE:      case K_LE:
	case K_NE:
		return (is1expr ());
	}
}

is0expr ()
{
	register c;

	if (! is1expr ())
		return (0);
	switch (c = getcha ()) {
	default:
		ungetcha (c);
	case -1:
		return (1);
	case '?':
		return (is0expr () && getcha () == ':' && is0expr ());
	}
}

isexpr (s)
char *s;
{
	ungetflag = 0;
	lptr = s;
	return (is0expr () && getcha () == -1);
}

static struct enode *getexpr (s)
char *s;
{
	ungetflag = 0;
	lptr = s;
	return (get0expr ());
}

parse (cell, s)
register struct ent *cell;
register char *s;
{
	if (cell) switch (*s) {
	case '=':
		switch (s [1]) {
		default:
			if (isexpr (s+1))
				let (cell, getexpr (s+1), 0);
			else
				label (cell, s, 0);
			break;
		case '<':
			if (isexpr (s+2))
				let (cell, getexpr (s+2), is_leftflush);
			else
				label (cell, s, 0);
			break;
		case '>':
			if (isexpr (s+2))
				let (cell, getexpr (s+2), is_rightflush);
			else
				label (cell, s, 0);
			break;
		case '^':
			if (isexpr (s+2))
				let (cell, getexpr (s+2), is_center);
			else
				label (cell, s, 0);
			break;
		}
		break;
	default:
		if (isexpr (s))
			let (cell, getexpr (s), 0);
		else
			label (cell, s, 0);
		break;
	case '"':
		label (cell, s+1, 0);
		break;
	case '^':
		label (cell, s+1, is_center);
		break;
	case '<':
		label (cell, s+1, is_leftflush);
		break;
	case '>':
		label (cell, s+1, is_rightflush);
		break;
	}
}

static struct enode *new (op, a1, a2)
struct enode *a1;
struct enode *a2;
{
	register struct enode *p = (struct enode *) mallo ((unsigned) sizeof (struct enode));

	p->op = op;
	p->e.o.left = a1;
	p->e.o.right = a2;
	return (p);
}

static struct enode *new_var (op, a1)
struct ent *a1;
{
	register struct enode *p = (struct enode *) mallo ((unsigned) sizeof (struct enode));

	p->op = op;
	p->e.v = a1;
	return (p);
}

static struct enode *new_const (op, a1)
double a1;
{
	register struct enode *p = (struct enode*) mallo ((unsigned) sizeof (struct enode));

	p->op = op;
	p->e.k = a1;
	return (p);
}

static label (v, s, flush)
register struct ent *v;
register char *s;
{
	if (v) {
		clearent (v);
		if (s && s[0]) {
			v->p.label = mallo ((unsigned) strlen (s) + 1);
			strcpy (v->p.label, s);
		} else
			v->p.label = 0;
		v->flags &= ~(is_rightflush | is_leftflush);
		v->flags |= flush;
		++FullUpdate;
		++modflg;
	}
}

static let (v, e, flush)
register struct ent *v;
register struct enode *e;
{
	register f;

	f = v->flags & ~(is_value | is_error);
	if (flush) {
		f &= ~(is_leftflush | is_rightflush);
		f |= flush;
	}
	clearent (v);
	v->flags = f | is_changed | is_value;
	if (constant (e)) {
		EvalErr = 0;
		v->d.value = eval (e);
		if (EvalErr)
			v->flags |= is_error;
		v->p.expr = 0;
		efree (e);
	} else
# ifdef OPTIM
		v->p.expr = optim (e);
# else
		v->p.expr = e;
# endif
}

# ifdef OPTIM
static struct enode *optim (e)
register struct enode *e;
{
	register struct enode *p;

	if (! e)
		return (0);
	switch (e->op) {
	case CONST:     case VAR:
	case RADD:      case RMUL:      case RSUB:      case RDIV:
		return (e);
	}
	e->e.o.lrflag = 0;
	switch (e->op) {
	case ADD:       case SUB:       case MUL:       case DIV:
	case POW:       case LT:        case EQ:        case GT:
	case LE:        case NE:        case GE:        case AND:
	case OR:        case ROUND:     case HYPOT:     case MIN:
	case MAX:       case QUEST:     case COLON:     case INT:
		if (! e->e.o.left)
			goto r;
		while (e->e.o.left->op == FIXED) {
			p = e->e.o.left;
			e->e.o.left = e->e.o.left->e.o.right;
			e->e.o.lrflag |= LFIXED;
			mfree ((char *) p, sizeof (struct enode));
		}
		if (e->e.o.left->op == VAR) {
			p = e->e.o.left;
			e->e.o.left = (struct enode *) e->e.o.left->e.v;
			e->e.o.lrflag |= LVAR;
			mfree ((char *) p, sizeof (struct enode));
		} else
			e->e.o.left = optim (e->e.o.left);
	case UMINUS:    case ROOT:      case NOT:       case FIXED:
	case SQUARE:    case ACOS:      case ASIN:      case ATAN:
	case CEIL:      case COS:       case EXP:       case FABS:
	case FLOOR:     case LOG:       case LOG10:     case SIN:
	case SQRT:      case TAN:       case DTR:       case RTD:
	case GAMMA:
r:              if (! e->e.o.right)
			break;
		while (e->e.o.right->op == FIXED) {
			p = e->e.o.right;
			e->e.o.right = e->e.o.right->e.o.right;
			e->e.o.lrflag |= RFIXED;
			mfree ((char *) p, sizeof (struct enode));
		}
		if (e->e.o.right->op == VAR) {
			p = e->e.o.right;
			e->e.o.right = (struct enode *) e->e.o.right->e.v;
			e->e.o.lrflag |= RVAR;
			mfree ((char *) p, sizeof (struct enode));
		} else
			e->e.o.right = optim (e->e.o.right);
	}
	return (e);
}
# endif

static constant (e)
register struct enode *e;
{
	if (! e)
		return (1);
	switch (e->op) {
	case CONST:
		return (1);
	case VAR:
	case RADD:
	case RMUL:
	case RSUB:
	case RDIV:
		return (0);
	default:
# ifdef OPTIM
		if (e->e.o.lrflag & (LVAR|RVAR))
			return (0);
# endif
		return (constant (e->e.o.left) && constant (e->e.o.right));
	}
}
