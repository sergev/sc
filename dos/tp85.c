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

# define FACTOR         (2./5.)

/*
 * Рисование дуги из точки x0,y0 в x1,y1 против час. стрелки
 * с центром в x,y  на Э-85
 * by @VG
 */

int emulate;

static int _Ox, _Oy;
static int _COx, _COy;

static float obotx = 0.;
static float oboty = 0.;
static float botx = 0.;
static float boty = 0.;
static float scalex = 1.;
static float scaley = 1.;

static _DotC(c, x, y)
{
	_Ox = x; _Oy = y;
	putch(c);
	x = (x-obotx)*scalex + botx;
	y = (y-oboty)*scaley + boty;
	if( x < 0 )    x = 0;
	if( x > 1023 ) x = 1023;
	if( y < 0 )    y = 0;
	if( y > 239  ) y = 239;
	_DotCC(x, y);
	_COx = x; _COy = y;
}

static _DotCC(x, y)
{
	long Lc;

	Lc = ((long)(239-y)<<10) + x;
	putch(((short)(Lc>>12) & 077) + '@');
	putch(((short)(Lc>> 6) & 077) + '@');
	putch(((short) Lc      & 077) + '@');
}

static putch(c)
{
	putc(c,stdout);
}

t_rectan( x1, y1, x2, y2, mode )

int     x1, y1, x2, y2, mode;
{
	if( mode > 4 ) {
		emulate = 1;
		i_rectan( x1, y1, x2, y2, mode );
		emulate = 0;
		return;
	}

	t_color( mode+1 );

	mode = (mode-1)%4;

	_DotC(013, x1, y1);
	putch( 033 );
	putch( '2'+mode%2 );
	putch( 033 );
	putch( '0'+((mode+1)>>1)%2 );
	_DotC(036, x1, y2);
	_DotC(035, x2, y2);

	putch( 033 );
	putch( '0' );
	putch( 033 );
	putch( '2' );
	putch( 021 );

	t_color( 1 );
}

t_sector( x0,y0,ir,fi1,fi2,mode )

int     x0,y0,ir,mode;
float   fi1, fi2;
{
	if( mode > 3 ) {
		emulate = 1;
		i_sector( x0,y0,ir,fi1,fi2,mode );
		emulate = 0;
		return;
	}

	t_color( mode+2 );

	mode = mode%4;

	_DotC(06, x0, y0);
	putch( 033 );
	putch( '2'+mode%2 );
	putch( 033 );
	putch( '0'+((mode+1)>>1)%2 );

	i_grarc( x0, y0, ir, fi1, fi2 );

	putch( 033 );
	putch( '0' );
	putch( 033 );
	putch( '2' );
	putch( 021 );

	t_color( 1 );
}

t_arc(x, y, x0, y0, x1, y1)
{
	int xx = _Ox, yy = _Oy;
	double sqrt();
	long r;

	/*
	 * Вычислим радиус
	 */
	r = scalex*sqrt( (x-x0)*(double)(x-x0) + (y-y0)*(double)(y-y0) );

	/*
	 * Выбор дуги окружности
	 * Если элемент z векторного произведения векторов
	 * (x,y)->(x0,y0) и (x,y)->(x1,y1) имеет положительный/отрицательный
	 * знак, то рисовать по/против часовой стрелки
	 */
	if( (x0-x)*(y1-y) < (x1-x)*(y0-y) )
		r = -r;

	/*
	 * Установим радиус, начальную точку и
	 * нарисуем до конечной точки
	 */
	putch(034);
	putch( ((short)(r>>12) & 077) + '@' );
	putch( ((short)(r>> 6) & 077) + '@' );
	putch( ((short) r      & 077) + '@' );
	_DotC(036, x0, y0);
	_DotC(032, x1, y1);

	_DotC(036, xx, yy);
}

/*
 * Рисование окружности - Э-85
 * by @VG
 */
t_circle(x, y, r)
{
	int xx=_Ox, yy=_Oy;

	_DotC(036, x, y);
	_DotC(02, x+r, y);
	_DotC(036, xx, yy);
}

t_closepl()
{
	putch(033); putch('0');
	fflush(stdout);
}

/*
 * Установка цвета для вывода на Электронике-85
 * by @VG
 */
t_color(col)
{
	putch(033);
	putch('x');
	if( col == 0 ) {
		putch('7');
		putch(033);
		putch('d');             /* clear by white */
	} else {
		col = (col-1)%7;
		putch('1'+col);
		putch(033);
		putch('c');             /* draw by col */
	}
}
/*
colormap(nc, r, g, b) {}
dot()
{
}
*/
/*
 * Очистка экрана - Э-85
 * by @VG
 */

t_erase()
{
	putch(014);
}
/*
 * Рисование текста - Э-85
 * by @VG
 */

t_label(s)
register char *s;
{
	register c;

	putch(033); putch('S'); putch(033) ; putch('e');
	putch(03);
	_DotCC(_COx-5, _COy+4);  /* Center the char */
	while( c = *s++ ) {
		_COx += 12;
		putch(c);
	}
	putch(05);      /* End of graph text mode */
	putch(036);
	_DotCC(_COx+5, _COy-4);  /* Set next point */
	putch(033); putch('R');
}
/*
 * Рисование линии - Э-85
 * by @VG
 */

t_line(x0,y0,x1,y1)
{
	_DotC(036,x0,y0);
	_DotC(035,x1,y1);
}
/*
 * Установка типа линии - Э-85
 * by @VG
 */

t_linemod(s)
char *s;
{
	putch(033);
	if( !strcmp(s, "solid") )
		putch('0');
	else
		putch('1');
}
/*
 * Установка текущей позиции - Э-85
 * by @VG
 */

t_move(x, y)
{
	_DotC(036, x, y);
}

t_openpl()
{
}
/*
 * Рисование точки - Э-85
 * by @VG
 */

t_point(x, y)
{
	_DotC(037, x, y);
}
/*
 * Установка размещения области рисования на экране Э-85
 * by @VG
 */

t_space(x0,y0,x1,y1)
{
	boty = 0.;
	obotx = x0;
	oboty = y0;
	scalex = 1023./(x1-x0);
	if( scalex*(y1-y0)*FACTOR > 239.)
		scalex = 239./((y1-y0)*FACTOR);
	scaley = FACTOR*scalex;
	botx = 0.;     /* Moves graph area to left ... */
}
/*
 * Процедура вывода координат для Э-85
 * by @VG
 */

t_cont(x,y)
{
	_DotC(035, x, y);
}

t_charsize( x,y )
int *x, *y;
{
	*x = (int)(13./scalex)+1;
	*y = (int)(8./scaley)+1;
}
