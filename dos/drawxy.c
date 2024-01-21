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

g_xygraph( n, valarg, values, t, f )

int     n;
char    *t;
float   valarg[], values[];
{
	float   xmin, xmax, xscale, ymin, ymax, yscale;
	int     i;

	erase();
	setspace();
	setsize();

	ymin = ymax = values[0] ;
	xmin = xmax = valarg[0] ;
	for( i = 1 ; i < n ; i++ ) {
		ymax = (values[i]>ymax) ? values[i] : ymax;
		ymin = (values[i]<ymin) ? values[i] : ymin;
		xmax = (valarg[i]>xmax) ? valarg[i] : xmax;
		xmin = (valarg[i]<xmin) ? valarg[i] : xmin;
	}

	if( xmin == xmax )
		xscale = 1.;
	else
		xscale = (LX-4*Dx)/(xmax-xmin);

	if( ymin >= 0. ) {
		ymin = 0.;
	} else if( ymax <=  0. )
		ymax = 0.;

	if( ymin == ymax )
		yscale = 1.;
	else
		yscale = LY/(ymax-ymin);

	_axis2XY( xmin, xmax, ymin, ymax, f );

	color( 9 );

	move( X0+round(xscale*(valarg[0]-xmin)) , Y0+round(yscale*(values[0]-ymin)) );
	point( X0+round(xscale*(valarg[0]-xmin)) , Y0+round(yscale*(values[0]-ymin)) );

	for( i = 0 ; i < n ; i++ )
		cont( X0+round(xscale*(valarg[i]-xmin)),
		      Y0+round(yscale*(values[i]-ymin)) );

	color( 1 );

	_title( MX/2, Dy, t, 40 );
}

g_xy2graph( n, valarg, val1, val2, t1, t2, f )

int     n;
char    *t1, *t2;
float   valarg[], val1[], val2[];
{
	float   xmin, xmax, xscale, ymin, ymax, yscale;
	int     i;

	erase();
	setspace();
	setsize();

	ymin = ymax = val1[0] ;
	xmin = xmax = valarg[0] ;
	for( i = 1 ; i < n ; i++ ) {
		ymax = (val1[i]>ymax) ? val1[i] : ymax;
		ymin = (val1[i]<ymin) ? val1[i] : ymin;
		xmax = (valarg[i]>xmax) ? valarg[i] : xmax;
		xmin = (valarg[i]<xmin) ? valarg[i] : xmin;
	}
	for( i = 0 ; i < n ; i++ ) {
		ymax = (val2[i]>ymax) ? val2[i] : ymax;
		ymin = (val2[i]<ymin) ? val2[i] : ymin;
	}

	if( xmin == xmax )
		xscale = 1.;
	else
		xscale = (LX-4*Dx)/(xmax-xmin);

	if( ymin >= 0. ) {
		ymin = 0.;
	} else if( ymax <=  0. )
		ymax = 0.;

	if( ymin == ymax )
		yscale = 1.;
	else
		yscale = LY/(ymax-ymin);

	_axis2XY( xmin, xmax, ymin, ymax, f );

	color( 9 );

	move( X0+round(xscale*(valarg[0]-xmin)) , Y0+round(yscale*(val1[0]-ymin)) );
	point( X0+round(xscale*(valarg[0]-xmin)) , Y0+round(yscale*(val1[0]-ymin)) );

	for( i = 0 ; i < n ; i++ )
		cont( X0+round(xscale*(valarg[i]-xmin)),
		      Y0+round(yscale*(val1[i]-ymin)) );

	color( 10 );

	linemod( "longdashed" );

	move( X0+round(xscale*(valarg[0]-xmin)) , Y0+round(yscale*(val2[0]-ymin)) );
	point( X0+round(xscale*(valarg[0]-xmin)) , Y0+round(yscale*(val2[0]-ymin)) );

	for( i = 0 ; i < n ; i++ )
		cont( X0+round(xscale*(valarg[i]-xmin)),
		      Y0+round(yscale*(val2[i]-ymin)) );

	color( 1 );

	linemod( "solid" );
	_g_ln2( t1, t2, f);
}

g_xy3graph( n, valarg, val1, val2, val3, t1, t2, t3, f )

int     n;
char    *t1, *t2, *t3;
float   valarg[], val1[], val2[], val3[];
{
	float   xmin, xmax, xscale, ymin, ymax, yscale;
	int     i;

	erase();
	setspace();
	setsize();

	ymin = ymax = val1[0] ;
	xmin = xmax = valarg[0] ;
	for( i = 1 ; i < n ; i++ ) {
		ymax = (val1[i]>ymax) ? val1[i] : ymax;
		ymin = (val1[i]<ymin) ? val1[i] : ymin;
		xmax = (valarg[i]>xmax) ? valarg[i] : xmax;
		xmin = (valarg[i]<xmin) ? valarg[i] : xmin;
	}
	for( i = 0 ; i < n ; i++ ) {
		ymax = (val2[i]>ymax) ? val2[i] : ymax;
		ymin = (val2[i]<ymin) ? val2[i] : ymin;
	}

	for( i = 0 ; i < n ; i++ ) {
		ymax = (val3[i]>ymax) ? val3[i] : ymax;
		ymin = (val3[i]<ymin) ? val3[i] : ymin;
	}

	if( xmin == xmax )
		xscale = 1.;
	else
		xscale = (LX-4*Dx)/(xmax-xmin);

	if( ymin >= 0. ) {
		ymin = 0.;
	} else if( ymax <=  0. )
		ymax = 0.;

	if( ymin == ymax )
		yscale = 1.;
	else
		yscale = LY/(ymax-ymin);

	_axis2XY( xmin, xmax, ymin, ymax, f );

	color( 9 );

	move( X0+round(xscale*(valarg[0]-xmin)) , Y0+round(yscale*(val1[0]-ymin)) );
	point( X0+round(xscale*(valarg[0]-xmin)) , Y0+round(yscale*(val1[0]-ymin)) );

	for( i = 0 ; i < n ; i++ )
		cont( X0+round(xscale*(valarg[i]-xmin)),
		      Y0+round(yscale*(val1[i]-ymin)) );

	color( 10 );

	linemod( "longdashed" );

	move( X0+round(xscale*(valarg[0]-xmin)) , Y0+round(yscale*(val2[0]-ymin)) );
	point( X0+round(xscale*(valarg[0]-xmin)) , Y0+round(yscale*(val2[0]-ymin)) );

	for( i = 0 ; i < n ; i++ )
		cont( X0+round(xscale*(valarg[i]-xmin)),
		      Y0+round(yscale*(val2[i]-ymin)) );

	color( 11 );

	linemod( "dotted" );

	move( X0+round(xscale*(valarg[0]-xmin)) , Y0+round(yscale*(val3[0]-ymin)) );
	point( X0+round(xscale*(valarg[0]-xmin)) , Y0+round(yscale*(val3[0]-ymin)) );

	for( i = 0 ; i < n ; i++ )
		cont( X0+round(xscale*(valarg[i]-xmin)),
		      Y0+round(yscale*(val3[i]-ymin)) );

	color( 1 );

	linemod( "solid" );
	_g_ln3( t1, t2, t3, f);
}

_axis2XY( xmin, xmax, ymin, ymax, f)

int     f;
float   xmin, xmax, ymin, ymax;
{
	int     i, ny, yb, nx, xb;
	char    s[33], *p;
	float   hx, hy, x, y, xscale, yscale, x0;

	move( X0,Y0 );
	cont( X0+LX-4*Dx, Y0 );
	move( X0, Y0 );
	cont( X0, Y0+LY );

	if( f & (AREA|GREEDX|GREEDY) ) {
		cont( X0+LX-4*Dx, Y0+LY );
		cont( X0+LX-4*Dx, Y0 );
	}

	hy = _hfact(ymax-ymin);
	ny = (int)((ymax-ymin)/hy);
	if( ny < 2 ) {
		hy /= 2;
		ny = (int)((ymax-ymin)/hy);
	}
	if( ymin == ymax )
		yscale = 1.;
	else
		yscale = LY/(ymax-ymin);

	if( ymin != 0. ) {
		move( X0, Y0+round(-yscale*ymin) );
		cont( X0+LX-4*Dx, Y0+round(-yscale*ymin) );
	}

	for( y = 0., i = 0 ; y <= ymax && i <= ny; y += hy, i++ ) {
		yb = round(yscale*(y-ymin));
		move( X0-Dx/3, Y0+yb );
		cont( X0, Y0+yb );
		if( f & GREEDY )
			cont( X0+LX-4*Dx, Y0+yb );
		move( Dx+Dx/3, Y0+yb );
		strcpy( s, dtoag( y, 8 ) );
		label( s );
	}

	for( y = -hy, i = 0 ; y >= ymin && i <= ny; y -= hy, i++ ) {
		yb = round(yscale*(y-ymin));
		move( X0-Dx/3, Y0+yb );
		cont( X0, Y0+yb );
		if( f & GREEDY )
			cont( X0+LX-4*Dx, Y0+yb );
		move( Dx+Dx/3, Y0+yb );
		strcpy( s, dtoag( y, 8 ) );
		label( s );
	}

	hx = _hfact(xmax-xmin);
	nx = (int)((xmax-xmin)/hx);
	if( nx < 2 ) {
		hx /= 2;
		nx = (int)((xmax-xmin)/hx);
	}
	if( xmin == xmax )
		xscale = 1.;
	else
		xscale = (LX-4*Dx)/(xmax-xmin);

	if( xmin > 0. && xmax > 0. )
		x0 = hx*((long)(xmin/hx)+1);
	else if( xmin < 0. && xmax < 0. )
		x0 = -hx*((long)(-xmin/hx)+1);
	else
		x0 = 0.;

	for( x = x0 , i = 0 ; x <= xmax && i <= nx; x += hx, i++ ) {
		xb = round(xscale*(x-xmin));
		move( X0+xb, Y0-Dy/3 );
		cont( X0+xb, Y0 );
		if( f & GREEDX )
			cont( X0+xb, Y0+LY );
		strcpy( s, dtoag( x, (nx>8) ? 7 : 8 ) );
		for( p = s ; *p == ' ' ; p++ ) ;
		move( X0+xb-Dx*strlen(p)/2, Y0-4*Dy/3 );
		label( p );
	}

	for( x = x0-hx, i = 0 ; x >= xmin && i <= nx; y -= hx, i++ ) {
		xb = round(xscale*(x-xmin));
		move( X0+xb, Y0-Dy/3 );
		cont( X0+xb, Y0 );
		if( f & GREEDX )
			cont( X0+xb, Y0+LY );
		strcpy( s, dtoag( x, (nx>8) ? 7 : 8 ) );
		for( p = s ; *p == ' ' ; p++ ) ;
		move( X0+xb-Dx*strlen(p)/2, Y0-4*Dy/3 );
		label( p );
	}
}

