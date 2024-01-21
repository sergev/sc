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

extern FILE *OutPlot;

# define _sx	500
# define _sy	500
# ifdef VENIX
# define _dx    12
# define _dy    18
# else
# define _dx    8
# define _dy    10
# endif

static float factor = 1.;
static float scale;

static putsi(a){
	putc((char)a,OutPlot);
	putc((char)(a>>8),OutPlot);
}

p_arc(xi,yi,x0,y0,x1,y1){
	putc('a',OutPlot);
	putsi(xi);
	putsi(yi);
	putsi(x0);
	putsi(y0);
	putsi(x1);
	putsi(y1);
}

p_box(x0, y0, x1, y1)
{
	p_move(x0, y0);
	p_cont(x0, y1);
	p_cont(x1, y1);
	p_cont(x1, y0);
	p_cont(x0, y0);
	p_move(x1, y1);
}

p_circle(x,y,r){
	putc('c',OutPlot);
	putsi(x);
	putsi(y);
	putsi(r);
}

p_closepl(){
	fflush(OutPlot);
}

p_cont(xi,yi){
	putc('n',OutPlot);
	putsi(xi);
	putsi(yi);
}

p_dot(xi,yi,dx,n,pat)
int  pat[];
{
	int i;
	putc('d',OutPlot);
	putsi(xi);
	putsi(yi);
	putsi(dx);
	putsi(n);
	for(i=0; i<n; i++)putsi(pat[i]);
}

p_erase(){
	putc('e',OutPlot);
}

p_label(s)
char *s;
{
	int i;
	putc('t',OutPlot);
	for(i=0;s[i];)putc(s[i++],OutPlot);
	putc('\n',OutPlot);
}

p_line(x0,y0,x1,y1){
	putc('l',OutPlot);
	putsi(x0);
	putsi(y0);
	putsi(x1);
	putsi(y1);
}

p_linemod(s)
char *s;
{
	int i;
	putc('f',OutPlot);
	for(i=0;s[i];)putc(s[i++],OutPlot);
	putc('\n',OutPlot);
}

p_move(xi,yi){
	putc('m',OutPlot);
	putsi(xi);
	putsi(yi);
}

p_openpl(){
}

p_point(xi,yi){
	putc('p',OutPlot);
	putsi(xi);
	putsi(yi);
}

p_space(x0,y0,x1,y1){
	putc('s',OutPlot);
	putsi(x0);
	putsi(y0);
	putsi(x1);
	putsi(y1);
	scale = _sx/(float)(x1-x0);
	if( ((float)(y1-y0))*scale*factor > _sy )
		scale = _sy/((float)(y1-y0)*factor);
}

p_color()
{
}

p_charsize( x,y )
int *x, *y;
{
	*x = (int)(_dx/scale)+1;
	*y = (int)(_dy/(factor*scale))+1;
}
