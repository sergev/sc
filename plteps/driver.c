/*
 *      SC - spreadsheet calculator, varsion 3.0.
 *
 *      (c) Copyright DEMOS, 1989
 *      All rights reserved.
 *
 *      Authors: Serg I. Ryshkov, Serg V. Vakulenko
 *
 *      Mon Apr 03 10:28:41 MSK 1989
 */

# include <stdio.h>
# if __MSDOS__
# include <fcntl.h>
# endif

FILE *OutHard;

int formfeed;
float deltx;
float delty;

main(argc,argv)  char **argv; {
	int std=1;
	FILE *fin;

	OutHard = stdout;

# if __MSDOS__
	setmode (fileno (stdout), O_BINARY);
# endif
	while(argc-- > 1) {
		if(*argv[1] == '-')
			switch(argv[1][1]) {
				case 'f':
					++formfeed;
					break;
				case 'l':
					deltx = atoi(&argv[1][2]) - 1;
					break;
				case 'w':
					delty = atoi(&argv[1][2]) - 1;
					break;
				}

		else {
			std = 0;
# if __MSDOS__
			if ((fin = fopen(argv[1], "rb")) == NULL) {
# else
			if ((fin = fopen(argv[1], "r")) == NULL) {
# endif
				fprintf(stderr, "can't open %s\n", argv[1]);
				exit(1);
				}
			fplt(fin);
			}
		argv++;
		}
	if (std)
		fplt( stdin );
	if (formfeed)
		putchar ('\f');
	exit(0);
	}


fplt(fin)  FILE *fin; {
	int c;
	char s[256];
	int xi,yi,x0,y0,x1,y1,r,dx,n,i;
	int pat[256];

	h_openpl();
	while((c=getc(fin)) != EOF){
		switch(c){
		case 'm':
			xi = getsi(fin);
			yi = getsi(fin);
			h_move(xi,yi);
			break;
		case 'l':
			x0 = getsi(fin);
			y0 = getsi(fin);
			x1 = getsi(fin);
			y1 = getsi(fin);
			h_line(x0,y0,x1,y1);
			break;
		case 't':
			getstr(s,fin);
			h_label(s);
			break;
		case 'e':
			h_erase();
			break;
		case 'p':
			xi = getsi(fin);
			yi = getsi(fin);
			h_point(xi,yi);
			break;
		case 'n':
			xi = getsi(fin);
			yi = getsi(fin);
			h_cont(xi,yi);
			break;
		case 's':
			x0 = getsi(fin);
			y0 = getsi(fin);
			x1 = getsi(fin);
			y1 = getsi(fin);
			h_space(x0,y0,x1,y1);
			break;
		case 'a':
			xi = getsi(fin);
			yi = getsi(fin);
			x0 = getsi(fin);
			y0 = getsi(fin);
			x1 = getsi(fin);
			y1 = getsi(fin);
			h_arc(xi,yi,x0,y0,x1,y1);
			break;
		case 'c':
			xi = getsi(fin);
			yi = getsi(fin);
			r = getsi(fin);
			h_circle(xi,yi,r);
			break;
		case 'f':
			getstr(s,fin);
			h_linemod(s);
			break;
		case 'd':
			xi = getsi(fin);
			yi = getsi(fin);
			dx = getsi(fin);
			n = getsi(fin);
			for(i=0; i<n; i++)pat[i] = getsi(fin);
			h_dot(xi,yi,dx,n,pat);
			break;
		case 'M':
			x0 = getsi(fin);
			y0 = getsi(fin);
			x1 = getsi(fin);
			y1 = getsi(fin);
			h_mapcolor(x0,y0,x1,y1);
			break;
		case 'C':
			xi = getsi(fin);
			h_color(xi);
			break;
			}
		}
	h_closepl();
	}
getsi(fin)  FILE *fin; {        /* get an integer stored in 2 ascii bytes. */
	short a, b;
	if((b = getc(fin)) == EOF)
		return(EOF);
	if((a = getc(fin)) == EOF)
		return(EOF);
	a = a<<8;
	return(a|b);
}
getstr(s,fin)  char *s;  FILE *fin; {
	for( ; *s = getc(fin); s++)
		if(*s == '\n')
			break;
	*s = '\0';
	return;
}
