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

# define Pi 3.141592653
# define Pi2 (2*Pi)
# define MAXINT 32000
# define PIEINIT (0.)		/* initial pie angle (from 12 hours clockwise */

# define ROTATE

# ifdef VENIX
#    define EPS 0.001
# else
#    define EPS 0.00001
# endif

# if defined (__MSDOS__) || defined (E85DEMOS) || defined (M_XENIX)
#    define GRAPHTTY
# endif

# ifndef TTONLY
#    define GRAPHPLOT
#    ifdef __MSDOS__
#       define HCOPY
#    endif
# endif

# define sign(x) ((x)>0?1:((x)<0?-1:0))
# define abs(x) ((x)>0?(x):-(x))
# define amin(x,y) ((x)<(y)?(x):(y))
# define amax(x,y) ((x)>(y)?(x):(y))

# define DX      152
# define DY      320
# define MX	 12000
# define MY      8000

# define H1      240
# define H2      200
# define H3      160
# define H4      120

# define CH1     160
# define CH2	 80

# define MAXTITLE 128
# define MAX1STYLE 12

# define RH(i) (LX/(i))

# ifdef __MSDOS__
#    define XPLOT
#    define CCONTUR
#    define RCONTUR
#    define RGRADUS
#    define CHSIZE
# endif

# define COLOR

# define RGRAD(g) (rgflag?(Pi/180.*round(180.*(g)/Pi)):(g))

# ifdef DRAWFILE
#    define EXTERN int
# else
#    define EXTERN extern int
# endif

EXTERN Dx;
EXTERN Dy;
EXTERN LX;
EXTERN LY;
EXTERN X0;
EXTERN Y0;

EXTERN bglevel;

float _hfact();

EXTERN emulate;
EXTERN plotflag;
EXTERN ttyflag;
EXTERN rgflag;

