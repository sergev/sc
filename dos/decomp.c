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

char *dline;
int dlim;

extern char *mallo (), *strcpy (), *dtoag (), *sprint ();

static decodev (v)
register struct ent *v;
{
	if (v)
		sprint (dline + dlim, "%s%d", coltoa (v->col), v->row);
	else
		sprint (dline + dlim, BADREFERENCE);
	dlim += strlen (dline + dlim);
}

char *coltoa (col)
register col;
{
	/* convert column number to text representation, like a, zz, etc */
	/* returns pointer to static area */
	/* two consequant calls allowed: program switches between two */
	/* static arrays */

	static char rname1 [3], rname2 [3];
	register char *p, *b;
	static trigger;

	b = p = trigger ? rname1 : rname2;      /* select one of two buffers */
	trigger ^= 1;                           /* turn switch */
	if (col > 25) {
		*p++ = col / 26 + 'a' - 1;
		col %= 26;
	}
	*p++ = col + 'a';
	*p = 0;
	return (b);
}

static decomp (e, c, priority)
register struct enode *e;
{
	switch (c) {
	case 'l':
# ifdef OPTIM
		if (e->e.o.lrflag & LFIXED)
			dline [dlim++] = '$';
		if (e->e.o.lrflag & LVAR)
			decodev ((struct ent *) e->e.o.left);
		else
# endif
		decompile (e->e.o.left, priority);
		break;
	case 'r':
# ifdef OPTIM
		if (e->e.o.lrflag & RFIXED)
			dline [dlim++] = '$';
		if (e->e.o.lrflag & RVAR)
			decodev ((struct ent *) e->e.o.right);
		else
# endif
		decompile (e->e.o.right, priority);
		break;
	}
}

decompile (e, priority)
register struct enode *e;
{
	register char *s;
	int mypriority;

	if (! e) {
		dline[dlim++] = '?';
		return;
	}
	switch (e->op) {
	case LT:        case EQ:        case GT:        case LE:
	case NE:
	case GE:        mypriority = 6;         break;
	case ADD:
	case SUB:       mypriority = 8;         break;
	case MUL:
	case DIV:       mypriority = 10;        break;
	case QUEST:     mypriority = 1;         break;
	case COLON:     mypriority = 2;         break;
	case OR:        mypriority = 3;         break;
	case AND:       mypriority = 4;         break;
	case POW:       mypriority = 12;        break;
	default:        mypriority = 99;        break;
	}
	if (mypriority < priority)
		dline[dlim++] = '(';
	switch (e->op) {
	case VAR:
		decodev (e->e.v);
		break;
	case CONST:
		strcpy (dline + dlim, dtoag (e->e.k, 15));
		dlim += strlen (dline + dlim);
		break;
	case RADD:
		decodev ((struct ent *) e->e.o.left);
		dline[dlim++] = '+';
		dline[dlim++] = '+';
		decodev ((struct ent *) e->e.o.right);
		break;
	case RMUL:
		decodev ((struct ent *) e->e.o.left);
		dline[dlim++] = '*';
		dline[dlim++] = '*';
		decodev ((struct ent *) e->e.o.right);
		break;
	case RDIV:
		decodev ((struct ent *) e->e.o.left);
		dline[dlim++] = '/';
		dline[dlim++] = '/';
		decodev ((struct ent *) e->e.o.right);
		break;
	case RSUB:
		decodev ((struct ent *) e->e.o.left);
		dline[dlim++] = '-';
		dline[dlim++] = '-';
		decodev ((struct ent *) e->e.o.right);
		break;
	case FIXED:
		dline[dlim++] = '$';
		decomp (e, 'r', 30);
		break;
	case UMINUS:
		dline[dlim++] = '-';
		decomp (e, 'r', 30);
		break;
	case ROOT:
		dline[dlim++] = '~';
		decomp (e, 'r', 30);
		break;
	case NOT:
		dline[dlim++] = '!';
		decomp (e, 'r', 30);
		break;
	case SQUARE:
		decomp (e, 'r', 30);
		dline[dlim++] = '\'';
		break;
	case ADD:
		decomp (e, 'l', mypriority);
		dline[dlim++] = '+';
		decomp (e, 'r', mypriority + 1);
		break;
	case SUB:
		decomp (e, 'l', mypriority);
		dline[dlim++] = '-';
		decomp (e, 'r', mypriority + 1);
		break;
	case MUL:
		decomp (e, 'l', mypriority);
		dline[dlim++] = '*';
		decomp (e, 'r', mypriority + 1);
		break;
	case DIV:
		decomp (e, 'l', mypriority);
		dline[dlim++] = '/';
		decomp (e, 'r', mypriority + 1);
		break;
	case OR:
		decomp (e, 'l', mypriority);
		dline[dlim++] = '|';
		decomp (e, 'r', mypriority + 1);
		break;
	case AND:
		decomp (e, 'l', mypriority);
		dline[dlim++] = '&';
		decomp (e, 'r', mypriority + 1);
		break;
	case QUEST:
		decomp (e, 'l', mypriority);
		dline[dlim++] = '?';
		decomp (e, 'r', mypriority + 1);
		break;
	case COLON:
		decomp (e, 'l', mypriority);
		dline[dlim++] = ':';
		decomp (e, 'r', mypriority + 1);
		break;
	case EQ:
		decomp (e, 'l', mypriority);
		dline[dlim++] = '=';
		decomp (e, 'r', mypriority + 1);
		break;
	case NE:
		decomp (e, 'l', mypriority);
		dline[dlim++] = '!';
		dline[dlim++] = '=';
		decomp (e, 'r', mypriority + 1);
		break;
	case LT:
		decomp (e, 'l', mypriority);
		dline[dlim++] = '<';
		decomp (e, 'r', mypriority + 1);
		break;
	case GT:
		decomp (e, 'l', mypriority);
		dline[dlim++] = '>';
		decomp (e, 'r', mypriority + 1);
		break;
	case LE:
		decomp (e, 'l', mypriority);
		dline[dlim++] = '<';
		dline[dlim++] = '=';
		decomp (e, 'r', mypriority + 1);
		break;
	case GE:
		decomp (e, 'l', mypriority);
		dline[dlim++] = '>';
		dline[dlim++] = '=';
		decomp (e, 'r', mypriority + 1);
		break;
	case ACOS:      s = "acos(";    goto more1;
	case ASIN:      s = "asin(";    goto more1;
	case ATAN:      s = "atan(";    goto more1;
	case GAMMA:     s = "gamma(";   goto more1;
	case CEIL:      s = "ceil(";    goto more1;
	case COS:       s = "cos(";     goto more1;
	case EXP:       s = "exp(";     goto more1;
	case FABS:      s = "fabs(";    goto more1;
	case FLOOR:     s = "floor(";   goto more1;
	case LOG:       s = "ln(";      goto more1;
	case LOG10:     s = "log(";     goto more1;
	case SIN:       s = "sin(";     goto more1;
	case SQRT:      s = "sqrt(";    goto more1;
	case TAN:       s = "tan(";     goto more1;
	case DTR:       s = "dtr(";     goto more1;
	case RTD:       s = "rtd(";     goto more1;
more1:
		while (dline[dlim++] = *s++);
		dlim--;
		decomp (e, 'r', 0);
		dline[dlim++] = ')';
		break;
	case HYPOT:     s = "hypot(";   goto more2;
	case POW:       s = "pow(";     goto more2;
more2:
		while (dline[dlim++] = *s++);
		dlim--;
		decomp (e, 'l', 0);
		dline[dlim++] = ',';
		decomp (e, 'r', 0);
		dline[dlim++] = ')';
		break;
	case MIN:       s = "min(";     goto minmax;
	case MAX:       s = "max(";     goto minmax;
minmax:
		while (dline[dlim++] = *s++);
		dlim--;
		genlarg (e);
		dline[dlim++] = ',';
		decomp (e, 'r', 0);
		dline[dlim++] = ')';
		break;
	case INT:       s = "int(";     goto opt2;
	case ROUND:     s = "round(";   goto opt2;
opt2:
		while (dline[dlim++] = *s++);
		dlim--;
		decomp (e, 'l', 0);
		if (e->e.o.right) {
			dline[dlim++] = ',';
			decomp (e, 'r', 0);
		}
		dline[dlim++] = ')';
		break;
	}
	if (mypriority < priority)
		dline[dlim++] = ')';
}

static genlarg (e)
register struct enode *e;
{
	if (! (e->e.o.lrflag & LVAR) && e->op == e->e.o.left->op) {
		genlarg (e->e.o.left);
		dline[dlim++] = ',';
		decomp (e->e.o.left, 'r', 0);
	} else
		decomp (e, 'l', 0);
}

editexp (row, col)
{
	register struct ent *p;

	p = lookat (row, col);
	if (p->flags & is_value)
		if (p->p.expr) {
			/* old xenix-286 C-compiler falls down
			/* on things like "dline = line + linelim" */
			dline = line;
			dline += linelim;
			dlim = 0;
			decompile (p->p.expr, 0);
			linelim += dlim;
			line [linelim] = 0;
		} else {
			strcpy (line + linelim, dtoag (p->d.value, 15));
			linelim += strlen (line + linelim);
		}
}
