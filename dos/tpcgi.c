/*
 *       Driver for XENIX CGI interface
 */

# define VECTORFONT
# define PLOT_KIAE
# define FACTOR 1.

/* # define CAN_MSG */

# ifdef CAN_MSG
#    include <stdio.h>
# endif

static short devhandle;                 /* Device handler */
static short savin[20];                 /* Workstation input parameters */
static short savary[66];                /* Workstation output parameters */

static int afterinit;

# define NXS    32767    /* Число точек по X */
# define NYS    32767    /* Число точек по Y */
# define PI     3.14159265359

# define COLOR(j) ((j) ? colortab [((j)-1) % 7] : 0)

static char colortab [7] = { 1, 3, 2, 4, 5, 6, 7, };

static float obotx = 0.;
static float oboty = 0.;
static float botx = 0.;
static float boty = 0.;
static float scalex = 1.;
static float scaley = 1.;
static int _OX, _OY;

# ifdef VECTORFONT

int     _SX, _SY;
# ifdef M_XENIX
int     printer;
# endif

/*
 * Описания к таблице символов
 * Символы закодированы перемещениями пера по
 * следующей матрице:
 *
 *  I S c m w   - размер прописной буквы
 *  H R b l v
 *  G Q a k u
 *  F.P.Z.j.t   - размер строчной буквы
 *  E O*Y*i s
 *  D N X h r
 *  C M W g q
 *  B.L.V.f.p.z - уровень строки
 *  A K U e o y
 *  @ J T d n x - место для подстрочных символов
 *
 * При этом разные линии (переход между которыми требует
 * поднятия пера) разделяются пробелами.
 * Таблица начинается с символа 040
 * После символа 0177 следует символ 0300
 *
 *      From (C) Руднев А.П. (KIAE)
 */
typedef char *tFONT[0140+0100];

# define X_PSIZE 6
# define Y_PSIZE 10
# define VF(c) ( (((c)&0340)==0 || ((c)&0340)==0200)? "":(c&0200)?VFONT[(c&0377)-0140]:VFONT[((c)&0377)-040] )
# define IndexC(c) ((c)>'Z'?(c)-'a'+('Z'-'@'+1):(c)-'@')
# define XP(i,sx) ((i/10)*(long)(sx)/X_PSIZE)
# define YP(i,sy) ((i%10-2)*(long)(sy)/Y_PSIZE)


tFONT VFONT= {
/* " " */ "",
/* "!" */ "VW cX",
/* """ */ "QS mk",
/* "#" */ "MQ kg rD Ft",
/* "$" */ "CgrsjPQRv cV",
/* "%" */ "Bw SIHRS pfgqp",
/* "&" */ "shgVLCabSHGp",
/* "'" */ "acma",

/* "(" */ "mbWf",
/* ")" */ "SbWL",
/* "*" */ "YGYaYuYsYqYWYCYEY",
/* "+" */ "Es aW",
/* "," */ "KgWV",
/* "-" */ "Es",
/* "." */ "LMWVL",
/* "/" */ "Bw",

/* "0" */ "CHSmvqfLC Bw",
/* "1" */ "QcVLf",
/* "2" */ "HSmvtBp",
/* "3" */ "IwZjsqfLC",
/* "4" */ "fmDr",
/* "5" */ "CLfqsjFIw",
/* "6" */ "FjsqfLCHSmv",
/* "7" */ "IwB",

/* "8" */ "PGHSmvujPECLfqsj",
/* "9" */ "LfqvmSHFOs",
/* ":" */ "ZakjZ VWgfV",
/* ";" */ "ZakZ KgWV",
/* "<" */ "kOg",
/* "=" */ "Dr Ft",
/* ">" */ "QiM",
/* "?" */ "RcmvuYX WV",

/* "@" */ "hjZONWgrtkQFCLf",
/* "A" */ "BGSmup Es",
/* "B" */ "FjuvmIBfqsj",
/* "C" */ "wSHCLp",
/* "D" */ "BfqvmIB",
/* "E" */ "wIBp jF",
/* "F" */ "BIw iE",
/* "G" */ "wSHCLprh",

/* "H" */ "BI Ft wp",
/* "I" */ "LfVcSm",
/* "J" */ "CLWcSm",
/* "K" */ "BI wFp",
/* "L" */ "IBp",
/* "M" */ "BIZwp",
/* "N" */ "BIpw",
/* "O" */ "CHSmvqfLC",

/* "P" */ "BImvtiE",
/* "Q" */ "CHSmvqfLC Xp",
/* "R" */ "BImclkZFZp",
/* "S" */ "BfqsjPGHSw",
/* "T" */ "Iw cV",
/* "U" */ "ICLfqw",
/* "V" */ "IVw",
/* "W" */ "ILYfw",

/* "X" */ "Bw Ip",
/* "Y" */ "IZw ZV",
/* "Z" */ "IwBp Oi",
/* "[" */ "mcVf",
/* "\" */ "Ip",
/* "]" */ "LVcS",
/* "^" */ "Eas",
/* "_" */ "Bz",

/* "`" */ "SkcS",
/* "a" */ "PjspLCDOs",
/* "b" */ "IBfqsjF",
/* "c" */ "tPECLp",
/* "d" */ "tPECLpw",
/* "e" */ "DrsjPECLp",
/* "f" */ "mbV Oi",
/* "g" */ "pLCEPtodJ",

/* "h" */ "BIFjsp",
/* "i" */ "ba ZWf",
/* "j" */ "AKVZ ab",
/* "k" */ "BIDtDp",
/* "l" */ "SbV",
/* "m" */ "BEPYVYjsp",
/* "n" */ "BFEPjsp",
/* "o" */ "CEPjsqfLC",

/* "p" */ "@FjsqfLC",
/* "q" */ "qfLCEPtn",
/* "r" */ "BFEPt",
/* "s" */ "BfqhNEPt",
/* "t" */ "cWf Pj",
/* "u" */ "FCLfqtp",
/* "v" */ "FVt",
/* "w" */ "FLXft",

/* "x" */ "FXBtXp",
/* "y" */ "FCLfqtodJ",
/* "z" */ "FtBp",
/* "{" */ "maYOYXf",
/* "|" */ "cV",
/* "}" */ "SaYiYXL",
/* "~" */ "EPhs",
/* DEL */ "BIwpBqDsFuHw",

/* "ю" */ "BFDXYjsqfWX",
/* "а" */ "qfLEPjstqz",
/* "б" */ "mSHjsqfLCEPj",
/* "ц" */ "FCLfqzyzqt",
/* "д" */ "cuqfLCEPjs",
/* "е" */ "DrsjPECLfq",
/* "ф" */ "UZXODCLWfqriX",
/* "г" */ "EPZiCLfq",

/* "х" */ "Bt Fp",
/* "и" */ "FBtp",
/* "й" */ "FBtp aZ",
/* "к" */ "BFDXtXp",
/* "л" */ "Btp",
/* "м" */ "BFXtp",
/* "н" */ "FBDrtp",
/* "о" */ "LDPjrfL",

/* "п" */ "BFtp",
/* "я" */ "BXrtZOXrp",
/* "р" */ "AFjsrgC",
/* "с" */ "sjPECLfq",
/* "т" */ "Ft ZV",
/* "у" */ "FDMgrtpeK",
/* "ж" */ "BNFNXZVXhthp",
/* "в" */ "DabSIBfqsjZD",

/* "ь" */ "FBVghYE",
/* "ы" */ "FBVghYE tp",
/* "з" */ "EPjshXqfLC",
/* "ш" */ "FBpt YV",
/* "э" */ "EPjsqfLC Xr",
/* "щ" */ "FBptpyx YV",
/* "ч" */ "FDMqtp",
/* "'" */ "FPLfqriO",

/* "Ю" */ "IB EORcmvqfVMO",
/* "А" */ "BDcwp Dr",
/* "Б" */ "wIBfqsjF",
/* "Ц" */ "IBpw pyx",
/* "Д" */ "ABzy LQcwp",
/* "Е" */ "mIBp Fj",
/* "Ф" */ "MgrulRGDM cV",
/* "Г" */ "BIw",

/* "Х" */ "BW IP",
/* "И" */ "IBwp",
/* "й" */ "IBwp Sbm",
/* "К" */ "IB FZp wZ",
/* "Л" */ "BDcwp",
/* "М" */ "BIYwp",
/* "Н" */ "IB Ft wp",
/* "О" */ "LDGSmurfL",

/* "П" */ "BIwp",
/* "Я" */ "BY sOFHSwp",
/* "Р" */ "BIcvtYE",
/* "С" */ "qfLCHSmv",
/* "Т" */ "HIwv cV",
/* "У" */ "IGPju wqfLC",
/* "Ж" */ "Bw cV pI",
/* "В" */ "FZklcIBfqsZ",

/* "Ь" */ "IBfqsjF",
/* "Ы" */ "IBVgiZF wp",
/* "З" */ "HSmvujsqfLC Zj",
/* "Ш" */ "IBpw bV",
/* "Э" */ "ImvqfB Os",
/* "Щ" */ "IBpw bV pyx",
/* "Ч" */ "IFOs wp",
/* "'" */ "HISLfqsjP"
};


# endif /* VECTORFONT */

static _convert(x, y)
int *x, *y;
{
	long ix, iy;

	ix = (*x-obotx)*scalex + botx;
	iy = (*y-oboty)*scaley + boty;
	if( ix < 0 )     ix = 0;
	if( ix > NXS-1 ) ix = NXS-1;
	if( iy < 0 )     iy = 0;
	if( iy > NYS-1 ) iy = NYS-1;
	*x = ix;
	*y = iy;
}

static cgi_error(func)
char func[];
{
	short errnum;

	errnum = -(vq_error());
	cgi_fatal(errnum,func);
}

/* ARGSUSED */

static cgi_fatal(errnum,func)
short errnum;
char func[];
{
   /* close the workstation */
   v_clswk(devhandle);           /* no recourse on error */

   /* now reopen and reclose it to leave it in default state */
   v_opnwk(savin, &devhandle, savary);
   v_enter_cur(devhandle);       /* (make sure we exit in cursor mode) */
   v_clswk(devhandle);

   /* write fatal message (using stdio) and bug out */
# ifdef CAN_MSG
   fprintf (stderr,"Fatal error no. %d in CGI function %s.\n", (int)errnum, func);
   exit (-2);
# endif
}

t_openpl()
{
   short error;

   /* savin[1] - savin[10] are passed to the driver*/
   savin[0] = 1;                /* don't preserve aspect ratio */
   savin[1] = 1;
   savin[2] = 1;
   savin[3] = 3;
   savin[4] = 1;
   savin[5] = 1;
   savin[6] = 1;
   savin[7] = 0;
   savin[8] = 0;
   savin[9] = 1;
   savin[10] = 1;  /* prompt for paper changes */
# ifdef M_XENIX
    if( printer ) {
	savin[11] = 'P';             /* OPEN CRT DEVICE */
	savin[12] = 'R';
	savin[13] = 'I';
	savin[14] = 'N';
	savin[15] = 'T';
	savin[16] = 'E';
	savin[17] = 'R';
	savin[18] = ' ';
   } else {
	savin[11] = 'D';             /* OPEN CRT DEVICE */
	savin[12] = 'I';
	savin[13] = 'S';
	savin[14] = 'P';
	savin[15] = 'L';
	savin[16] = 'A';
	savin[17] = 'Y';
	savin[18] = ' ';
   }
# else
   savin[11] = 'D';             /* OPEN CRT DEVICE */
   savin[12] = 'I';
   savin[13] = 'S';
   savin[14] = 'P';
   savin[15] = 'L';
   savin[16] = 'A';
   savin[17] = 'Y';
   savin[18] = ' ';
# endif
   /* open the workstation and save output in savary array*/
   error = v_opnwk(savin, &devhandle, savary);
   if (error < 0)
   {
# ifdef CAN_MSG
      fprintf(stderr,"Error %d opening device display\n",vq_error());
      exit (-1);
# endif
   }
}

t_closepl()
{
   /* clear the workstation*/
   if (v_clrwk(devhandle) < 0)
      cgi_error("v_clrwk");
   /* close the workstation*/
   if (v_clswk(devhandle) < 0)
      cgi_error("v_clswk");
}

/*
 * Установка размещения области рисования
 * by @VG & V&S
 */

t_space(x0,y0,x1,y1)
{
	boty = 0.;
	obotx = x0;
	oboty = y0;
# ifndef PLOT_KIAE
	scalex = ((float)NYS)/(x1-x0);
	scaley = ((float)NXS)/(y1-y0);
# else
	scaley = scalex = ((float)NYS)/(x1-x0);
	if( (y1-y0)*scalex*FACTOR > ((float)NXS) )
		scaley = scalex = ((float)NXS)/(((float)(y1-y0))*FACTOR);
# endif
	scaley = scalex = scalex*0.95;
	botx = 0.;     /* Moves graph area to left ... */
# ifdef VECTORFONT
	_SX = (x1-x0)/70;
	_SY = (x1-x0)/50;
# endif
}

t_cont(x1,y1)
int x1,y1;
{
	int x = _OX, y = _OY;
	short ptsin[4];

	_OX = x1; _OY = y1;
	_convert( &x, &y );
	_convert( &x1, &y1 );

	ptsin[0] = x;
	ptsin[1] = y;
	ptsin[2] = x1;
	ptsin[3] = y1;

	if (v_pline(devhandle, 2, ptsin) < 0)
	       cgi_error("v_pline");
	afterinit = 1;
}

t_line(x0,y0,x1,y1)
{
	t_move(x0, y0);
	t_cont(x1, y1);
}

t_point(x, y)
{
	t_line( x, y, x, y );
}

t_linemod(s)
char *s;
{
	if(      !strcmp(s, "dotted") )
		vsl_type( devhandle, 3 );
	else if( !strcmp(s, "dotdashed") )
		vsl_type( devhandle, 4 );
	else if( !strcmp(s, "shortdashed") )
		vsl_type( devhandle, 7 );
	else if( !strcmp(s, "longdashed") )
		vsl_type( devhandle, 2 );
	else /* solid */
		vsl_type( devhandle, 1 );
}

t_move(x, y)
{
	_OX = x; _OY = y;
}

t_erase()
{
       /* clear the workstation*/

       if( ! afterinit ) {
	       afterinit = 0;
	       return;
       }

       if (v_clrwk(devhandle) < 0)
	     cgi_error("v_clrwk");
}

# ifdef VECTORFONT
static _vect(s,dx,dy,x,y)
char *s;         /* Строка */
int  dx,dy, x, y;         /* Размеры символа и нач. координаты */
{
	register char *font;
	register int x1,y1;
	int xyindex;
	int pen;        /* 1 - DOWN, 0 - UP, >1 - rest */
	while(*s) {
		font=VF(*s);
		s++;
		if(!font) goto next;
		pen=0;
		for(;*font;font++)
		{
			if(*font==' ') {
				pen=0;
				continue;
			}
			xyindex = IndexC(*font);
			y1=YP(xyindex,dy) + y;
			x1=XP(xyindex,dx) + x;
			if( pen )
				t_cont(x1,y1);
			else
				t_move(x1,y1);
			pen = 1;
		}
next:
		x += (long)dx;
	}
	t_move(x,y);
}

t_sector( x,y,r,f1,f2,j )
int     x,y,r,j;
float   f1, f2;
{
	int     fi1, fi2, dfi, sx, sy;

	sx = x;
	sy = y;

	_convert( &x,&y );

	fi1 = round(180.*f1/PI);
	fi2 = round(180.*f2/PI);

        if( fi1 == fi2 ) {
		i_grline( sx, sy, r, f1 );
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

	/* setfillstyle(pattern[(j/maxcolor)%MAXFILL],COLOR(j+2)); */
	vsf_color(devhandle, COLOR(j+2));
	if( j < (savary[13]==16?7:savary[13]-1) )
		vsf_interior(devhandle, 1);
	else
		vsf_interior(devhandle, 3);
	vsf_style(devhandle, (j%33)+1);
	if( fi1 < 0  ) {
		v_pieslice( devhandle, x, y, (int)(scalex*r),
			      10*(360+fi1), 3600 );
		v_pieslice( devhandle, x, y, (int)(scalex*r),
			      0, 10*(fi1+dfi) );
	} else
		v_pieslice( devhandle, x, y, (int)(scalex*r),
			      10*fi1, 10*(fi1+dfi) );
	vsf_color(devhandle, 1);
	vsf_interior(devhandle, 0);
	vsf_style(devhandle, 1);
	if( fi1 < 0  ) {
		v_pieslice( devhandle, x, y, (int)(scalex*r),
			      10*(360+fi1), 3600 );
		v_pieslice( devhandle, x, y, (int)(scalex*r),
			      0, 10*(fi1+dfi) );
	} else
		v_pieslice( devhandle, x, y, (int)(scalex*r),
			      10*fi1, 10*(fi1+dfi) );
}

t_grarc( x,y,r,f1,f2 )
int     x,y,r;
float   f1, f2;
{
	int     fi1, fi2, dfi;

	_convert( &x,&y );

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
		v_arc( devhandle, x, y, (int)(scalex*r), 10*(360+fi1), 3600 );
		v_arc( devhandle, x, y, (int)(scalex*r), 0, 10*(fi1+dfi) );
	} else
		v_arc( devhandle, x, y, (int)(scalex*r), 10*fi1, 10*(fi1+dfi) );
}

t_rectan( x1,y1,x2,y2,c )
int     x1,y1,x2,y2,c;
{
	short xy[4];

	_convert( &x1, &y1 );
	_convert( &x2, &y2 );
	xy[0] = x1;
	xy[1] = y1;
	xy[2] = x2;
	xy[3] = y2;

	vsf_color(devhandle, COLOR(c+1));
	if( c-1 < (savary[13]==16?7:savary[13]-1))
		vsf_interior(devhandle, 1);
	else
		vsf_interior(devhandle, 3);
	vsf_style(devhandle, ((c-1)%33)+1);
	/* setfillstyle(pattern[((c-1)/maxcolor)%MAXFILL],COLOR(c+1)); */
	v_bar( devhandle, xy );
}

t_label(s)
register char *s;
{
	_vect(s,_SX,_SY,_OX-_SX/2,_OY-_SY/2);
}

# endif /* VECTORFONT */

t_color(col)
{
# ifdef DEBUG
	printf("max=%d, COLOR=%d, col=%d\n", savary[13], COLOR(col), col);
	if( savary[13] > 0 )
	       printf("vsl=%d\n",vsl_color( devhandle, COLOR(col) ));
# else
	vsl_color( devhandle, COLOR(col) );
# endif
}

/* ARGSUSED */

t_circle(x, y, r)
{
}

t_charsize( x,y )
int *x, *y;
{
# ifndef VECTORFONT
	*x = (int)(8./scalex)+1;
	*y = (int)(10./(FACTOR*scaley))+1;
# else
	*x = _SX;
	*y = _SY;
# endif
}

