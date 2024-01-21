/*
 *      int CapInit (buf)
 *      char buf [1024];
 *
 *              - get termcap entry into buffer buf.
 *              Return 1 if ok, else 0.
 *
 *      CapGet (tab, buf)
 *      struct CapTab *tab;
 *      char *buf;
 *
 *              - read terminal properties, described in table tab.
 *              Buffer used as argument to CapInit must be available.
 *              Store strings in buffer buf.
 *
 *      char *CapGoto (movestr, col, line)
 *      char *movestr;
 *
 *              - expand string movestr by column and line numbers.
 */

# ifdef __MSDOS__
CapInit ()
{
	return (1);
}
# else

# include "cap.h"

# define BUFSIZ 1024
# define MAXHOP 32              /* max number of tc= indirections */
# define TERMCAP "/etc/termcap"
# define MAXRETURNSIZE 64
# define CTRL(c) (c & 037)

static char *tname;             /* terminal name */
static char *tbuf;
static hopcount;                /* detect infinite loops in termcap, init 0 */
char *tskip (), *tdecode (), *getenv ();
extern char *strcpy (), *strcat ();

/*
 * Get an entry for terminal name in buffer bp,
 * from the termcap file.  Parse is very rudimentary;
 * we just notice escaped newlines.
 */

CapInit (bp)
char *bp;
{
	if (tbuf)
		return (1);
	if (! (tname = getenv ("TERM")))
		tname = "unknown";
	return (tgetent (bp, tname));
}

static tgetent (bp, name)
char *bp, *name;
{
	register char *cp;
	register int c;
	register int i = 0, cnt = 0;
	char ibuf [BUFSIZ];
	int tf;

	tbuf = bp;
	tf = -1;
	cp = getenv ("TERMCAP");
	/*
	 * TERMCAP can have one of two things in it. It can be the
	 * name of a file to use instead of /etc/termcap. In this
	 * case it better start with a "/". Or it can be an entry to
	 * use so we don't have to read the file. In this case it
	 * has to already have the newlines crunched out.
	 */
	if (cp && *cp) {
		if (*cp == '/') {
			tf = open (cp, 0);
		} else {
			tbuf = cp;
			c = tnamatch (name);
			tbuf = bp;
			if (c) {
				strcpy (bp,cp);
				return (tnchktc ());
			}
		}
	}
	if (tf < 0)
		tf = open (TERMCAP, 0);
	if (tf < 0)
		return (0);
	for (;;) {
		cp = bp;
		for (;;) {
			if (i == cnt) {
				cnt = read(tf, ibuf, BUFSIZ);
				if (cnt <= 0) {
					close(tf);
					return (0);
				}
				i = 0;
			}
			c = ibuf[i++];
			if (c == '\n') {
				if (cp > bp && cp[-1] == '\\'){
					cp--;
					continue;
				}
				break;
			}
			if (cp >= bp+BUFSIZ) {
				outerr ("Termcap entry too long\n");
				break;
			} else
				*cp++ = c;
		}
		*cp = 0;

		/*
		 * The real work for the match.
		 */
		if (tnamatch (name)) {
			close (tf);
			return (tnchktc ());
		}
	}
}

/*
 * tnchktc: check the last entry, see if it's tc=xxx. If so,
 * recursively find xxx and append that entry (minus the names)
 * to take the place of the tc=xxx entry. This allows termcap
 * entries to say "like an HP2621 but doesn't turn on the labels".
 * Note that this works because of the left to right scan.
 */
static tnchktc ()
{
	register char *p, *q;
	char tcname [16];       /* name of similar terminal */
	char tcbuf [BUFSIZ];
	char *holdtbuf = tbuf;
	int l;

	p = tbuf + strlen(tbuf) - 2;	/* before the last colon */
	while (*--p != ':')
		if (p<tbuf) {
			outerr ("Bad termcap entry\n");
			return (0);
		}
	p++;
	/* p now points to beginning of last field */
	if (p[0] != 't' || p[1] != 'c')
		return (1);
	strcpy (tcname,p+3);
	q = tcname;
	while (*q && *q != ':')
		q++;
	*q = 0;
	if (++hopcount > MAXHOP) {
		outerr ("Infinite tc= loop\n");
		return (0);
	}
	if (! tgetent (tcbuf, tcname)) {
		hopcount = 0;		/* unwind recursion */
		return (0);
	}
	for (q=tcbuf; *q != ':'; q++);
	l = p - holdtbuf + strlen(q);
	if (l > BUFSIZ) {
		outerr ("Termcap entry too long\n");
		q[BUFSIZ - (p-tbuf)] = 0;
	}
	strcpy (p, q+1);
	tbuf = holdtbuf;
	hopcount = 0;			/* unwind recursion */
	return (1);
}

/*
 * Tnamatch deals with name matching.  The first field of the termcap
 * entry is a sequence of names separated by |'s, so we compare
 * against each such name.  The normal : terminator after the last
 * name (before the first field) stops us.
 */

static tnamatch (np)
char *np;
{
	register char *Np, *Bp;

	Bp = tbuf;
	if (*Bp == '#')
		return (0);
	for (;;) {
		for (Np = np; *Np && *Bp == *Np; Bp++, Np++)
			continue;
		if (*Np == 0 && (*Bp == '|' || *Bp == ':' || *Bp == 0))
			return (1);
		while (*Bp && *Bp != ':' && *Bp != '|')
			Bp++;
		if (*Bp == 0 || *Bp == ':')
			return (0);
		Bp++;
	}
}

/*
 * Skip to the next field.  Notice that this is very dumb, not
 * knowing about \: escapes or any such.  If necessary, :'s can be put
 * into the termcap file in octal.
 */

static char *tskip (bp)
register char *bp;
{

	while (*bp && *bp != ':')
		bp++;
	if (*bp == ':')
		bp++;
	return (bp);
}

CapGet (t, area)
struct CapTab *t;
char *area;
{
	register char *bp;
	register struct CapTab *p;
	register i, base;
	char name [2];

	if (! tbuf)
		return;
	bp = tbuf;
	for (;;) {
		bp = tskip(bp);
		if (! bp[0] || ! bp[1])
			return;
		if (bp[0] == ':' || bp[1] == ':')
			continue;
		name[0] = *bp++;
		name[1] = *bp++;
		for (p=t; p->tname[0]; ++p)
			if (p->tname[0] == name[0] && p->tname[1] == name[1])
				break;
		if (! p->tname[0] || p->tdef)
			continue;
		p->tdef = 1;
		if (*bp == '@')
			continue;
		switch (p->ttype) {
		case CAPNUM:
			if (*bp != '#')
				continue;
			bp++;
			base = 10;
			if (*bp == '0')
				base = 8;
			i = 0;
			while (*bp>='0' && *bp<='9')
				i = i * base, i += *bp++ - '0';
			*(p->ti) = i;
			break;
		case CAPFLG:
			if (*bp && *bp != ':')
				continue;
			*(p->tc) = 1;
			break;
		case CAPSTR:
			if (*bp != '=')
				continue;
			bp++;
			*(p->ts) = tdecode (bp, &area);
			break;
		}
	}
}

/*
 * Tdecode does the grung work to decode the
 * string capability escapes.
 */

static char *tdecode (str, area)
register char *str;
char **area;
{
	register char *cp;
	register int c;
	register char *dp;
	int i;

	cp = *area;
	while ((c = *str++) && c != ':') {
		switch (c) {

		case '^':
			c = *str++ & 037;
			break;

		case '\\':
			dp = "E\033^^\\\\::n\nr\rt\tb\bf\f";
			c = *str++;
nextc:
			if (*dp++ == c) {
				c = *dp++;
				break;
			}
			dp++;
			if (*dp)
				goto nextc;
			if (c>='0' && c<='9') {
				c -= '0', i = 2;
				do
					c <<= 3, c |= *str++ - '0';
				while (--i && *str>='0' && *str<='9');
			}
			break;
		}
		*cp++ = c;
	}
	*cp++ = 0;
	str = *area;
	*area = cp;
	return (str);
}

/*
 * Routine to perform cursor addressing.
 * CM is a string containing printf type escapes to allow
 * cursor addressing.  We start out ready to print the destination
 * line, and switch each time we print row or column.
 * The following escapes are defined for substituting row/column:
 *
 *	%d	as in printf
 *	%2	like %2d
 *	%3	like %3d
 *	%.	gives %c hacking special case characters
 *	%+x	like %c but adding x first
 *
 *	The codes below affect the state but don't use up a value.
 *
 *	%>xy	if value > x add y
 *	%r	reverses row/column
 *	%i	increments row/column (for one origin indexing)
 *	%%	gives %
 *	%B	BCD (2 decimal digits encoded in one byte)
 *	%D	Delta Data (backwards bcd)
 *
 * all other characters are ``self-inserting''.
 */

char *CapGoto (CM, destcol, destline)
char *CM;
{
	static char result [MAXRETURNSIZE];
	static char added [10];
	char *cp = CM;
	register char *dp = result;
	register c;
	int oncol = 0;
	register which = destline;

	if (! cp)
toohard:        return ("OOPS");

	added[0] = 0;
	while (c = *cp++) {
		if (c != '%') {
			*dp++ = c;
			continue;
		}
		switch (c = *cp++) {
		case 'n':
			destcol ^= 0140;
			destline ^= 0140;
			goto setwhich;
		case 'd':
			if (which < 10)
				goto one;
			if (which < 100)
				goto two;
			/* fall into... */
		case '3':
			*dp++ = (which / 100) | '0';
			which %= 100;
			/* fall into... */
		case '2':
two:                    *dp++ = which / 10 | '0';
one:                    *dp++ = which % 10 | '0';
swap:                   oncol = 1 - oncol;
setwhich:               which = oncol ? destcol : destline;
			continue;
		case '>':
			if (which > *cp++)
				which += *cp++;
			else
				cp++;
			continue;
		case '+':
			which += *cp++;
			/* fall into... */
		case '.':
			/*
			 * This code is worth scratching your head at for a
			 * while.  The idea is that various weird things can
			 * happen to nulls, EOT's, tabs, and newlines by the
			 * tty driver, arpanet, and so on, so we don't send
			 * them if we can help it.
			 *
			 * Tab is taken out to get Ann Arbors to work, otherwise
			 * when they go to column 9 we increment which is wrong
			 * because bcd isn't continuous.  We should take out
			 * the rest too, or run the thing through more than
			 * once until it doesn't make any of these, but that
			 * would make termlib (and hence pdp-11 ex) bigger,
			 * and also somewhat slower.  This requires all
			 * programs which use termlib to stty tabs so they
			 * don't get expanded.  They should do this anyway
			 * because some terminals use ^I for other things,
			 * like nondestructive space.
			 */
			if (!which || which == CTRL('d') || which == '\n') {
				if (oncol) /* Assumption: backspace works */
					/*
					 * Loop needed because newline happens
					 * to be the successor of tab.
					 */
					do {
						strcat (added, "\b");
						which++;
					} while (which == '\n');
			}
			*dp++ = which;
			goto swap;
		case 'r':
			oncol = 1;
			goto setwhich;
		case 'i':
			destcol++;
			destline++;
			which++;
			continue;
		case '%':
			*dp++ = c;
			continue;
		case '/':
			c = *cp;
			*dp++ = which / c | '0';
			which %= *cp++;
			continue;
		case 'B':
			which = (which/10 << 4) + which%10;
			continue;
		case 'D':
			which = which - 2 * (which%16);
			continue;
		default:
			goto toohard;
		}
	}
	strcpy (dp, added);
	return (result);
}

# endif /* MSDOS */
