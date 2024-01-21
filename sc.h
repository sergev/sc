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

/* if sizeof (double) >= 2 * sizeof (char *) */
# define OPTIM                  /* optimize expression tree references */
/* end if */

# define DEFCOLS 16                     /* default table width */
# define DEFROWS 64			/* default table height */
# define MAXHEIGHT (32767/sizeof(int*)) /* maximum table height */
# define MAXWIDTH (26+26*26)            /* maximum table width */

# define DEFWIDTH 10                    /* default column width */
# define DEFPREC 0                      /* default column precision */

# define RESCOL 6               /* columns reserved for row numbers */
# define INFROW 0               /* global table info */
# define NUMROW 1               /* column numbers */
# define TOPROW 2               /* table top row */
# define BOTROW (LINES - 4)     /* table bottom row + 1 */
# define STAROW (LINES - 4)     /* current cell ctatus */
# define PRMROW (LINES - 3)     /* command line */
# define HELROW (LINES - 2)     /* help string */
# define ERRROW (LINES - 1)     /* error messages */

# define MAXPROP 200            /* recalculation loop limit */

# define DEFCOLDELIM '\t'       /* default tbl column delimiter */
# define WORKFILE "workfile.sc" /* default file name */
# define BADREFERENCE "??"

/* default file extensions */

# define EXTSC	"sc"
# define EXTLP	"lp"
# define EXTTBL	"tbl"
# define EXTPLT	"plt"

# define PI (double)3.1415926535897932384626433832795028841971694
# define E (double)2.718281828459045235360287471352662497757247

/* op values */

# define ACOS   1
# define ASIN   2
# define ATAN   3
# define CEIL   4
# define COS    5
# define EXP    6
# define FABS   7
# define FLOOR  8
# define HYPOT  9
# define LOG    10
# define LOG10  11
# define POW    12
# define SIN    13
# define SQRT   14
# define TAN    15
# define DTR    16
# define RTD    17
# define MIN    18
# define MAX    19
# define VAR    20
# define CONST  21
# define FIXED  22
# define RADD   23
# define RSUB   24
# define RMUL   25
# define RDIV   26
# define ADD    27
# define SUB    28
# define MUL    29
# define DIV    30
# define UMINUS 31
# define ROOT   32
# define NOT    33
# define SQUARE 34
# define OR     35
# define AND    36
# define QUEST  37
# define COLON  38
# define EQ     39
# define NE     40
# define LT     41
# define GT     42
# define LE     43
# define GE     44
# define ROUND  45
# define INT    46

/* flag values */

# define is_value	0400	/* cell contains value or formula */
# define is_changed	0200	/* cell is changed since last refresh */
# define is_leftflush	0100	/* cell is leftaligned */
# define is_rightflush	0040	/* cell is rightaligned */
# define is_deleted	0020	/* cell was deleted (for syncrefs) */
# define is_error	0010	/* cell is invalid (if is_value) */
# define is_locked	0004	/* tags for eval */
# define is_computed	0002
# define is_delayed	0001

# define is_center	(is_leftflush|is_rightflush)	/* cell is centered */

# define RIGHTALIGN(f)	(((f)&is_center)==is_rightflush)
# define LEFTALIGN(f)	(((f)&is_center)==is_leftflush)
# define CENTER(f)	(((f)&is_center)==is_center)
# define DFLTALIGN(f)	(((f)&is_center)==0)

/* lrflag values */

# ifdef OPTIM
#    define LVAR        001     /* left child is reference to cell */
#    define LFIXED      002     /* left reference (if LVAR) is 'fixed' */
#    define RVAR        010     /* right child is reference to cell */
#    define RFIXED      020     /* right reference (if RVAR) is 'fixed' */
# endif

# define ctl(c) (c & 037)

# ifdef MESGFILE

# ifdef RUS
extern char *rmesg (), *lmesg ();
extern rusout, rusin;
# define MSG(n) (rusout ? rmesg (n) : lmesg (n))
# define HLP(n) (rusin ? rmesg (n) : lmesg (n))
# define CASE(l,r) case l: case (r) & 0377
# else
extern char *lmesg ();
# define MSG(n) lmesg (n)
# define HLP(n) lmesg (n)
# define CASE(l,r) case l
# endif

# else /* MESGFILE */

# ifdef RUS
extern char *rmesg[], *lmesg[];
extern rusout, rusin;
# define MSG(n) (rusout ? rmesg [n] : lmesg [n])
# define HLP(n) (rusin ? rmesg [n] : lmesg [n])
# define CASE(l,r) case l: case (r) & 0377
# else
extern char *lmesg[];
# define MSG(n) lmesg [n]
# define HLP(n) lmesg [n]
# define CASE(l,r) case l
# endif

# endif /* MESGFILE */

struct ent {                    /* table cell */
	union {
		double value;           /* value (if is_value) */
		struct ent *prev;	/* ptr to previous in queue (eval) */
	} d;
	union {
		char *label;            /* text string (if !is_value) */
		struct enode *expr;     /* expression or 0 (if is_value) */
	} p;
	struct ent *next;       /* ptr to next deleted or next in queue */
	short flags;            /* flags, etc */
	short prec;		/* precision */
	short row;              /* row number */
	short col;              /* column number */
};

struct enode {                  /* expression tree node */
	short op;               /* operation code */
	union {
		double k;               /* constant (if op == CONST) */
		struct ent *v;          /* cell reference (if op == VAR) */
		struct {
			struct enode *left;     /* left child */
			struct enode *right;    /* right child */
# ifdef OPTIM
			short lrflag;           /* left-right child tags */
# endif
		} o;
	} e;
};

extern struct ent ***tbl;       /* cell table */
extern char *fwidth;            /* column width */
extern char *hiddncol;          /* column hidden flag */
extern char *hiddnrow;          /* row hidden flag */
extern struct ent *to_fix;      /* pointer to deleted entries (for Undelete) */
extern MAXROWS, MAXCOLS;        /* current table height and width */
extern currow, curcol;          /* current cell */
extern maxrow, maxcol;          /* last used cell */
extern FullUpdate;              /* sheet was globally changed */
extern changed;                 /* sheet was changed since last EvalAll */
extern modflg;                  /* sheet was modified and must be saved */
extern char line [100];         /* input line */
extern linelim;                 /* input line end */
extern precision;         	/* global default precision */
extern char curfile [100];      /* current file name */

struct ent *lookat ();
char *coltoa ();
