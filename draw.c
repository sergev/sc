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

# define DRAWFILE

# include <math.h>
# include "draw.h"
# include "dglob.h"

g_title( s )
char *s;
{
	_title( MX/2, MY-Dy, s, 70 );
}

g_border()
{
	move( 0, 0 );
	cont( 0, MY-1 );
	cont( MX-1, MY-1 );
	cont( MX-1, 0 );
	cont( 0, 0 );
}

g_hysto( n, names, values, t, f )

int     n;
char    *names[], *t;
float   values[];
{
	float   min, max, scale;
	int     rd, rh, yy0, i;

	int     ly;
	int     yyy0;

	erase();
	setspace();
	setsize();

	ly = LY;
	yyy0 = Y0;

	for( i = 1, min = max = values[0] ; i < n ; i++ ) {
		max = (values[i]>max) ? values[i] : max;
		min = (values[i]<min) ? values[i] : min;
	}

	f |= maxset(n,names);

	if( f & VERTICAL )
		_corry( names, n, &yyy0, &ly );

	if( min >= 0. ) {
		yy0 = yyy0;
		min = 0.;
	} else if( max > 0. )
		yy0 = round((-(float)min*ly)/(max-min)+yyy0);
	else {
		yy0 = ly+yyy0;
		max = 0.;
	}

	rh = RH(n);
	rd = rh/3;

	if( min == max )
		scale = 1.;
	else
		scale = ly/(max-min);

	_axisXY( n, names, min, max, yyy0, ly, f );

	for( i = 0 ; i < n ; i++ ) {
		_rectang( X0+rh*i+rd, yy0,
			 X0+rh*i+3*rd, yy0+round(scale*values[i]),
			 1 , f);
	}
	_title( MX/2, Dy, t, 40 );
}

g_2hysto( n, names, val1, val2, t1, t2, f )

int     n;
char    *names[], *t1, *t2;
float   val1[], val2[];
{
	float   min, max, scale, sp, sm;
	int     rd, rh, yy0, yy1, y2;
	int     i;

	int     ly;
	int     yyy0;

	erase();
	setspace();
	setsize();

	ly = LY;
	yyy0 = Y0;

	for( i = 0, min = max = 0. ; i < n ; i++ ) {
		sm = sp = 0.;
		if( val1[i] > 0. )
			sp += val1[i];
		else
			sm += val1[i];
		if( val2[i] > 0. )
			sp += val2[i];
		else
			sm += val2[i];
		max = (sp > max) ? sp : max;
		min = (sm < min) ? sm : min;
	}

	f |= maxset(n,names);

	if( f & VERTICAL )
		_corry( names, n, &yyy0, &ly );

	if( min >= 0. ) {
		yy0 = yyy0;
	} else if( max > 0. )
		yy0 = round((-(float)min*ly)/(max-min)+yyy0);
	else {
		yy0 = ly+yyy0;
	}

	rh = RH(n);
	rd = rh/3;

	if( min == max )
		scale = 1.;
	else
		scale = ly/(max-min);

	_axisXY( n, names, min, max, yyy0, ly, f );

	for( i = 0 ; i < n ; i++ ) {
		yy1 = round(scale*val1[i]);
		y2 = round(scale*val2[i]);
		_rectang( X0+rh*i+rd, yy0,
			 X0+rh*i+3*rd, yy0+yy1, 1, f);
		if( sign(yy1)*sign(y2) < 0 )
			yy1 = 0.;
		_rectang( X0+rh*i+rd, yy0+yy1,
			 X0+rh*i+3*rd, yy0+yy1+y2, 2, f );
	}
	_g_rc2( t1, t2, f);
}

g_c2hysto( n, names, val1, val2, t1, t2, f )

int     n, f;
char    *names[], *t1, *t2;
float   val1[], val2[];
{
	float   min, max, scale;
	int     rd, rh, yy0;
	int     i;

	int     ly;
	int     yyy0;

	erase();
	setspace();
	setsize();

	ly = LY;
	yyy0 = Y0;

	for( i = 1, max = val1[0], min = val1[0] ; i < n ; i++ ) {
		max = (val1[i]>max) ? val1[i] : max;
		min = (val1[i]<min) ? val1[i] : min;
	}
	for( i = 0 ; i < n ; i++ ) {
		max = (val2[i]>max) ? val2[i] : max;
		min = (val2[i]<min) ? val2[i] : min;
	}

	f |= maxset(n,names);

	if( f & VERTICAL )
		_corry( names, n, &yyy0, &ly );

	if( min >= 0. ) {
		yy0 = yyy0;
		min = 0.;
	} else if( max > 0. )
		yy0 = round((-(float)min*ly)/(max-min)+yyy0);
	else {
		yy0 = ly+yyy0;
		max = 0.;
	}

	rh = RH(n);
	rd = rh/3;

	if( min == max )
		scale = 1.;
	else
		scale = ly/(max-min);

	_axisXY( n, names, min, max, yyy0, ly, f );

	for( i = 0 ; i < n ; i++ ) {
		_rectang( X0+rh*i+rd, yy0,
			 X0+rh*i+2*rd, yy0+round(scale*val1[i]),
			 1, f );
		_rectang( X0+rh*i+2*rd, yy0,
			 X0+rh*i+3*rd, yy0+round(scale*val2[i]),
			 2, f );
	}

	_g_rc2( t1, t2, f);
}

g_3hysto( n, names, val1, val2, val3, t1, t2, t3, f )

int     n, f;
char    *names[], *t1, *t2, *t3;
float   val1[], val2[], val3[];
{
	float   min, max, scale, sp, sm;
	int     rd, rh, yy0, yy1, y2, y3, y1d, y2d, yp, ym;
	int     i;

	int     ly;
	int     yyy0;

	erase();
	setspace();
	setsize();

	ly = LY;
	yyy0 = Y0;

	for( i = 0, min = max = 0. ; i < n ; i++ ) {
		sm = sp = 0.;
		if( val1[i] > 0. )
			sp += val1[i];
		else
			sm += val1[i];
		if( val2[i] > 0. )
			sp += val2[i];
		else
			sm += val2[i];
		if( val3[i] > 0. )
			sp += val3[i];
		else
			sm += val3[i];
		max = (sp > max) ? sp : max;
		min = (sm < min) ? sm : min;
	}

	f |= maxset(n,names);

	if( f & VERTICAL )
		_corry( names, n, &yyy0, &ly );

	if( min >= 0. ) {
		yy0 = yyy0;
	} else if( max > 0. )
		yy0 = round((-(float)min*ly)/(max-min)+yyy0);
	else {
		yy0 = ly+yyy0;
	}

	rh = RH(n);
	rd = rh/3;

	if( min == max )
		scale = 1.;
	else
		scale = ly/(max-min);

	_axisXY( n, names, min, max, yyy0, ly, f );

	for( i = 0 ; i < n ; i++ ) {
		yy1 = round(scale*val1[i]);
		y2 = round(scale*val2[i]);
		y3 = round(scale*val3[i]);
		_rectang( X0+rh*i+rd, yy0,
			 X0+rh*i+3*rd, yy0+yy1, 1, f);
		if( sign(yy1)*sign(y2) < 0 )
			y1d = 0.;
		else
			y1d = yy1;
		_rectang( X0+rh*i+rd, yy0+y1d,
			 X0+rh*i+3*rd, yy0+y1d+y2, 2, f );

		yp = ym = 0;
		if( yy1 >= 0 )
			yp = yy1;
		if( y1d+y2 > yp )
			yp = y1d+y2;
		if( yy1 < 0 )
			ym = yy1;
		if( y1d+y2 < ym )
			ym = y1d+y2;
		y2d = y3 > 0 ? yp : ym;

		_rectang( X0+rh*i+rd, yy0+y2d,
			 X0+rh*i+3*rd, yy0+y2d+y3, 3, f );
	}
	_g_rc3( t1, t2, t3, f);
}

g_c3hysto( n, names, val1, val2, val3, t1, t2, t3, f )

int     n, f;
char    *names[], *t1, *t2, *t3;
float   val1[], val2[], val3[];
{
	float   min, max, scale;
	int     rd, rh, yy0;
	int     i;

	int     ly;
	int     yyy0;

	erase();
	setspace();
	setsize();

	ly = LY;
	yyy0 = Y0;

	for( i = 1, max = val1[0], min = val1[0] ; i < n ; i++ ) {
		max = (val1[i]>max) ? val1[i] : max;
		min = (val1[i]<min) ? val1[i] : min;
	}
	for( i = 0 ; i < n ; i++ ) {
		max = (val2[i]>max) ? val2[i] : max;
		min = (val2[i]<min) ? val2[i] : min;
	}
	for( i = 0 ; i < n ; i++ ) {
		max = (val3[i]>max) ? val3[i] : max;
		min = (val3[i]<min) ? val3[i] : min;
	}

	f |= maxset(n,names);

	if( f & VERTICAL )
		_corry( names, n, &yyy0, &ly );

	if( min >= 0. ) {
		yy0 = yyy0;
		min = 0.;
	} else if( max > 0. )
		yy0 = round((-(float)min*ly)/(max-min)+yyy0);
	else {
		yy0 = ly+yyy0;
		max = 0.;
	}

	rh = RH(n);
	rd = rh/4;

	if( min == max )
		scale = 1.;
	else
		scale = ly/(max-min);

	_axisXY( n, names, min, max, yyy0, ly, f );

	for( i = 0 ; i < n ; i++ ) {
		_rectang( X0+rh*i+5*rd/4, yy0,
			 X0+rh*i+9*rd/4, yy0+round(scale*val1[i]),
			 1, f );
		_rectang( X0+rh*i+9*rd/4, yy0,
			 X0+rh*i+13*rd/4, yy0+round(scale*val2[i]),
			 2, f );
		_rectang( X0+rh*i+13*rd/4, yy0,
			 X0+rh*i+17*rd/4, yy0+round(scale*val3[i]),
			 3, f );
	}

	_g_rc3( t1, t2, t3, f);
}

g_graph( n, names, values, t, f )

int     n;
char    *names[], *t;
float   values[];
{
	float   min, max, scale;
	int     rd, rh;
	int     i;

	int     ly;
	int     yyy0;

	erase();
	setspace();
	setsize();

	ly = LY;
	yyy0 = Y0;

	for( i = 1, min = max = values[0] ; i < n ; i++ ) {
		max = (values[i]>max) ? values[i] : max;
		min = (values[i]<min) ? values[i] : min;
	}

	f |= maxset(n,names);

	if( f & VERTICAL )
		_corry( names, n, &yyy0, &ly );

	if( min >= 0. ) {
		min = 0.;
	} else if( max <=  0. )
		max = 0.;

	rh = RH(n);
	rd = rh/3;

	if( min == max )
		scale = 1.;
	else
		scale = ly/(max-min);

	_axisXY( n, names, min, max, yyy0, ly, f );

	color( 9 );

	move( X0+2*rd, yyy0+round(scale*(values[0]-min)) );
	point( X0+2*rd, yyy0+round(scale*(values[0]-min)) );

	for( i = 0 ; i < n ; i++ ) {
		cont( X0+2*rd+rh*i, yyy0+round(scale*(values[i]-min)) );
	}
	color( 1 );

	_title( MX/2, Dy, t, 40 );
}

g_2graph( n, names, val1, val2, t1, t2, f )

int     n, f;
char    *names[], *t1, *t2;
float   val1[], val2[];
{
	float   min, max, scale;
	int     rd, rh;
	int     i;

	int     ly;
	int     yyy0;

	erase();
	setspace();
	setsize();

	ly = LY;
	yyy0 = Y0;

	for( i = 1, max = val1[0], min = val1[0] ; i < n ; i++ ) {
		max = (val1[i]>max) ? val1[i] : max;
		min = (val1[i]<min) ? val1[i] : min;
	}
	for( i = 0 ; i < n ; i++ ) {
		max = (val2[i]>max) ? val2[i] : max;
		min = (val2[i]<min) ? val2[i] : min;
	}

	f |= maxset(n,names);

	if( f & VERTICAL )
		_corry( names, n, &yyy0, &ly );

	if( min >= 0. ) {
		min = 0.;
	} else if( max <=  0. )
		max = 0.;

	rh = RH(n);
	rd = rh/3;

	if( min == max )
		scale = 1.;
	else
		scale = ly/(max-min);

	_axisXY( n, names, min, max, yyy0, ly, f );

	color( 9 );

	move( X0+2*rd, yyy0+round(scale*(val1[0]-min)) );
	point( X0+2*rd, yyy0+round(scale*(val1[0]-min)) );

	for( i = 0 ; i < n ; i++ ) {
		cont( X0+2*rd+rh*i, yyy0+round(scale*(val1[i]-min)) );
	}

	color( 10 );

	linemod( "longdashed" );

	move( X0+2*rd, yyy0+round(scale*(val2[0]-min)) );
	point( X0+2*rd, yyy0+round(scale*(val2[0]-min)) );

	for( i = 0 ; i < n ; i++ ) {
		cont( X0+2*rd+rh*i, yyy0+round(scale*(val2[i]-min)) );
	}

	color( 1 );

        linemod( "solid" );
	_g_ln2( t1, t2, f);
}

g_3graph( n, names, val1, val2, val3, t1, t2, t3, f )

int     n, f;
char    *names[], *t1, *t2, *t3;
float   val1[], val2[], val3[];
{
	float   min, max, scale;
	int     rd, rh;
	int     i;

	int     ly;
	int     yyy0;

	erase();
	setspace();
	setsize();

	ly = LY;
	yyy0 = Y0;

	for( i = 1, max = val1[0], min = val1[0] ; i < n ; i++ ) {
		max = (val1[i]>max) ? val1[i] : max;
		min = (val1[i]<min) ? val1[i] : min;
	}
	for( i = 0 ; i < n ; i++ ) {
		max = (val2[i]>max) ? val2[i] : max;
		min = (val2[i]<min) ? val2[i] : min;
	}
	for( i = 0 ; i < n ; i++ ) {
		max = (val3[i]>max) ? val3[i] : max;
		min = (val3[i]<min) ? val3[i] : min;
	}

	f |= maxset(n,names);

	if( f & VERTICAL )
		_corry( names, n, &yyy0, &ly );

	if( min >= 0. ) {
		min = 0.;
	} else if( max <=  0. )
		max = 0.;

	rh = RH(n);
	rd = rh/3;

	if( min == max )
		scale = 1.;
	else
		scale = ly/(max-min);

	_axisXY( n, names, min, max, yyy0, ly, f );


	color( 9 );

	move( X0+2*rd, yyy0+round(scale*(val1[0]-min)) );
	point( X0+2*rd, yyy0+round(scale*(val1[0]-min)) );

	for( i = 0 ; i < n ; i++ ) {
		cont( X0+2*rd+rh*i, yyy0+round(scale*(val1[i]-min)) );
	}

	color( 10 );

	linemod( "longdashed" );

	move( X0+2*rd, yyy0+round(scale*(val2[0]-min)) );
	point( X0+2*rd, yyy0+round(scale*(val2[0]-min)) );

	for( i = 0 ; i < n ; i++ ) {
		cont( X0+2*rd+rh*i, yyy0+round(scale*(val2[i]-min)) );
	}

	color( 11 );

	linemod( "dotted" );

	move( X0+2*rd, yyy0+round(scale*(val3[0]-min)) );
	point( X0+2*rd, yyy0+round(scale*(val3[0]-min)) );

	for( i = 0 ; i < n ; i++ ) {
		cont( X0+2*rd+rh*i, yyy0+round(scale*(val3[i]-min)) );
	}

	color( 1 );

	linemod( "solid" );

	_g_ln3( t1, t2, t3, f);
}
