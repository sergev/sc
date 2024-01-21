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
# include "state.h"
# include "graph.h"

# define min(a,b) ((a)<(b)?(a):(b))
# define max(a,b) ((a)>(b)?(a):(b))

extern Arg, narg;                       /* defined in main.c */
int savedrow, savedcol;
int getrow, getcol, getnum;
int State;
int AutoCalc = 1, Border = 1;

extern char *filename (), *getenv (), *strcpy ();

# ifdef RUS
language ()
{
	if (rusin)
		rusout = rusin = 0;
	else if (rusout)
		rusin = 1;
	else
		rusout = 1;
}
# endif

char *helpstr ()
{
	switch (State) {
	case 0:         return (HLP (64));
	case CELL:      return (HLP (65));
	case UTILITY:   return (HLP (66));
	case GRAPH:     return (HLP (107));
	case SHAPE:     return (HLP (123));
	case JUMP:      return (HLP (67));
	case AREA:      return (HLP (68));
	case SFILE:     return (HLP (69));
	case ROW:       return (HLP (70));
	case COLUMN:    return (HLP (71));
	case AFORMAT:	return (HLP (145));
	case CFORMAT:
	case RFORMAT:
	case OFORMAT:   return (HLP (54));
	default:        return (0);
	}
}

backstate ()
{
	if (State & LEVEL3)
		State &= ~LEVEL3;
	else if (State & LEVEL2)
		State &= ~LEVEL2;
	else if (State & LEVEL1)
		State &= ~LEVEL1;
	else
		State = 0;
	setstate ();
}

setstate ()
{
	register char *s;

	switch (State) {
	case 0:         line [linelim = 0] = 0; clrerr (); return;
	case CELL:      s = HLP (72);   clrerr ();      break;
	case UTILITY:   s = HLP (73);   clrerr ();      break;
	case GRAPH:     s = HLP (108);  msggraph ();    break;
	case SHAPE:     s = HLP (113);  msgshape ();    break;
	case JUMP:      s = HLP (74);   clrerr ();      break;
	case SFILE:     s = HLP (75);   clrerr ();      break;
	case AREA:      s = HLP (76);   clrerr ();      break;
	case ROW:       s = HLP (77);   clrerr ();      break;
	case COLUMN:    s = HLP (78);   clrerr ();      break;
	case CFORMAT:   s = HLP (137);	clrerr ();      break;
	case AFORMAT:   s = HLP (138);	clrerr ();      break;
	case RFORMAT:   s = HLP (139);	clrerr ();      break;
	case OFORMAT:   s = HLP (140);	clrerr ();      break;
	default:        s = "???/";     clrerr ();      break;
	}
	strcpy (line, s);
	linelim = strlen (line);
}

static mark ()
{
	show (HLP (83));
	savedrow = currow;
	savedcol = curcol;
}

static cmd0 (c)
{
	switch (c) {
	default:
		VBeep ();
		return;
	CASE ('u', 'р'):        /* Utility, Разное */
		State = UTILITY;
		setstate ();
		return;
	CASE ('g', 'г'):        /* Graph, График */
		State = GRAPH;
		setstate ();
		return;
	CASE ('j', 'д'):        /* Jump, Движение */
		State = JUMP;
		setstate ();
		return;
	CASE ('f', 'ф'):        /* File, Файл */
		State = SFILE;
		setstate ();
		return;
	CASE ('a', 'о'):        /* Area, Область */
		State = AREA;
		setstate ();
		return;
	CASE ('r', 'с'):        /* Row, Строка */
		State = ROW;
		setstate ();
		return;
	CASE ('o', 'т'):        /* cOlumn, сТолбец */
		State = COLUMN;
		setstate ();
		return;
	CASE ('c', 'я'):        /* Cell, Ячейка */
		State = CELL;
		setstate ();
		return;
	CASE ('q', 'ы'):        /* Quit, вЫход */
		show (HLP (79));
		if (modcheck ()) {
			endterm ();
			exit (0);
		}
		return;
	}
}

static cmdcformat (c)
{
	switch (c) {
	default:
		VBeep ();
		return;
	CASE ('p', 'т'):        /* Precision, Точность */
		show (HLP (106));
		/* "Enter precision: " */
		getnumber (MSG (27), 0, 72);
		setfixed (currow, curcol, currow, curcol, getnum);
		break;
	CASE ('l', 'л'):        /* Leftalign, вЛево */
		show (HLP (141));
		setformat (currow, curcol, currow, curcol, is_leftflush);
		break;
	CASE ('r', 'п'):        /* Rightalign, вПраво */
		show (HLP (142));
		setformat (currow, curcol, currow, curcol, is_rightflush);
		break;
	CASE ('c', 'ц'):        /* Center, Центр */
		show (HLP (143));
		setformat (currow, curcol, currow, curcol, is_center);
		break;
	CASE ('d', 'у'):        /* Default, Умолчание */
		show (HLP (144));
		setformat (currow, curcol, currow, curcol, 0);
		break;
	}
}

static cmdaformat (c)
{
	switch (c) {
	default:
		VBeep ();
		return;
	CASE ('p', 'т'):        /* Precision, Точность */
		show (HLP (106));
		/* "Enter precision: " */
		getnumber (MSG (27), 0, 72);
		setfixed (savedrow, savedcol, currow, curcol, getnum);
		break;
	CASE ('m', 'м'):        /* Mark, Метка */
		mark ();
		break;
	CASE ('l', 'л'):        /* Leftalign, вЛево */
		show (HLP (141));
		setformat (savedrow, savedcol, currow, curcol, is_leftflush);
		break;
	CASE ('r', 'п'):        /* Rightalign, вПраво */
		show (HLP (142));
		setformat (savedrow, savedcol, currow, curcol, is_rightflush);
		break;
	CASE ('c', 'ц'):        /* Center, Центр */
		show (HLP (143));
		setformat (savedrow, savedcol, currow, curcol, is_center);
		break;
	CASE ('d', 'у'):        /* Default, Умолчание */
		show (HLP (144));
		setformat (savedrow, savedcol, currow, curcol, 0);
		break;
	}
}

static cmdrformat (c)
{
	switch (c) {
	default:
		VBeep ();
		return;
	CASE ('p', 'т'):        /* Precision, Точность */
		show (HLP (106));
		/* "Enter precision: " */
		getnumber (MSG (27), 0, 72);
		setfixed (currow, 0, currow, maxcol, getnum);
		break;
	CASE ('l', 'л'):        /* Leftalign, вЛево */
		show (HLP (141));
		setformat (currow, 0, currow, maxcol, is_leftflush);
		break;
	CASE ('r', 'п'):        /* Rightalign, вПраво */
		show (HLP (142));
		setformat (currow, 0, currow, maxcol, is_rightflush);
		break;
	CASE ('c', 'ц'):        /* Center, Центр */
		show (HLP (143));
		setformat (currow, 0, currow, maxcol, is_center);
		break;
	CASE ('d', 'у'):        /* Default, Умолчание */
		show (HLP (144));
		setformat (currow, 0, currow, maxcol, 0);
		break;
	}
}

static cmdoformat (c)
{
	switch (c) {
	default:
		VBeep ();
		return;
	CASE ('p', 'т'):        /* Precision, Точность */
		show (HLP (106));
		/* "Enter precision: " */
		getnumber (MSG (27), 0, 72);
		setfixed (0, curcol, maxrow, curcol, getnum);
		break;
	CASE ('l', 'л'):        /* Leftalign, вЛево */
		show (HLP (141));
		setformat (0, curcol, maxrow, curcol, is_leftflush);
		break;
	CASE ('r', 'п'):        /* Rightalign, вПраво */
		show (HLP (142));
		setformat (0, curcol, maxrow, curcol, is_rightflush);
		break;
	CASE ('c', 'ц'):        /* Center, Центр */
		show (HLP (143));
		setformat (0, curcol, maxrow, curcol, is_center);
		break;
	CASE ('d', 'у'):        /* Default, Умолчание */
		show (HLP (144));
		setformat (0, curcol, maxrow, curcol, 0);
		break;
	}
}

static cmdcell (c)
{
	switch (c) {
	default:
		VBeep ();
		return;
	CASE ('f', 'ф'):        /* Format, Формат */
		State = CFORMAT;
		setstate ();
		return;
	CASE ('d', 'с'):        /* Delete, Стереть */
		show (HLP (81));
	{
		register struct ent **p;
		register col;

		flush_saved ();
		for (col=curcol; Arg-- && col<MAXCOLS; col++) {
			p = &tbl[currow][col];
			if (*p) {
				free_ent (*p);
				*p = 0;
			}
		}
		syncrefs ();
		++FullUpdate;
		break;
	}
	CASE ('u', 'в'):        /* Undelete, Вернуть */
		show (HLP (82));
		pullcells ('m');
		break;
	CASE ('m', 'м'):        /* Mark, Метка */
		show (HLP (83));
		savedrow = currow;
		savedcol = curcol;
		break;
	CASE ('c', 'к'):        /* Copy, Копия */
		show (HLP (84));
	{
		register struct ent *p = tbl[savedrow][savedcol];
		register col;
		register struct ent *n;

		if (!p)
			break;
		FullUpdate++;
		modflg++;
		for (col=curcol; Arg-- && col<MAXCOLS; col++) {
			n = lookat (currow, col);
			clearent (n);
			copy1 (p, n, currow-savedrow, col-savedcol);
		}
		break;
	}
	}
}

static cmdshape (c)
{
	switch (c) {
	default:
		VBeep ();
		return;
	CASE ('p', 'д'):        /* Pie, Диаграмма */
		show (HLP (128));
		graph.shape = PIE;
		break;
	CASE ('g', 'г'):        /* Graph, График */
		show (HLP (129));
		graph.shape = GRAPHIC;
		break;
	CASE ('x', 'х'):        /* X-Y, Х-У */
		show (HLP (80));
		graph.shape = XYGRAPH;
		break;
	CASE ('b', 'и'):        /* Bar, гИстограмма */
		show (HLP (130));
		graph.shape = HYSTO;
		break;
	CASE ('s', 'с'):        /* Stacked-bar, Совмещенная-гистограмма */
		show (HLP (131));
		graph.shape = CHYSTO;
		break;
	}
	msgshape ();
}

static cmdgraph (c)
{
	register struct axis *a;

	switch (c) {
	default:
		VBeep ();
		return;
	CASE ('s', 'т'):        /* Shape, Тип */
		State = SHAPE;
		setstate ();
		return;
	CASE ('m', 'м'):        /* Mark, Метка */
		mark ();
		break;
	CASE ('a', 'а'):        /* axisA, осьА */
		show (HLP (110));
		a = &graph.axisA;
laxis:
		a->minr = min (savedrow, currow);
		a->minc = min (savedcol, curcol);
		a->maxr = max (savedrow, currow);
		a->maxc = max (savedcol, curcol);
		a->valid = 1;
		curcol = savedcol;
		currow = savedrow;
		break;
	CASE ('b', 'б'):        /* axisB, осьБ */
		show (HLP (111));
		a = &graph.axisB;
		goto laxis;
	CASE ('c', 'в'):        /* axisC, осьВ */
		show (HLP (112));
		a = &graph.axisC;
		goto laxis;
	CASE ('d', 'г'):        /* axisD, осьГ */
		show (HLP (117));
		a = &graph.axisD;
		goto laxis;
	CASE ('r', 'с'):        /* Reset, Сброс */
		show (HLP (114));
		graph.axisA.valid = 0;
		graph.axisB.valid = 0;
		graph.axisC.valid = 0;
		graph.axisD.valid = 0;
		break;
	CASE ('h', 'п'):        /* Hardcopy, Печать */
		show (HLP (136));
# ifdef __MSDOS__
		hardcopy ("prn");
# endif
		break;
	}
	msggraph ();
}

static cmdutility (c)
{
	switch (c) {
	default:
		VBeep ();
		return;
	CASE ('p', 'т'):        /* Precision, Точность */
		show (HLP (106));
		/* "Current precision is %d, legal range is 0-72." */
		message (MSG (26), precision);
		/* "Enter precision: " */
		getnumber (MSG (27), 0, 72);
		precision = getnum;
		++FullUpdate;
		++modflg;
		clrerr ();
		break;
	CASE ('l', 'я'):        /* Language, Язык */
		show (HLP (85));
# ifdef RUS
		language ();
		setstate ();
		++FullUpdate;
		clrerr ();
# endif
		break;
	CASE ('a', 'а'):        /* Autocalc, Авто */
		show (HLP (109));
		AutoCalc ^= 1;
		break;
	CASE ('r', 'п'):        /* Recalc, Перевычислить */
		show (HLP (86));
		EvalAll ();
		break;
	CASE ('b', 'р'):        /* Border, Рамка */
		show (HLP (135));
		Border ^= 1;
		clrerr ();
		++FullUpdate;
		break;
	CASE ('w', 'ш'):        /* Width, Ширина */
		show (HLP (87));
		expandwidth ();
		break;
	CASE ('h', 'в'):        /* Height, Высота */
		show (HLP (88));
		expandheight ();
		break;
	CASE ('m', 'с'):        /* Minimize, Сократить */
		show (HLP (89));
		if (maxrow < MAXROWS-1)
			newheight (maxrow+1);
		if (maxcol < MAXCOLS-1)
			newwidth (maxcol+1);
		break;
	}
}

static cmdjump (c)
{
	switch (c) {
	default:
		VBeep ();
		return;
	CASE ('e', 'к'):        /* End, Конец */
		show (HLP (90));
		currow = maxrow;
		curcol = maxcol;
		break;
	CASE ('c', 'я'):        /* Cell, Ячейка */
		show (HLP (91));
		/* "Enter name of cell: " */
		getcell (MSG (18));
		currow = getrow;
		curcol = getcol;
		break;
	CASE ('d', 'н'):        /* Down, вНиз */
		show (HLP (92));
		currow = MAXROWS - 1;
		while (! tbl[currow][curcol] && currow>0)
			--currow;
		break;
	CASE ('u', 'в'):        /* Up, вВерх */
		show (HLP (93));
		currow = 0;
		break;
	CASE ('l', 'л'):        /* Left, вЛево */
		show (HLP (94));
		curcol = 0;
		break;
	CASE ('r', 'п'):        /* Right, вПраво */
		show (HLP (95));
		curcol = MAXCOLS - 1;
		while (! tbl[currow][curcol] && curcol>0)
			--curcol;
		break;
	}
}

static cmdsfile (c)
{
	switch (c) {
	default:
		VBeep ();
		return;
	CASE ('s', 'з'):        /* Save, Запись */
		show (HLP (96));
		/* "Save table in file: " */
		writefile (filename (MSG (19), EXTSC, curfile));
		break;
	CASE ('l', 'ч'):        /* Load, Чтение */
		show (HLP (97));
		/* "Load file: " */
		readfile (filename (MSG (20), EXTSC, curfile), 1);
		break;
	CASE ('m', 'с'):        /* Merge, Слияние */
		show (HLP (98));
		/* "Merge current table and file: " */
		readfile (filename (MSG (21), EXTSC, (char *) 0), 0);
		break;
	CASE ('p', 'п'):        /* Print, Печать */
		show (HLP (99));
		/* "Print table to file: " */
		printfile (filename (MSG (22), EXTLP, curfile));
		break;
	CASE ('o', 'г'):        /* plOt, График */
		show (HLP (118));
		/* "Plot graph to file: " */
		plotfile (filename (MSG (133), EXTPLT, curfile));
		break;
	CASE ('t', 'т'):        /* Tbl, Тбл */
		show (HLP (100));
		/* "Write table in tbl format to file: " */
		tblprintfile (filename (MSG (23), EXTTBL, curfile));
		break;
	}
}

static cmdarea (c)
{
	switch (c) {
	default:
		VBeep ();
		return;
	CASE ('f', 'ф'):        /* Format, Формат */
		State = AFORMAT;
		setstate ();
		return;
	CASE ('v', 'ч'):        /* Valuize, Числа */
		show (HLP (101));
		valueize_area (savedrow, savedcol, currow, curcol);
		/* "Area %s%d-%s%d valueized." */
		error (MSG (24),
			coltoa (savedcol), savedrow,
			coltoa (curcol), currow);
		++modflg;
		curcol = savedcol;
		currow = savedrow;
		break;
	CASE ('d', 'с'):        /* Delete, Стереть */
		show (HLP (81));
		flush_saved ();
		erase_area (savedrow, savedcol, currow, curcol);
		/* "Area %s%d-%s%d deleted." */
		error (MSG (25),
			coltoa (savedcol), savedrow,
			coltoa (curcol), currow);
		syncrefs ();
		++FullUpdate;
		++modflg;
		curcol = savedcol;
		currow = savedrow;
		break;
	CASE ('u', 'в'):        /* Undelete, Вернуть */
		show (HLP (82));
		pullcells ('m');
		break;
	CASE ('c', 'к'):        /* Copy, Копия */
		show (HLP (84));
		copy (currow, curcol, savedrow, savedcol);
		break;
	CASE ('i', 'з'):        /* fIll, Заполнить */
		show (HLP (104));
		fill (currow, curcol, savedrow, savedcol);
		curcol = savedcol;
		currow = savedrow;
		break;
	CASE ('m', 'м'):        /* Mark, Метка */
		mark ();
		break;
	}
}

static cmdrow (c)
{
	switch (c) {
	default:
		VBeep ();
		return;
	CASE ('f', 'ф'):        /* Format, Формат */
		State = RFORMAT;
		setstate ();
		return;
	CASE ('d', 'с'):        /* Delete, Стереть */
		show (HLP (81));
		deleterow (Arg);
		break;
	CASE ('u', 'в'):        /* Undelete, Вернуть */
		show (HLP (82));
		while (Arg--)
			pullcells ('r');
		break;
	CASE ('h', 'п'):        /* Hide, сПрятать */
		show (HLP (102));
		hiderow (Arg);
		break;
	CASE ('s', 'о'):        /* Show, Открыть */
		show (HLP (103));
		rowshow ();
		break;
	CASE ('i', 'а'):        /* Insert, встАвить */
		show (HLP (105));
		if (currow <= maxrow)
			openrow (currow, Arg);
		break;
	CASE ('c', 'к'):        /* Copy, Копия */
		show (HLP (84));
		duprow (Arg);
		break;
	CASE ('v', 'ч'):        /* Valuize, Числа */
		show (HLP (101));
		valueize_area (currow, 0, currow + Arg - 1, maxcol);
		++modflg;
		break;
	}
}

static cmdcolumn (c)
{
	switch (c) {
	default:
		VBeep ();
		return;
	CASE ('f', 'ф'):        /* Format, Формат */
		State = OFORMAT;
		setstate ();
		return;
	CASE ('d', 'с'):        /* Delete, Стереть */
		show (HLP (81));
		deletecol (Arg);
		break;
	CASE ('u', 'в'):        /* Undelete, Вернуть */
		show (HLP (82));
		while (Arg--)
			pullcells ('c');
		break;
	CASE ('h', 'п'):        /* Hide, сПрятать */
		show (HLP (102));
		hidecol (Arg);
		break;
	CASE ('s', 'о'):        /* Show, Открыть */
		show (HLP (103));
		colshow ();
		break;
	CASE ('i', 'а'):        /* Insert, встАвить */
		show (HLP (105));
		if (curcol <= maxcol)
			opencol (curcol, Arg);
		break;
	CASE ('c', 'к'):        /* Copy, Копия */
		show (HLP (84));
		dupcol (Arg);
		break;
	CASE ('w', 'ш'):        /* Width, Ширина */
		show (HLP (87));
		/* "Current width is %d, legal range is 0-72." */
		message (MSG (28), fwidth [curcol]);
		/* "Enter width for current column: " */
		getnumber (MSG (29), 0, 72);
		fwidth [curcol] = getnum;
		++FullUpdate;
		++modflg;
		clrerr ();
		break;
	CASE ('v', 'ч'):        /* Valuize, Числа */
		show (HLP (101));
		valueize_area (0, curcol, maxrow, curcol + Arg - 1);
		++modflg;
		break;
	}
}

command (c)
{
	if (c == '/') {
		State = 0;
		setstate ();
		return;
	}
	switch (State) {
	case 0:
		cmd0 (c);
		return;
	case CFORMAT:		/* cell format */
		cmdcformat (c);
		break;
	case AFORMAT:           /* area format */
		cmdaformat (c);
		break;
	case RFORMAT:           /* row format */
		cmdrformat (c);
		break;
	case OFORMAT:           /* column format */
		cmdoformat (c);
		break;
	case CELL:
		cmdcell (c);
		break;
	case SHAPE:
		cmdshape (c);
		break;
	case GRAPH:
		cmdgraph (c);
		break;
	case UTILITY:
		cmdutility (c);
		break;
	case JUMP:
		cmdjump (c);
		break;
	case SFILE:
		cmdsfile (c);
		break;
	case AREA:
		cmdarea (c);
		break;
	case ROW:
		cmdrow (c);
		break;
	case COLUMN:
		cmdcolumn (c);
		break;
	}
	Arg = 1;
	narg = 0;
}
