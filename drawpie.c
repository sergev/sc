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

# define MOVE(x,y) move((int)(a1*(x)+b1*(y)+x0+0.5),(int)(a2*(x)+b2*(y)+yy0+0.5))
# define CONT(x,y) cont((int)(a1*(x)+b1*(y)+x0+0.5),(int)(a2*(x)+b2*(y)+yy0+0.5))

extern char *dtoaf (), *sprint ();

_piechart( x, y, r, n, names, values, m, f )

int     x, y, r;
int     n, m;
char    *names[];
float   values[];
{
	float sum, arg, darg, val, otherbnd, othersum, g1, g2;
	char s[MAXTITLE], *p;
	int dx, dy;
	int i, k, js, jc, oc, jm, np;
	int ovl, ql, q, q0, yq0, yl, yq;
	int xx, yy, xd, yd;

# if (defined(CCONTUR) || ! defined(XPLOT) ) && ( !defined(__MSDOS__) || !__MSDOS__ ) && ( ! defined(M_XENIX) )
	f |= BOUND;
# endif

# ifdef ROTATE
	f |= AUTOROT;
# endif
	if( emulate )
		f |= BOUND;

	for( i = 0 , sum = 0. ; i < n ; sum += abs(values[i]), i++ );

	oc = 0;
	np = n;

	if( sum == 0. ) {
		if( f & BOUND )
			grarc( x, y, r, 0., 2*Pi );
		return;
	}

	if( f & OTHERS ) {
		otherbnd = sum/36;
		othersum = 0.;
		for( i = 0 ; i < n ; i++ )
			if( abs(values[i]) < otherbnd ) {
				oc ++;
				othersum += abs(values[i]);
			}
		if( oc > 1 && othersum > 0 && oc < n-2 )
			np ++;
	}

	if( f & AUTOROT && n > 0 ) {
		jm = 0;
		q = yq = 0;
		ovl = MAXINT;

		for( i = 0 , jc = 0 , arg = PIEINIT ; i < np ; i++, jc++ ) {

			jc = jc % ( (f & OTHERS)?(n+1):n);
			if( jc >= n )
				val = othersum;
			else
				val = values[jc];

			if( jc < n && oc > 1 && oc < n-1 && val < otherbnd )
				goto Skip1;
			darg = Pi*abs(val)/sum;

			if( val < 0 ) {
				xd = round(0.2*r*cos(arg+darg));
				yd = -round(0.2*r*sin(arg+darg));
			} else {
				xd = yd = 0;
			}

			if( i > 0 ) {
				ql = q;
				yl = yq;
                	}

			q = (arg+darg >= Pi/2 && arg+darg < 3*Pi/2);
			yq = round(y-1.2*r*sin(arg+darg))+yd;

			if( i == 0 ) {
                               	q0 = q;
				yq0 = yq;
			} else if( q == ql && abs(yq-yl) < ovl ) {
				ovl = abs(yq-yl);
				jm = jc;
			}

                	arg += Pi2*abs(val)/sum;

Skip1: 		        ;
		}

                if( q == q0 && abs(yq-yq0) < ovl )
			jm = 0;

		jc = jm;
	} else
		jc = 0;

	for( i = 0 , arg = PIEINIT, js = jc ; i < np ; i++, jc++ ) {

		jc = jc % ( (f & OTHERS)?(n+1):n);
		if( jc >= n )
			val = othersum;
		else
			val = values[jc];

		if( jc < n && oc > 1 && oc < n-1 && val < otherbnd )
			goto Skip;

		darg = Pi*abs(val)/sum;

		if( val < 0 ) {
			xd = round(0.2*r*cos(arg+darg));
			yd = -round(0.2*r*sin(arg+darg));
		} else {
			xd = yd = 0;
		}

		xx  = x+xd;
		yy  = y+yd;

		g1 = 2*Pi-RGRAD(arg+Pi2*abs(val)/sum);
		g2 = 2*Pi-RGRAD(arg);
		sector( xx, yy, r, g1, g2, js );

		if( f & BOUND ) {

			grarc( xx, yy, r, g1, g2 );
			grline( xx, yy, r, -RGRAD(arg) );

			if( val < 0 || (jc < n-1  && values[jc+1] < 0)
				    || (jc >= n-1 && values[0] < 0) )
				grline( xx, yy, r, -RGRAD(arg+Pi2*abs(val)/sum) );

		}

		if( jc < n )
			p = names[jc];
		else
			p = "Others";
		for( k = 0 ; k < ((f&PERSENT)?(m>5?(m-5):1):m) && p[k] ; k++ )
			s[k] = p[k];
		s[k] = '\0';

		if( f & PERSENT )
			sprint( s+k , "(%s%%)", dtoaf( 100*(abs(val)/sum), 1 ) );

		dx = (arg+darg >= Pi/2 && arg+darg < 3*Pi/2) ? - (strlen(s)*Dx-Dx/2) : Dx/2;
		dy = 0;

		move( round(x+1.2*r*cos(arg+darg))+dx+xd,
		      round(y-1.2*r*sin(arg+darg))+dy+yd );
		label( s );

		if( f & SLASH ) {
			move( round(x+0.95*r*cos(arg+darg)+xd),
			      round(y-0.95*r*sin(arg+darg))+yd );
			cont( round(x+1.15*r*cos(arg+darg))+xd,
			      round(y-1.15*r*sin(arg+darg))+yd );

		}

		arg += Pi2*abs(val)/sum;
		js = (js+1) % np;

Skip:           ;
	}
	if( f & BOUND )
		grline( xx, yy, r, RGRAD(PIEINIT) );
}

g_piechart ( n, nm, val, t, f )

int     n;
char    *nm[], *t;
float   val[];
{
	erase();
	setspace();
	setsize();
	_piechart( MX/2, MY/2, 3*(MY/10), n, nm, val , 13, f );
	_title( MX/2, Dy, t, 60 );
}

g_pie2chart( n, nm, val1, val2, t1, t2, f )

int     n;
char    *nm[], *t1, *t2;
float   val1[], val2[];
{
	erase();
	setspace();
	setsize();
	_piechart( (int)(3l*MX/10l), (int)(13l*MY/20l), (int)(3l*MY/20l), n, nm, val1, 13, f );
	_title( (int)(3l*MX/10l), (int)(7l*MY/20l)+Dy, t1, 30 );
	_piechart( (int)(7l*MX/10l), (int)(3l*MY/10l), (int)(3l*MY/20l), n, nm, val2, 13, f );
	_title( (int)(7l*MX/10l), Dy, t2, 30 );
}

g_pie3chart( n, nm, val1, val2, val3, t1, t2, t3, f )

int     n;
char    *nm[], *t1, *t2, *t3;
float   val1[], val2[], val3[];
{
	erase();
	setspace();
	setsize();
	_piechart( MX/4, (int)(29l*MY/40l), MY/8, n, nm, val1, 13, f );
	_title( MX/4, (int)(48l*MY/100l), t1, 30 );
	_piechart( (int)(3l*MX/4l), (int)(29l*MY/40l), MY/8, n, nm, val2, 13, f );
	_title( (int)(3l*MX/4l), (int)(48l*MY/100l), t2, 30 );
	_piechart( MX/2, (int)(11l*MY/40l), MY/8, n, nm, val3, 13, f );
	_title( MX/2, Dy, t3, 30 );
}

/*                              does it realy need ?
static float fiarg( x,y )
float   x,y;
{
	if( x >= 0.0 )
		if( y >= 0.0 )
			return( atan2(y,x) );
		else
			return( 2*Pi-atan2(-y,x) );
	else
		if( y >= 0.0 )
			return( Pi-atan2(y,-x) );
		else
			return( Pi+atan2(-y,-x) );
}
*/

i_sector( x0, yy0, ir, fi1, fi2, c )
float fi1, fi2;
{
	float   dfi, r, sd, sf;

	while( fi1 < 0. || fi2 < 0. ) {
		fi1 += 2*Pi;
		fi2 += 2*Pi;
	}
	r = ir;

# ifdef DEBUG
	printf("sector : fi1=%f fi2=%f r=%f\n", fi1, fi2, r);
# endif
	if( fi2 < fi1 )
		dfi = 2*Pi-fi1+fi2;
	else
		dfi = fi2-fi1;

	while( fi1 < 0. )
		fi1 += 2*Pi;

	color( c+2 );

	c = (c-1)%12;

	sd = (c < 6) ? CH1 : CH2;

	if( c%6 < 4 ) {
		sf = Pi*c/4;
		_gsect( (float)x0 ,(float)yy0, r, fi1, dfi, sf, sd );
        } else {
		sf = (c%2)*Pi/4;
		_gsect( (float)x0 ,(float)yy0, r, fi1, dfi, sf, sd );
		_gsect( (float)x0 ,(float)yy0, r, fi1, dfi, sf+Pi/2, sd );
	}

	color(1);
}

static _gsect( x0, yy0, r, fi1, dfi, sf, sd)

float   x0, yy0 ,r, fi1, dfi, sf, sd;
{
	float a1, a2, b1, b2;

# ifdef DEBUG
	printf("gsect : sf=%f sd=%f\n", sf, sd);
# endif
	bglevel = 0;

	a1 =  cos(sf);
	b1 = -sin(sf);
	a2 =  sin(sf);
	b2 =  cos(sf);

	_bgsect( x0, yy0, r, fi1-sf, dfi, sd, a1, a2, b1, b2);
}

static _bgsect( x0, yy0, r, fi1, dfi, sd, a1, a2, b1, b2)

float   x0, yy0 ,r, fi1, dfi, sd, a1, a2, b1, b2;
{
	float d;

# ifdef DEBUG
	printf("bgsect: fi1=%f dfi=%f a1=%f a2=%f b1=%f b2=%f\n",fi1,dfi,a1,a2,b1,b2);
# endif

	if( ++bglevel > 10 ) {
		printf("bgsect: recursive level too large.");
		return;
	}

	if( dfi >= -EPS && dfi <= EPS )
		return;

	if( fi1 >= 2*Pi )
		_bgsect( x0, yy0, r, fi1-2*Pi, dfi, sd, a1, a2, b1, b2 );
	else if( fi1 < 0. )
		_bgsect( x0, yy0, r, fi1+2*Pi, dfi, sd, a1, a2, b1, b2 );
	else if( (int)(fi1/Pi) != (int)((fi1+dfi)/Pi) ) {
		for (d=0.; d<=fi1; d+=Pi);
/*
		d = ((int)(fi1/Pi))+1;
		d = d * Pi;
*/
		if( fi1 < Pi )
			_elem( x0, yy0, r, fi1, d-fi1, sd, a1, a2, b1, b2 );
		else
			_elem(x0, yy0, r, 0., d-fi1, sd, a1, a2, -b1, -b2);
		_bgsect( x0, yy0, r, d, fi1+dfi-d, sd, a1, a2, b1, b2 );
	} else if( fi1 >= Pi )
		_bgsect( x0, yy0, r, 2*Pi-fi1-dfi, dfi, sd, a1, a2, -b1, -b2 );
	else
		_elem( x0, yy0, r, fi1, dfi, sd, a1, a2, b1, b2 );
}

static _elem( x0, yy0, r, fi1, dfi, sd, a1, a2, b1, b2 )

float x0, yy0, r, fi1, dfi, sd, a1, a2, b1, b2;
{
	float xl, xr, y, yy1, y2;

	float c1 = cos(fi1);
	float c2 = cos(fi1+dfi);
	float s1 = sin(fi1);
        float s2 = sin(fi1+dfi);

# ifdef DEBUG
	printf("elem : fi1=%f dfi=%f sd=%f a1=%f a2=%f b1=%f b2=%f\n", fi1, dfi, sd, a1, a2, b1, b2);
# endif
	yy1 = s1*r;
	y2 = s2*r;

# ifdef DEBUG
	printf("elem : yy1=%f y2=%f\n", yy1, y2 );
# endif
	for( y = sd/2 ; y < yy1 && y < y2 ; y += sd ) {
		MOVE( r*c1*y/yy1, y );
		CONT( r*c2*y/y2, y );
	}

	for( ; y < yy1 ; y += sd ) {
		MOVE( r*c1*y/yy1, y );
                CONT( -sqrt(r*r-y*y), y );
	}

	for( ; y < y2 ; y += sd ) {
                MOVE( r*c2*y/y2, y );
                CONT( sqrt(r*r-y*y), y );
	}
	if( fi1 < Pi/2. && fi1+dfi > Pi/2. )
		for( ; y < r ; y += sd ) {
        	     	xr = sqrt(r*r-y*y);
			xl = -xr;
			MOVE( xl, y );
			CONT( xr, y );
		}
}
