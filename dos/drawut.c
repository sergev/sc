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
# include "draw.h"
# include "dglob.h"

extern char *dtoag (), *strcpy ();

_title( x, y, s, n )
char *s;
{
	char b[MAXTITLE];
	int k;

	for( k = 0 ; k < MAXTITLE && s[k] ; k++ )
		b[k] = s[k];
	b[k] = '\0';
        b[n] = '\0';

	move( x-Dx*strlen(b)/2, y );
	label( b );
}

_txt( x, y, s, n )
char *s;
{
	char b[MAXTITLE];
	int k;

	for( k = 0 ; k < MAXTITLE && s[k] ; k++ )
		b[k] = s[k];
	b[k] = '\0';
        b[n] = '\0';

	move( x, y );
	label( b );
}

g_xtitle( s )
char *s;
{
	int x;

	x = MX - Dx/2 - Dy*amin(strlen(s),10);
	_txt( x, Dy*2, s, 10 );
}

g_ytitle( s )
char *s;
{
	_y_txt( Dx/2+10, Y0+LY/2+(Dy*amin(strlen(s),20))/2, s, 20 );
}

_y_txt( x,y,s,n )
char *s;
{
	int i, im;
	char buf [2];

	im = amin(strlen(s),n);

	for( i = 0 ; i < im ; i ++ ) {
# ifdef __MSDOS__
		if( ttyflag )
			buf[0] = s[i]=='.' ? 250 : s[i] ; /* Centered dot */
		else
			buf[0] = s[i];
# else
			buf[0] = s[i];
# endif
		buf[1] = '\0';
		_txt( x, y, buf, 1 );
		y -= Dy;
	}
}

_g_rc2( t1, t2, f )

char *t1, *t2;
{
	int 	ip, l;

	if( t1 == 0 && t2 == 0 )
		return;

	if( *t1 == 0 && *t2 == 0 )
		return;

	l = Dx*(amin(strlen(t1),15)+amin(strlen(t2),15))+16*Dx;
	ip = MX/2-l/2;

	_rectang( ip, Dy/2, ip+4*Dx, 3*Dy/2, 1, f);
	move( ip+5*Dx, Dy);
	label( "-" );
	_txt( ip+6*Dx, Dy, t1, 15 );
	ip += (Dx*amin(strlen(t1),15)+10*Dx);

	_rectang( ip, Dy/2, ip+4*Dx, 3*Dy/2, 2, f );
	move( ip+5*Dx, Dy);
	label( "-" );
	_txt( ip+6*Dx, Dy, t2, 15 );
}

_g_rc3( t1, t2, t3, f )

char *t1, *t2, *t3;
{
	int 	ip, l;

	if( t1 == 0 && t2 == 0 && t3 == 0 )
		return;

	if( *t1 == 0 && *t2 == 0 && *t3 == 0 )
		return;

	l = Dx*(amin(strlen(t1),10)+amin(strlen(t2),10)+amin(strlen(t3),10))+22*Dx;
	ip = MX/2-l/2;

	_rectang( ip, Dy/2, ip+4*Dx, 3*Dy/2, 1, f);
	move( ip+5*Dx, Dy);
	label( "-" );
	_txt( ip+6*Dx, Dy, t1, 10 );
	ip += (Dx*amin(strlen(t1),10)+8*Dx);

	_rectang( ip, Dy/2, ip+4*Dx, 3*Dy/2, 2, f );
	move( ip+5*Dx, Dy);
	label( "-" );
	_txt( ip+6*Dx, Dy, t2, 10 );
	ip += (Dx*amin(strlen(t2),10)+8*Dx);

	_rectang( ip, Dy/2, ip+4*Dx, 3*Dy/2, 3, f );
	move( ip+5*Dx, Dy);
	label( "-" );
	_txt( ip+6*Dx, Dy, t3, 10 );
}

_g_ln2( t1, t2, f )

char *t1, *t2;
{
	int 	ip, l;

		f = f;
	if( t1 == 0 && t2 == 0 )
		return;

	if( *t1 == 0 && *t2 == 0 )
		return;

	l = Dx*(amin(strlen(t1),15)+amin(strlen(t2),15))+13*Dx;
	ip = MX/2-l/2;

	color( 2 );
	line(ip,Dy,ip+4*Dx,Dy);
	color( 1 );
	_txt( ip+5*Dx, Dy, t1, 15 );
	ip += (Dx*amin(strlen(t1),15)+8*Dx);

	color( 3 );
	linemod("longdashed");
	line(ip,Dy,ip+4*Dx,Dy);
	linemod("solid");
	color( 1 );
	_txt( ip+5*Dx, Dy, t2, 10 );
}

_g_ln3( t1, t2, t3, f )

char *t1, *t2, *t3;
{

	int 	ip, l;

	f = f;
	if( t1 == 0 && t2 == 0 && t3 == 0 )
		return;

	if( *t1 == 0 && *t2 == 0 && *t3 == 0 )
		return;

	l = Dx*(amin(strlen(t1),10)+amin(strlen(t2),10)+amin(strlen(t3),10))+19*Dx;
	ip = MX/2-l/2;

	color( 2 );
	line(ip,Dy,ip+4*Dx,Dy);
	color( 1 );
	_txt( ip+5*Dx, Dy, t1, 10 );
	ip += (Dx*amin(strlen(t1),10)+7*Dx);

	color( 3 );
	linemod("longdashed");
	line(ip,Dy,ip+4*Dx,Dy);
	linemod("solid");
	color( 1 );
	_txt( ip+5*Dx, Dy, t2, 10 );
	ip += (Dx*amin(strlen(t2),10)+7*Dx);

	color( 4 );
	linemod("dotted");
	line(ip,Dy,ip+4*Dx,Dy);
	linemod("solid");
	color( 1 );
	_txt( ip+5*Dx, Dy, t3, 10 );
}

_corry( nm, n, yy0, yl )

char *nm[];
int *yy0, *yl, n;
{
        int i, l ,m;

  	for( i = 0 , m = 0 ; i < n ; i++ )
		if( (l = strlen(nm[i])) > m )
			m = l;
	m = m > 6 ? 6 : m;

	*yy0 += Dy*m;
	*yl -= Dy*m;
}

maxset(n, names)
char *names[];
int n;
{
	int rh, maxlit, all, i;
	int r = 0;

	rh = RH(n);
	maxlit = rh/Dx-1;

	if( n > MAX1STYLE )
		r = VERTICAL;

	all = 1;
	for( i = 0 ; i < n ; i ++ )
		all = (( strlen(names[i]) > maxlit ) ? 0 : all);
	if( all )
		r = 0;

	return( r );
}

setsize()
{
	charsize( &Dx, &Dy );
	X0 = 9*Dx+Dx/8;
	Y0 = 4*Dy+Dy/2;
	LX = MX-X0-Dx/8;
	LY = MY-Y0-2*Dy;
}

setspace()
{
	register x, y;

	x = MX;
	y = MY;
	if( plotflag ) {
		if (MY > x)
			x = MY;
		if (MX > y)
			y = MX;
	}
	space( 0, 0, x, y );
}

int _axisXY( n, nm, min, max, yy0, ly, f)

int     n, yy0, ly, f;
char    *nm[];
float   min, max;
{
	int     rh, maxlit, i, j, ny, yb;
	char    s[33];
	float   hy, y, scale;

	rh = RH(n);
	maxlit = rh/Dx-1;

	move( X0, yy0 );
	cont( X0+LX, yy0 );
	move( X0, yy0 );
	cont( X0, yy0+ly );

        if( f & (AREA|GREEDY) ) {
		cont( X0+LX, yy0+ly );
		cont( X0+LX, yy0 );
	}

	hy = _hfact(max-min);
	ny = (int)((max-min)/hy);
	if( ny < 2 ) {
		hy /= 2;
		ny = (int)((max-min)/hy);
	}
	if( min == max )
		scale = 1.;
	else
		scale = ly/(max-min);

	if( min != 0. ) {
		move( X0, yy0+round(-scale*min) );
		cont( X0+LX, yy0+round(-scale*min) );
	}

	for( y = 0., i = 0 ; y <= max && i <= ny; y += hy, i++ ) {
		yb = round(scale*(y-min));
		move( X0-Dx/3, yy0+yb );
		cont( X0, yy0+yb );
		if( f & GREEDY )
			cont( X0+LX, yy0+yb );
		move( Dx+Dx/3, yy0+yb );
		strcpy( s, dtoag( y, 8 ) );
		label( s );
	}

	for( y = -hy, i = 0 ; y >= min && i <= ny; y -= hy, i++ ) {
		yb = round(scale*(y-min));
		move( X0-Dx/3, yy0+yb );
		cont( X0, yy0+yb );
		if( f & GREEDY )
			cont( X0+LX, yy0+yb );
		move( Dx+Dx/3, yy0+yb );
		strcpy( s, dtoag( y, 8 ) );
		label( s );
	}

	for( i = 0 ; i < n ; i++ ) {
		for( j = 0 ; nm[i][j] && j < maxlit && j < 32 ; j++ )
			s[j] = nm[i][j];
		s[j] = '\0';
		move( X0 + 2*rh/3 + rh*i , yy0-2*Dy/3 );
		cont( X0 + 2*rh/3 + rh*i , yy0 );
		if( f & GREEDX )
			cont( X0 + 2*rh/3 +rh*i , yy0+ly );
		if( !(f & VERTICAL) ) {
			move( X0 + 2*rh/3 + rh*i - Dx*j/2+Dx/2 , yy0-Dy-Dy/8 );
			label( s );
		} else
			_y_txt( X0 + 2*rh/3 + rh*i, yy0-3*Dy/2, nm[i], 6 );
	}
}

float _hfact( m )

float   m;
{
	float   f;
	if( m == 0. )
		f = 1.;
	else if( m > 1. ) {
		f = 1.;
		while( m > 10. ) {
			f = f * 10.;
			m /= 10.;
		}
	} else {
		f = 0.1;
		while( m < 0.1 ) {
			f /= 10.;
			m = m * 10.;
		}
	}
	return( f );
}

_rectang( x1, yy1, x2, y2, mode, f )

int     x1, x2, yy1, y2, mode;
{
	int t;

	if( yy1 > y2 ) {
		t = y2;
		y2 = yy1;
		yy1 = t;
	}
	rectan( x1, yy1, x2, y2, mode );
# if defined(RCONTUR) || ! defined(XPLOT)
	f |= BOUND;
# endif

	if( f & BOUND || emulate ) {
		move( x1, yy1 );
		cont( x1, y2 );
		cont( x2, y2 );
		cont( x2, yy1 );
		cont( x1, yy1 );
	}
}

# ifndef COLOR
color( c ) { }
# endif

static _st0( x1, yy1, x2, y2, inv, h )

int x1, yy1, x2, y2, inv, h;
{
	int     i, lx, xt1, xt2, yt1, yt2;

	lx = x2-x1;
	/* ly = y2-yy1; */
	for( i = yy1-lx ; i < y2 ; i += h ) {
		xt1 = x1;
		yt1 = i;
		xt2 = x2;
		yt2 = i+lx;
		if( yt1 < yy1 ) {
			xt1 += (yy1-i);
			yt1 = yy1;
		}
		if( yt2 > y2 ) {
			xt2 -= ( yt2-y2);
			yt2 = y2;
		}
		if( inv ) {
			move( x2-(xt1-x1), yt1 );
			cont( x2-(xt2-x1), yt2 );
		} else {
			move( xt1, yt1 );
			cont( xt2, yt2 );
		}
	}
}

i_rectan( x1, yy1, x2, y2, mode )

int     x1, yy1, x2, y2, mode;
{
	int     h, inv = 0;
	h = mode==1?H1:(mode==2?H2:(mode==3?H3:H4));
	if( mode == 2 || mode == 4 )
		inv = 1;

	color( mode+1 );

	_st0( x1, yy1, x2, y2, inv, h );

	color( 1 );
}

i_grline( x, y, r, f )
float f;
{
	move( x, y );
	cont( x+round(r*cos(f)), y+round(r*sin(f)) );
}

i_grarc( x, y, r, f1, f2 )
float f1, f2;
{
	int nh, i;
	float h;

	nh = (int)((amax(f1,f2)-amin(f2,f1))/(Pi/18.))+1;
	h = (f2-f1)/nh;

	move( x+round(r*cos(f1)), y+round(r*sin(f1)) );
	for( i = 0 ; i <= nh ; i++ )
		cont( x+round(r*cos(f1+i*h)), y+round(r*sin(f1+i*h)) );
}

i_charsize( x, y)
int *x, *y;
{
	*x = DX;
	*y = DY;
}

round(x)
float x;
{
	if( x < 0 )
		return( -((int)(-x+0.5)) );
	else
		return( (int)(x+0.5) );
}

extern i_sector();

# ifdef GRAPHPLOT
g_plot()
{
	extern p_openpl();
	extern p_closepl();
	extern p_erase();
	extern p_space();
	extern p_color();
	extern p_point();
	extern p_circle();
	extern p_cont();
	extern p_move();
	extern p_label();
	extern p_charsize();
	extern p_linemod();
	extern p_line();

	r_openpl = p_openpl;
	r_closepl = p_closepl;
	r_erase = p_erase;
	r_space = p_space;
	r_color = p_color;
	r_point = p_point;
	r_circle = p_circle;
	r_grarc = i_grarc;
	r_sector = i_sector;
	r_cont = p_cont;
	r_move = p_move;
	r_label = p_label;
	r_charsize = p_charsize;
	r_linemod = p_linemod;
	r_rectan = i_rectan;
	r_line = p_line;
	r_grline = i_grline;
	emulate = 1;
	plotflag = 1;
	ttyflag = 0;
	rgflag = 0;
}
# endif

# ifdef HCOPY
g_hcopy()
{
	extern h_openpl();
	extern h_closepl();
	extern h_erase();
	extern h_space();
	extern h_color();
	extern h_point();
	extern h_circle();
	extern h_cont();
	extern h_move();
	extern h_label();
	extern h_charsize();
	extern h_linemod();
	extern h_rectan();
	extern h_line();

	r_openpl = h_openpl;
	r_closepl = h_closepl;
	r_erase = h_erase;
	r_space = h_space;
	r_color = h_color;
	r_point = h_point;
	r_circle = h_circle;
	r_grarc = i_grarc;
	r_sector = i_sector;
	r_cont = h_cont;
	r_move = h_move;
	r_label = h_label;
	r_charsize = h_charsize;
	r_linemod = h_linemod;
	r_rectan = i_rectan;
	r_line = h_line;
	r_grline = i_grline;
	emulate = 1;
	plotflag = 0;
	ttyflag = 0;
	rgflag = 0;
}
# endif

# ifdef GRAPHTTY
g_tty()
{
	extern t_openpl();
	extern t_closepl();
	extern t_erase();
	extern t_space();
	extern t_color();
	extern t_point();
	extern t_circle();
	extern t_sector();
	extern t_cont();
	extern t_move();
	extern t_label();
	extern t_charsize();
	extern t_linemod();
	extern t_rectan();
	extern t_line();
# ifdef __MSDOS__
	extern t_grline();
	extern t_grarc();
# endif

	r_openpl = t_openpl;
	r_closepl = t_closepl;
	r_erase = t_erase;
	r_space = t_space;
	r_color = t_color;
	r_point = t_point;
	r_circle = t_circle;
	r_sector = t_sector;
	r_cont = t_cont;
	r_move = t_move;
	r_label = t_label;
	r_charsize = t_charsize;
	r_linemod = t_linemod;
	r_rectan = t_rectan;
	r_line = t_line;
# ifdef __MSDOS__
	r_grline = t_grline;
	r_grarc = t_grarc;
# else
	r_grline = i_grline;
	r_grarc = i_grarc;
# endif
	emulate = 0;
	plotflag = 0;
	ttyflag = 1;
	rgflag = 1;
}
# endif
