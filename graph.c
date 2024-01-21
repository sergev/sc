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
# include "sc.h"
# include "graph.h"

struct graph graph;

# define MAXVAL 100

# ifdef __MSDOS__
# undef USEGPLOT
# endif

# ifndef USEGPLOT
# include "draw.h"
# undef label

FILE *OutPlot;
# ifdef __MSDOS__
FILE *OutHard;
# endif
static char *lp;
static char **names;
static num, dim;
static float *x, *a, *b, *c;

# ifdef __MSDOS__
static float stx [MAXVAL], sta [MAXVAL], stb [MAXVAL], stc [MAXVAL];
# endif

extern char *dtoaf (), *dtoag (), *strcpy (), *sprint ();

# ifdef __MSDOS__
hardcopy(fname)
char *fname;
{
	char lin [1024];
	char *stnames [MAXVAL];
# ifndef  __MSDOS__
	float stx [MAXVAL], sta [MAXVAL], stb [MAXVAL], stc [MAXVAL];
# endif

	if (! graph.axisA.valid || ! graph.axisB.valid)
		/* "Nothing to plot." */
		cerror (MSG (119));
	if (! (OutHard = fopen (fname, "wb")))
		/* "Cannot open %s" */
		cerror (MSG (62), fname);
	lp = lin;
	names = stnames;
	x = stx;
	a = sta;
	b = stb;
	c = stc;
	makeval ();
	g_hcopy();
	openpl ();
	draw ();
	closepl ();
	fclose (OutHard);
}
# endif

graphflag ()
{
	return (graph.shape == XYGRAPH);
}

plotfile (fname)
char *fname;
{
	char lin [1024];
	char *stnames [MAXVAL];
# ifndef  __MSDOS__
	float stx [MAXVAL], sta [MAXVAL], stb [MAXVAL], stc [MAXVAL];
# endif

	if( (OutPlot = fopen( fname, "wb")) == 0 )
		/* "Can't create %s" */
		cerror (MSG (31), fname);

	if (! graph.axisA.valid || ! graph.axisB.valid)
		/* "Nothing to plot." */
		cerror (MSG (119));
	lp = lin;
	names = stnames;
	x = stx;
	a = sta;
	b = stb;
	c = stc;
	makeval (graphflag ());
	g_plot();
	openpl ();
	draw ();
	closepl ();
	fclose (OutPlot);
}

# if defined (__MSDOS__) || defined (E85DEMOS) || defined (M_XENIX)
view ()
{
	char lin [1024];
	char *stnames [MAXVAL];
# ifndef __MSDOS__
	float stx [MAXVAL], sta [MAXVAL], stb [MAXVAL], stc [MAXVAL];
# endif

	if (! graph.axisA.valid || ! graph.axisB.valid)
		/* "Nothing to plot." */
		cerror (MSG (119));
	lp = lin;
	names = stnames;
	x = stx;
	a = sta;
	b = stb;
	c = stc;
	makeval (graphflag ());
	g_tty();
	openpl ();
	draw ();
# ifndef __MSDOS__
	fflush (stdout);
# endif
	KeyGet ();
	closepl ();
	VRedraw ();
}
# else
view()
{
}
# endif

draw ()
{
	switch (graph.shape) {
	case PIE:
		switch (dim) {
		case 1:
			g_piechart (num, names, a, "", 0);
			break;
		case 2:
			g_pie2chart (num, names, a, b, "", "", 0);
			break;
		case 3:
			g_pie3chart (num, names, a, b, c, "", "", "", 0);
			break;
		}
		break;
	case GRAPHIC:
		switch (dim) {
		case 1:
			g_graph (num, names, a, "", 0);
			break;
		case 2:
			g_2graph (num, names, a, b, "", "", 0);
			break;
		case 3:
			g_3graph (num, names, a, b, c, "", "", "", 0);
			break;
		}
		break;
	case XYGRAPH:
		switch (dim) {
		case 1:
			g_xygraph (num, x, a, "", 0);
			break;
		case 2:
			g_xy2graph (num, x, a, b, "", "", 0);
			break;
		case 3:
			g_xy3graph (num, x, a, b, c, "", "", "", 0);
			break;
		}
		break;
	case CHYSTO:
		switch (dim) {
		case 1:
			g_hysto (num, names, a, "", 0);
			break;
		case 2:
			g_2hysto (num, names, a, b, "", "", 0);
			break;
		case 3:
			g_3hysto (num, names, a, b, c, "", "", "", 0);
			break;
		}
		break;
	case HYSTO:
		switch (dim) {
		case 1:
			g_hysto (num, names, a, "", 0);
			break;
		case 2:
			g_c2hysto (num, names, a, b, "", "", 0);
			break;
		case 3:
			g_c3hysto (num, names, a, b, c, "", "", "", 0);
			break;
		}
		break;
	}
}

static char *printval (row, col)
{
	register struct ent *p;
	register char *s;
	register i;

	if (row<=maxrow && col<=maxcol && fwidth[col] && (p = tbl[row][col]) && ! (p->flags & is_error)) {
		if (! (p->flags & is_value))
			return (p->p.label);
		s = lp;
		strcpy (lp, dtoaf (p->d.value, p->prec));
		for (i=0; i<32; ++i)
			if (! *lp++)
				return (s);
		*lp++ = 0;
		return (s);
	} else
		return ("");
}

static float getval (row, col)
{
	register struct ent *p;

	if (row<=maxrow && col<=maxcol && (p = tbl[row][col]) &&
	    ! (p->flags & is_error) && (p->flags & is_value))
		return (p->d.value);
	return (0.0);
}

static makeval (xflag)
int xflag;			/* flag of numerical X axis */
{
	register r1, c1;
	register r2, c2;
	register r3, c3;
	register r4, c4;

	num = 0;
	r1 = graph.axisA.minr;
	c1 = graph.axisA.minc;
	r2 = graph.axisB.minr;
	c2 = graph.axisB.minc;
	dim = 1;
	if (graph.axisC.valid) {
		++dim;
		r3 = graph.axisC.minr;
		c3 = graph.axisC.minc;
		if (graph.axisD.valid) {
			++dim;
			r4 = graph.axisD.minr;
			c4 = graph.axisD.minc;
		}
	}
	for (;;) {
		if (c1 > graph.axisA.maxc)
			c1 = graph.axisA.minc, ++r1;
		if (c2 > graph.axisB.maxc)
			c2 = graph.axisB.minc, ++r2;
		if (graph.axisC.valid) {
			if (c3 > graph.axisC.maxc)
				c3 = graph.axisC.minc, ++r3;
			if (graph.axisD.valid) {
				if (c4 > graph.axisD.maxc)
					c4 = graph.axisD.minc, ++r4;
			}
		}
		if (num >= MAXVAL)
			return;
		if (r1 > graph.axisA.maxr)
			return;
		if (r2 > graph.axisB.maxr)
			return;
		if (graph.axisC.valid) {
			if (r3 > graph.axisC.maxr)
				return;
			if (graph.axisD.valid) {
				if (r4 > graph.axisD.maxr)
					return;
			}
		}
		if (xflag)
			x [num] = getval (r1, c1);
		else
			names [num] = printval (r1, c1);
		a [num] = getval (r2, c2);
		if (graph.axisC.valid) {
			b [num] = getval (r3, c3);
			if (graph.axisD.valid) {
				c [num] = getval (r4, c4);
			}
		}
		++num;
		++c1;
		++c2;
		if (graph.axisC.valid) {
			++c3;
			if (graph.axisD.valid)
				++c4;
		}
	}
}

# else /* USEGPLOT */

# define GPLOT  "/usr/local/gplot"
# define PLOT   "gplot"

static char *lp;
static out;

callsys (f, v)
char *f, **v;
{
	int t, status, pip [2];

	if (pipe (pip) == -1)
		/* "Cannot create pipe." */
		cerror (MSG (122));
	if ((t = fork ()) == -1)
		/* "Can't fork." */
		cerror (MSG (120));
	if (t == 0) {                           /* child */
		close (pip [1]);
		close (0);
		if (dup (pip [0]) != 0)
			exit (1);
		close (pip [0]);
		if (out >= 0) {
			close (1);
			if (dup (out) != 1)
				exit (1);
			close (out);
		}
		execv (f, v);
		exit (1);
	}
	close (pip [0]);                        /* parent */
	if (out >= 0)
		close (out);
	printdata (pip [1]);
	close (pip [1]);
	while (t != wait (&status));
	if (status & 0377)
		/* "Error in %s." */
		cerror (MSG (121), PLOT);
	return ((status >> 8) & 0377);
}

view ()
{
	char *argvect [20];

	out = -1;
	makeargs (argvect);
	VClear ();
	VSync ();
	if (callsys (GPLOT, argvect))
		/* "Error in %s." */
		cerror (MSG (121), PLOT);
	else {
		KeyGet ();
		VClear ();
	}
	++FullUpdate;
}

plotfile (name)
char *name;
{
	char *argvect [20];

	if ((out = creat (name, 0666)) < 0)
		/* "Can't create %s" */
		cerror (MSG (31), name);
	makeargs (argvect);
	if (callsys (GPLOT, argvect))
		/* "Error in %s." */
		cerror (MSG (121), PLOT);
}

static makeargs (ap)
register char **ap;
{
	if (! graph.axisA.valid || ! graph.axisB.valid)
		/* "Nothing to plot." */
		cerror (MSG (119));
	*ap++ = PLOT;
	switch (graph.shape) {
	case CHYSTO:
		*ap++ = "-c";
		break;
	case HYSTO:
		*ap++ = "-h";
		break;
	case GRAPHIC:
		*ap++ = "-g";
		break;
	case PIE:
		*ap++ = "-p";
		*ap++ = "-o";
		break;
	default:
		return;
	}
	*ap++ = "-M";
	*ap++ = 0;
}

printdata (d)
register d;
{
	register r1, c1;
	register r2, c2;
	register r3, c3;
	register r4, c4;
	char lin [100];

	r1 = graph.axisA.minr;
	c1 = graph.axisA.minc;
	r2 = graph.axisB.minr;
	c2 = graph.axisB.minc;
	if (graph.axisC.valid) {
		r3 = graph.axisC.minr;
		c3 = graph.axisC.minc;
		if (graph.axisD.valid) {
			r4 = graph.axisD.minr;
			c4 = graph.axisD.minc;
		}
	}
	for (;;) {
		if (c1 > graph.axisA.maxc)
			c1 = graph.axisA.minc, ++r1;
		if (c2 > graph.axisB.maxc)
			c2 = graph.axisB.minc, ++r2;
		if (graph.axisC.valid) {
			if (c3 > graph.axisC.maxc)
				c3 = graph.axisC.minc, ++r3;
			if (graph.axisD.valid) {
				if (c4 > graph.axisD.maxc)
					c4 = graph.axisD.minc, ++r4;
			}
		}
		if (r1 > graph.axisA.maxr)
			return;
		if (r2 > graph.axisB.maxr)
			return;
		if (graph.axisC.valid) {
			if (r3 > graph.axisC.maxr)
				return;
			if (graph.axisD.valid) {
				if (r4 > graph.axisD.maxr)
					return;
			}
		}
		lp = lin;
		printval (r1, c1);
		*lp++ = ' ';
		printval (r2, c2);
		if (graph.axisC.valid) {
			*lp++ = ' ';
			printval (r3, c3);
			if (graph.axisD.valid) {
				*lp++ = ' ';
				printval (r4, c4);
			}
		}
		*lp++ = '\n';
		if (write (d, lin, lp-lin) != lp-lin)
			return;
		++c1;
		++c2;
		if (graph.axisC.valid) {
			++c3;
			if (graph.axisD.valid)
				++c4;
		}
	}
}

static printval (r, c)
{
	register struct ent *p;

	if (r<=maxrow && c<=maxcol && (p = tbl[r][c]) &&
	    ! (p->flags & is_error) && (p->flags & is_value)) {
		strcpy (lp, dtoag (p->d.value, 15));
		while (*lp++);
		--lp;
	} else
		*lp++ = '0';
}
# endif /* USEGPLOT */

static char *txtshape ()
{
	switch (graph.shape) {
	case PIE:	return (MSG (124));
	case GRAPHIC:	return (MSG (125));
	case XYGRAPH:	return (MSG (80));
	case HYSTO:	return (MSG (126));
	case CHYSTO:	return (MSG (127));
	default:	return (0);
	}
}

msgshape ()
{
	/* "Graph type is '%s'." */
	message (MSG (132), txtshape ());
}

msggraph ()
{
	register char *s;
	char lin [80];

	s = lin;
	/* "Shape is '%s'" */
	sprint (s, MSG (115), txtshape ());
	for (; *s; ++s);
	if (graph.axisA.valid) {
		/* ", axes %s%d-%s%d" */
		sprint (s, MSG (116),
			coltoa (graph.axisA.minc), graph.axisA.minr,
			coltoa (graph.axisA.maxc), graph.axisA.maxr);
		for (; *s; ++s);
		if (graph.axisB.valid) {
			/* ", %s%d-%s%d" */
			sprint (s, ", %s%d-%s%d",
				coltoa (graph.axisB.minc), graph.axisB.minr,
				coltoa (graph.axisB.maxc), graph.axisB.maxr);
			for (; *s; ++s);
			if (graph.axisC.valid) {
				/* ", %s%d-%s%d" */
				sprint (s, ", %s%d-%s%d",
					coltoa (graph.axisC.minc), graph.axisC.minr,
					coltoa (graph.axisC.maxc), graph.axisC.maxr);
				for (; *s; ++s);
				if (graph.axisD.valid) {
					/* ", %s%d-%s%d" */
					sprint (s, ", %s%d-%s%d",
						coltoa (graph.axisD.minc), graph.axisD.minr,
						coltoa (graph.axisD.maxc), graph.axisD.maxr);
					for (; *s; ++s);
				}
			}
		}
	}
	*s++ = '.';
	*s = 0;
	message (lin);
}
