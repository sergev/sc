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

# define SIGNIF 50                      /* number of significant bits in double */
# define MAXPOWTWO ((double)(1L << 30) * (1L << SIGNIF - 31))
# define NMAX ((SIGNIF * 3 + 19)/10)    /* restrict max precision */
# define MAXLONG ((long) (((unsigned long) -1L) >> 1))
# define NDIG 80
# define MAXPREC 60
# define MAXFSIG 18

# define min(a,b)       ((a)<(b)?(a):(b))
# define APPEND(p,r,d)  { register char *q=r[d]; while (*q) *p++ = *q++; }
# define GETARG(p,t)    *((t *) p)
# define NEXTARG(p,t)   p = (int *) (((t *) p) + 1)
# define TRUNC(f)       ((int) (f))

# if defined (u3b) || defined (u3b5)
#   define POW1_25LEN 9
# else
#   define POW1_25LEN 6
# endif

static double pow1_25 [POW1_25LEN] = { 0.0 };
static char cvtbuf [NDIG];
static char buf [NDIG];

static char *r3 [3] = { "m", "mm", "mmm", };
static char *r2 [9] = { "c", "cc", "ccc", "cd", "d", "dc", "dcc", "dccc", "cm", };
static char *r1 [9] = { "x", "xx", "xxx", "xl", "l", "lx", "lxx", "lxxx", "xc", };
static char *r0 [9] = { "i", "ii", "iii", "iv", "v", "vi", "vii", "viii", "ix", };

extern double ldexp ();

# ifdef __386BSD__
# undef TRUNC
# define TRUNC trunc
static inline int trunc (f)
register double f;
{
	register int i;

	if (f < 0) {
		if ((i = -f) > -f)
			--i;
		return (-i);
	} else {
		if ((i = f) > f)
			--i;
		return (i);
	}
}
# endif

static char *cvt (val, ndigit, decpt, sign, flag)
double val;
int ndigit, *sign, flag;
register *decpt;
{
	register char *p = &cvtbuf[0], *last = &cvtbuf[ndigit];

	if (val < 0) {
		*sign = 1;
		val = -val;
	} else
		*sign = 0;

	if (ndigit<1 || ndigit>NDIG)
		ndigit = NDIG;

	*decpt = 0;
	*p = 0;
	if (val != 0.0) {
		/* rescale to range [1.0, 10.0) */
		/* in binary for speed and to minimize error build-up */
		/* even for the IEEE standard with its high exponents,
		/* it's probably better for speed to just loop on them */

		static struct s { double p10; int n; } s[] = {
			1e32,	32,
			1e16,	16,
			1e8,	8,
			1e4,	4,
			1e2,	2,
			1e1,	1,
		};
		register struct s *sp = s;

		++*decpt;
		if (val >= 2.0 * MAXPOWTWO)     /* can't be precisely integral */
			do {
				for ( ; val >= sp->p10; *decpt += sp->n)
					val /= sp->p10;
			} while (sp++->n > 1);
		else if (val >= 10.0) {         /* convert integer part separately */
			register double pow10 = 10.0, powtemp;

			while ((powtemp = 10.0 * pow10) <= val)
				pow10 = powtemp;
			for ( ; ; pow10 /= 10.0) {
				register int digit = TRUNC (val/pow10);
				*p++ = digit + '0';
				val -= digit * pow10;
				++*decpt;
				if (pow10 <= 10.0)
					break;
			}
		} else if (val < 1.0)
			do {
				for ( ; val * sp->p10 < 10.0; *decpt -= sp->n)
					val *= sp->p10;
			} while (sp++->n > 1);
	}
	if (flag)
		last += *decpt;
	if (last >= cvtbuf) {
		if (last > &cvtbuf[NDIG - 2])
			last = &cvtbuf[NDIG - 2];
		for ( ; ; ++p) {
			if (val == 0 || p >= &cvtbuf[NMAX])
				*p = '0';
			else {
				register int intx = TRUNC (val);
				*p = intx + '0';
				val = 10.0 * (val - (double) intx);
			}
			if (p >= last) {
				p = last;
				break;
			}
		}
		if (*p >= '5')          /* check rounding in last place + 1 */
			do {
				if (p == cvtbuf) {      /* rollover from 99999... */
					cvtbuf[0] = '1'; /* later digits are 0 */
					++*decpt;
					if (flag)
						++last;
					break;
				}
				*p = '0';
			} while (++*--p > '9');         /* propagate carries left */
		*last = '\0';
	}
	return (cvtbuf);
}

static char *cvtf (bp, prec, decpt, sign)
register char *bp;
{
	register char *p;
	register nn;
	int k;

	/* Initialize buffer pointer */
	p = &buf[0];

	/* determine the sign */
	if (sign && decpt > -prec && *bp != '0')
		*p++ = '-';

	nn = decpt;

	/* Emit the digits before the decimal point */
	k = 0;
	do
		*p++ = (nn<=0 || *bp=='\0' || k>=MAXFSIG) ? '0' : (k++, *bp++);
	while (--nn > 0);

	/* Decide whether we need a decimal point */
	if (prec > 0)
		*p++ = '.';

	/* Digits (if any) after the decimal point */
	nn = min (prec, MAXPREC);
	while (--nn >= 0)
		*p++ = (++decpt<=0 || *bp=='\0' || k>=MAXFSIG) ? '0' : (k++, *bp++);

	*p = 0;
	return (buf);
}

static char *cvte (bp, prec, decpt, sign)
register char *bp;
{
	register char *p;
	register nn, i;

	/* Initialize buffer pointer */
	p = &buf[0];

	/* determine the sign */
	if (sign && decpt > -prec && *bp != '0')
		*p++ = '-';

	/* Place the first digit in the buffer*/
	*p++ = (*bp != '\0') ? *bp++ : '0';

	/* Put in a decimal point if needed */
	if (prec != 0)
		*p++ = '.';

	/* Create the rest of the mantissa */
	for (nn=prec; nn>0 && *bp!='\0'; --nn)
		*p++ = *bp++;

	/* Put in the e */
	*p++ = 'e';

	/* Put in the exponent sign */
	*p++ = (decpt>0) ? '+' : '-';

	/* Create the exponent */
	nn = decpt - 1;
	if (nn < 0)
		nn = -nn;
	for (i=10000; i>1; i/=10)
		if (nn/i)
			break;
	for (; i>=1; i/=10)
		*p++ = '0' + (nn/i) % 10;

	*p = 0;
	return (buf);
}

/*
 *      char *dtoaf (double value, int precision)
 *
 *      Convert double to ascii string using %f format.
 *      Returns pointer to internal buffer.
 *
 *      double value -  double value to convert
 *
 *      int precision - number of digits after decimal point
 */

char *dtoaf (dval, prec)
double dval;
{
	register char *bp;
	int decpt, sign;

	/*
	 * F-format floating point.  This is a
	 * good deal less simple than E-format.
	 * The overall strategy will be to call
	 * fcvt, reformat its result into buf,
	 * and calculate how many trailing
	 * zeroes will be required.  There will
	 * never be any leading zeroes needed.
	 */

	bp = cvt (dval, min (prec, MAXPREC), &decpt, &sign, 1);
	return (cvtf (bp, prec, decpt, sign));
}

/*
 *      char *dtoae (double value, int precision)
 *
 *      Convert double to ascii string using %e format.
 *      Returns pointer to internal buffer.
 *
 *      double value -  double value to convert
 *
 *      int precision - number of digits in value
 */

char *dtoae (dval, prec)
double dval;
{
	register char *p;
	int decpt, sign;
	register nn;

	/*
	 * E-format.  The general strategy
	 * here is fairly easy: we take
	 * what ecvt gives us and re-format it.
	 */

	if (dval == 0) {
		p = buf;
		*p++ = '0';
		if (prec != 0)
			*p++ = '.';
		for (nn=prec; nn>0; --nn)
			*p++ = '0';
		*p++ = 'e';
		*p++ = '+';
		*p++ = '0';
		*p = 0;
		return (buf);
	}

	/* Develop the mantissa */
	p = cvt (dval, min (prec+1, MAXPREC), &decpt, &sign, 0);
	return (cvte (p, prec, decpt, sign));
}

/*
 *      char *dtoag (double value, int precision)
 *
 *      Convert double to ascii string using %g format.
 *      Returns pointer to internal buffer.
 *
 *      double value -  double value to convert
 *
 *      int precision - number of digits in value
 */

char *dtoag (dval, prec)
double dval;
{
	register char *bp;
	register n, nn;
	int decpt, sign;

	/*
	 * g-format.  We play around a bit
	 * and then jump into e or f, as needed.
	 */

	if (prec == 0)
		prec = 1;

	/* Do the conversion */
	bp = cvt (dval, min (prec, MAXPREC), &decpt, &sign, 0);
	if (dval == 0)
		decpt = 1;

	nn = prec;
	n = strlen (bp);
	if (n < nn)
		nn = n;
	while (nn >= 1 && bp[nn-1] == '0')
		--nn;

	if (decpt < -3 || decpt > prec)
		return (cvte (bp, nn-1, decpt, sign));
	else
		return (cvtf (bp, nn-decpt, decpt, sign));
}

/*
 *      double atod (char *p)
 *
 *      Converts ascii string to double.
 *      Format is [+|-][0-9]*[.][0-9]*[e[+|-| ][0-9]*]
 */

double atod (p)
register char *p;
{
	register c;
	int exp = 0, neg_val = 0;
	double fl_val;

	while ((c = *p) == ' ' || c=='\t')      /* eat leading white space */
		p++;
	switch (c) {                    /* process sign */
	case '-':
		neg_val++;
	case '+':                       /* fall-through */
		p++;
	}

	{                               /* accumulate value */
# ifdef pdp11
		/* "long" arithmetic on the PDP-11 is simulated using int's and
		/* is outrageously slow, so the accumulation is done using double's */

		register decpt = 0;

		fl_val = 0.0;
		while ((c = *p++) >= '0' && c<='9' || c == '.' && ! decpt++) {
			if (c == '.')
				continue;
			exp -= decpt;   /* decr exponent if dec point seen */
			if (fl_val < 2.0 * MAXPOWTWO)
				fl_val = 10.0 * fl_val + (double)(c - '0');
			else
				exp++;
		}
		if (! fl_val)
			return (0.0);
# else
		register long high = 0, low = 0, scale = 1;
		register decpt = 0, nzeroes = 0;

		while ((c = *p++) >= '0' && c<='9' || c == '.' && ! decpt++) {
			if (c == '.')
				continue;
			if (decpt) {    /* handle trailing zeroes specially */
				if (c == '0') {         /* ignore zero for now */
					nzeroes++;
					continue;
				}
				while (nzeroes > 0) {   /* put zeroes back in */
					exp--;
					if (high < MAXLONG/10) {
						high *= 10;
					} else if (scale < MAXLONG/10) {
						scale *= 10;
						low *= 10;
					} else
						exp++;
					nzeroes--;
				}
				exp--;  /* decr exponent if decimal pt. seen */
			}
			if (high < MAXLONG/10) {
				high *= 10;
				high += c - '0';
			} else if (scale < MAXLONG/10) {
				scale *= 10;
				low *= 10;
				low += c - '0';
			} else
				exp++;
		}
		if (! high)
			return (0.0);
		fl_val = (double) high;
		if (scale > 1)
			fl_val = (double) scale * fl_val + (double) low;
# endif
	}
	if (c == 'E' || c == 'e') {     /* accumulate exponent */
		register e_exp = 0, neg_exp = 0;

		switch (*p) {           /* process sign */
		case '-':
			neg_exp++;
		case '+':               /* fall-through */
		case ' ':               /* many FORTRAN environments generate this! */
			p++;
		}
		if ((c = *p) >= '0' && c<='9') {        /* found a legitimate exponent */
			do
				e_exp = 10 * e_exp + c - '0';
			while ((c = *++p) >= '0' && c<='9');
			if (neg_exp)
				exp -= e_exp;
			else
				exp += e_exp;
		}
	}
	/*
	 * The following computation is done in two stages,
	 * first accumulating powers of (10/8), then jamming powers of 8,
	 * to avoid underflow in situations like the following (for
	 * the DEC representation): 1.2345678901234567890e-37,
	 * where exp would be about (-37 + -18) = -55, and the
	 * value 10^(-55) can't be represented, but 1.25^(-55) can
	 * be represented, and then 8^(-55) jammed via ldexp().
	 */
	if (exp != 0) {         /* apply exponent */
		register double *powptr = pow1_25, fl_exp = fl_val;

		if (*powptr == 0.0) {   /* need to initialize table */
			*powptr = 1.25;
			for (; powptr < &pow1_25[POW1_25LEN - 1]; powptr++)
				powptr[1] = *powptr * *powptr;
			powptr = pow1_25;
		}
		if ((c = exp) < 0) {
			c = -c;
			fl_exp = 1.0;
		}
		for ( ; ; powptr++) {
			/* binary representation of ints assumed; otherwise
			/* replace (& 01) by (% 2) and (>>= 1) by (/= 2) */
			if (c & 01)
				fl_exp *= *powptr;
			if ((c >>= 1) == 0)
				break;
		}
		fl_val = ldexp(exp < 0 ? fl_val/fl_exp : fl_exp, 3 * exp);
	}
	return (neg_val ? -fl_val : fl_val);    /* apply sign */
}

/*
 *      char *ltoa (long value)
 *
 *      Convert long integer to ascii string using %d format.
 *      Returns pointer to internal buffer.
 *
 *      long value -    long integer value to convert
 */

char *ltoa (val)
long val;
{
	register char *bp = buf + NDIG-1;
	register long qval = val, n;

	*bp = 0;
	if (val == 0) {
		*--bp = '0';
		return (bp);
	}
	qval = qval<0 ? -qval : qval;
	while (qval > 0) {
		n = qval;
		qval /= 10;
		*--bp = n - qval * 10 + '0';
	}
	if (val < 0)
		*--bp = '-';
	return (bp);
}

/*
 *      char *itoa (int value)
 *
 *      Convert integer to ascii string using %d format.
 *      Returns pointer to internal buffer.
 *
 *      int value -     integer value to convert
 */

char *itoa (val)
{
	register char *bp = buf + NDIG-1;
	register long qval = val, n;

	*bp = 0;
	if (val == 0) {
		*--bp = '0';
		return (bp);
	}
	qval = qval<0 ? -qval : qval;
	while (qval > 0) {
		n = qval;
		qval /= 10;
		*--bp = n - qval * 10 + '0';
	}
	if (val < 0)
		*--bp = '-';
	return (bp);
}

/*
 *      char *itoaw (int value, int width)
 *
 *      Convert integer to ascii string using %*d format.
 *      Returns pointer to internal buffer.
 *
 *      int value -     double value to convert
 *
 *      int width -     width of string
 */

char *itoaw (val, width)
{
	register char *bp = itoa (val);
	register len = strlen (bp);

	if (width >= NDIG)
		width = NDIG-1;
	while (len++ < width)
		*--bp = ' ';
	return (bp);
}

/*
 *      char *itoar (int val)
 *
 *      Converts integer value to roman number (lower letters).
 *      Example: i, ii, iii, iv, v, etc.
 *      If value <= 0 or >= 4000, returns 0.
 *
 *      int val -       value to convert
 */

char *itoar (val)
register val;
{
	register char *p;
	register d;

	if (val <= 0 || val >= 4000)
		return (0);
	p = buf;
	if (d = val/1000) {     APPEND (p, r3, d-1);    val -= d*1000;  }
	if (d = val/100) {      APPEND (p, r2, d-1);    val -= d*100;   }
	if (d = val/10) {       APPEND (p, r1, d-1);    val -= d*10;    }
	if (val)                APPEND (p, r0, val-1);
	*p = 0;
	return (buf);
}

/*
 *      char *itoaur (int val)
 *
 *      Converts integer value to roman number (upper letters).
 *      Example: I, II, III, IV, V, etc.
 *      If value <= 0 or >= 4000, returns 0.
 *
 *      int val -       value to convert
 */

char *itoaur (val)
{
	register char *p;

	if (! itoar (val))
		return (0);
	for (p=buf; *p; *p++^=040);
	return (buf);
}

/*
 *      char *itoap (int val)
 *
 *      Converts integer value to +/- format.
 *      If value <= -80 or >= 80, returns 0.
 *
 *      int val -       value to convert
 */

char *itoap (val)
register val;
{
	register char *p;

	if (val == 0)
		return (".");
	if (val > 0) {
		if (val >= NDIG)
			return (0);
		for (p=buf; --val>=0; *p++='+');
	} else {
		if (val <= -NDIG)
			return (0);
		for (p=buf; ++val<=0; *p++='-');
	}
	*p = 0;
	return (buf);
}

char *doprint (string, fmt, ap)
char *string, *fmt;
int *ap;
{
	register c;
	long l;
	register char *p, *s = string;

	for (;;) {
		c = *fmt++;
		if (! c) {
			*s = 0;
			return (string);
		}
		if (c != '%') {
			*s++ = c;
			continue;
		}
		switch (c = *fmt++) {
		case 0:
			*s = 0;
			return (string);
		default:
			*s++ = c;
			break;
		case 'c':
			*s++ = GETARG (ap, int);
			NEXTARG (ap, int);
			break;
		case 's':
			p = GETARG (ap, char *);
			NEXTARG (ap, char *);
			while (*p)
				*s++ = *p++;
			break;
		case 'd':
			c = GETARG (ap, int);
			NEXTARG (ap, int);
			p = itoa (c);
			while (*p)
				*s++ = *p++;
			break;
		case 'l':
			if (*fmt == 'd')
				++fmt;
			l = GETARG (ap, long);
			NEXTARG (ap, long);
			p = ltoa (l);
			while (*p)
				*s++ = *p++;
			break;
		}
	}
}

/*
 *      sprint (char *string, char *format, arg, ...)
 *
 *      Print arguments according to format to string.
 *      Available formats are %c, %s, %d, %ld.
 */

/* VARARGS2 */

char *sprint (string, fmt, arg)
char *string, *fmt;
{
	return (doprint (string, fmt, &arg));
}
