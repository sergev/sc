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
# include <math.h>
# include <graphics.h>

# define PI	3.14159265359

# define COLOR(c) ((c) > 0 ? colors[((c)-1)%((maxcolor<=15)?maxcolor:15)] : 0)
# define DEFAULTCOLOR 1

# define sign(x) ((x)>0?1:((x)<0?-1:0))

static	int     _tx, _ty, _xx0, _yy0;
static	float   _scale = 1.0;
static	int     _movef = 0;
static	int     _color;

static	float   _sx;
static	float   _sy;
static  float   _factor;

static  int	Dx, Dy;

static colors[15] = { 7,1,4,2,3,5,6,8,10,12,9,11,13,14,15 };
static int maxcolor;

static pattern[] = { 	SOLID_FILL,	CLOSE_DOT_FILL,	SLASH_FILL,
			LTSLASH_FILL,	BKSLASH_FILL,   LTBKSLASH_FILL,
			LINE_FILL,	HATCH_FILL,	XHATCH_FILL,
			INTERLEAVE_FILL,WIDE_DOT_FILL };
# define MAXFILL 11

t_openpl()

{
	int drv, mode, xasp, yasp;

# ifndef NOBGIOBJ
	registerbgidriver (CGA_driver);
	registerbgidriver (EGAVGA_driver);
	registerbgidriver (Herc_driver);
# endif

	detectgraph( &drv, &mode );
	initgraph( &drv, &mode, (char *) 0 );
	_sx = getmaxx ();
	_sy = getmaxy ();
	getaspectratio (&xasp, &yasp);
	_factor = ((double) xasp) / yasp;
	maxcolor = getmaxcolor();
	t_color( DEFAULTCOLOR );
	_tx = 0;  _ty = 0;
	_movef = 0;
	/* printf("%d\n",maxcolor); */
	Dx = textwidth("0");
	Dy = textheight("0");
}

float fiarg( x,y )

float   x,y;
{

	if( x >= 0.0 && y >= 0.0 )
	    return( atan2(y,x) );
	else if( x < 0.0 && y >= 0.0 )
	    return( PI-atan2(y,-x) );
	else if( x >= 0.0 && y < 0.0 )
	    return( 2*PI-atan2(-y,x) );
	else if( x < 0.0 && y < 0.0 )
	    return( PI+atan2(-y,-x) );
}
/*
t_arc( x,y,x0,y0,x1,y1 )

int     x,y,x0,y0,x1,y1;
{
	float   fi1, fi2, dfi, r;
	float   fiarg();

	r = sqrt(((float)(x-x0))*(x-x0)+((float)(y-y0))*(y-y0));
        x1 -= x;
	y1 -= y;
	x0 -= x;
	y0 -= y;

	_tsfr( &x, &y );
	_tsfr( &x0, &y0 );
	_tsfr( &x1, &y1 );

	y0 = _sy-y0;
	y1 = _sy-y1;

	fi1 = fiarg((float)(x0),(float)(y0+sign(y0)*0.7)/_factor);
	fi2 = fiarg((float)(x1),(float)(y1+sign(y1)*0.7)/_factor);

	if( fi2 < fi1 ) {
	    dfi = 2*PI-fi1+fi2;
	} else
	    dfi = fi2-fi1;
	if( fi1+dfi > 2*PI)
		fi1 -= 2*PI;
	if( fi1 < 0  ) {
		arc( x,y,(int)(360.5+180.*fi1/PI),360,
			 (int)(_scale*r+.5));
		arc( x,y,0,(int)(180.*(fi1+dfi)/PI+0.5),
			 (int)(_scale*r+.5));
	} else
		arc( x,y,(int)(180.*fi1/PI+0.5),(int)(180.*(fi1+dfi)/PI+0.5),
			 (int)(_scale*r+.5));
	_movef = 1;
}

t_sector( x,y,x0,y0,x1,y1,j )

int     x,y,x0,y0,x1,y1;
{
	float   fi1, fi2, dfi, r;
	float   fiarg();
	int     pn;

	setcolor( COLOR(j+2) );
	r = sqrt(((float)(x-x0))*(x-x0)+((float)(y-y0))*(y-y0));
        x1 -= x;
	y1 -= y;
	x0 -= x;
	y0 -= y;

	_tsfr( &x, &y );
	_tsfr( &x0, &y0 );
	_tsfr( &x1, &y1 );

	y0 = _sy-y0;
	y1 = _sy-y1;
	fi1 = fiarg((float)(x0),(float)(y0+sign(y0)*0.7)/_factor);
	fi2 = fiarg((float)(x1),(float)(y1+sign(y1)*0.7)/_factor);

	if( fi2 < fi1 ) {
	    dfi = 2*PI-fi1+fi2;
	} else
	    dfi = fi2-fi1;
	if( fi1+dfi > 2*PI)
		fi1 -= 2*PI;

	pn = j/maxcolor;
	setfillstyle(pattern[pn%MAXFILL],COLOR(j+2));
	if( fi1 < 0  ) {
		pieslice( x,y,(int)(360.5+180.*fi1/PI),360,
			(int)(_scale*r+.5));
		pieslice( x,y,0,(int)(180.*(fi1+dfi)/PI+.5),
			(int)(_scale*r+.5));
	} else
		pieslice( x,y,(int)(180.*fi1/PI+.5),(int)(180.*(fi1+dfi)/PI+.5),
			(int)(_scale*r+.5));
	setcolor( _color );
	_movef = 1;
}
*/

t_sector( x,y,r,f1,f2,j )
int     x,y,r,j;
float   f1, f2;
{
	int     fi1, fi2, dfi, sx, sy;

/*
	setcolor( COLOR(j+2) );
*/
	sx = x;
	sy = y;

	_tsfr( &x,&y );

	fi1 = round(180.*f1/PI);
	fi2 = round(180.*f2/PI);

        if( fi1 == fi2 ) {
		t_grline( sx, sy, r, f1 );
		return;
	}

	while( fi1 < 0 ) {
		fi1 += 360;
		fi2 += 360;
	}

	if( fi2 < fi1 ) {
	    dfi = 360-fi1+fi2;
	} else
	    dfi = fi2-fi1;

	while( fi1+dfi > 360)
		fi1 -= 360;

	setfillstyle(pattern[(j/maxcolor)%MAXFILL],COLOR(j+2));
	if( fi1 < 0  ) {
		pieslice( x, y, 360+fi1, 360,
			(int)(_scale*r));
		pieslice( x, y, 0, fi1+dfi,
			(int)(_scale*r));
	} else
		pieslice( x, y, fi1, fi1+dfi,
			(int)(_scale*r));
	setcolor( _color );
	_movef = 1;
}

t_grarc( x,y,r,f1,f2 )
int     x,y,r;
float   f1, f2;
{
	int     fi1, fi2, dfi;

	_tsfr( &x,&y );

	fi1 = round(180.*f1/PI);
	fi2 = round(180.*f2/PI);

	while( fi1 < 0 ) {
		fi1 += 360;
		fi2 += 360;
	}

	if( fi2 < fi1 ) {
	    dfi = 360-fi1+fi2;
	} else
	    dfi = fi2-fi1;

	while( fi1+dfi > 360)
		fi1 -= 360;

	if( fi1 < 0  ) {
		arc( x, y, 360+fi1, 360, (int)(_scale*r));
		arc( x, y, 0, fi1+dfi, (int)(_scale*r));
	} else
		arc( x, y, fi1, fi1+dfi, (int)(_scale*r));
	_movef = 1;
}

t_grline( x, y, r, f )
float f;
{
	_tsfr( &x,&y );
	moveto( x, y );
	lineto( x+(int)(_scale*r*cos(f)),
		y-(int)(_factor*(int)(r*_scale*sin(f))) );
/*
	int x1 = x+(int)(r*cos(f));
	int y1 = y+(int)(r*sin(f));

	_tsfr( &x, &y );
	_tsfr( &x1, &y1 );

	moveto( x, y );
	lineto( x1, y1 );
*/
}

t_circle( x, y, r )
int     x,y,r;
{
	_tsfr( &x,&y );
	r = (int)(_scale*r+.5);
	circle(x,y,r);
	_movef = 1;
}

t_closepl()
{
	/* getch(); */
	closegraph();
}

t_color( c )
{
	_color = COLOR(c);
	setcolor( _color );
}

t_cont( x,y )
int     x,y;
{
	_tsfr( &x, &y );
	if( _movef )
	    moveto( _tx, _ty );
	/* printf("c %d %d\n", x, y ); */
	lineto( x, y );
	_tx = x;  _ty = y;
	_movef = 0;
}

t_rectan( x1,y1,x2,y2,c )
int     x1,y1,x2,y2,c;
{
	_tsfr( &x1, &y1 );
	_tsfr( &x2, &y2 );
	setfillstyle(pattern[((c-1)/maxcolor)%MAXFILL],COLOR(c+1));
	bar( x1, y1, x2, y2 );
	setcolor( _color );
	_movef = 1;
}

t_erase()
{
	static first = 1;
	if( ! first ) {
	    cleardevice();
	}
	first = 0;
}

t_label( s )
char    *s;
{
	outtextxy( _tx-Dx/2, _ty-Dy/2, s);
}

t_linemod( s )
char    *s;
{
	if( ! strcmp( s,"dotted" ) )
		setlinestyle( DOTTED_LINE, 0, NORM_WIDTH );
	else if( ! strcmp( s,"solid" ) )
		setlinestyle( SOLID_LINE, 0, NORM_WIDTH );
	else if( ! strcmp( s,"longdashed" ) )
		setlinestyle( DASHED_LINE, 0, NORM_WIDTH );
	else if( ! strcmp( s,"shortdashed" ) )
		setlinestyle( CENTER_LINE, 0, NORM_WIDTH );
	else if( ! strcmp( s,"dotdashed" ) )
		setlinestyle( DOTTED_LINE, 0, NORM_WIDTH );
}

t_move( x,y )
int     x,y;
{
	_tsfr( &x, &y );
	moveto( x, y );
	_tx = x;  _ty = y;
	_movef = 0;
}

t_line( x1,y1,x2,y2 )
int x1,y1,x2,y2;
{
	t_move( x1, y1 );
	t_cont( x2, y2 );
}

t_point( x,y )
{
	_tsfr( &x, &y );
	putpixel( x,y,_color);
}

t_space( x1,y1,x2,y2 )
int     x1,y1,x2,y2;
{
	_xx0 = x1;
	_yy0 = y1;
	_scale = _sx/(float)(x2-x1);
	if( ((float)(y2-y1))*_scale*_factor > _sy )
	    _scale = _sy/((float)(y2-y1)*_factor);
}

_tsfr( x,y )
int     *x,*y;
{
	*x = (int)(((float)(*x-_xx0))*_scale);
	*y = _sy-(int)(((int)((float)(*y-_yy0))*_scale)*_factor);
}

t_charsize( x,y )
int *x, *y;
{
	*x = (int)(textwidth("0")/_scale)+1;
	*y = (int)(textheight("0")/(_factor*_scale))+1;
}
