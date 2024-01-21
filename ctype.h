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

# define _U     01      /* Upper case */
# define _L     02      /* Lower case */
# define _N     04      /* Numeral (digit) */
# define _S     010     /* Spacing character */
# define _P     020     /* Punctuation */
# define _C     040     /* Control character */
# define _B     0100    /* Blank */
# define _X     0200    /* heXadecimal digit */

# ifndef lint

extern char ctype [];

# define isalpha(c)     ((ctype + 1)[c] & (_U | _L))
# define isupper(c)     ((ctype + 1)[c] & _U)
# define islower(c)     ((ctype + 1)[c] & _L)
# define isdigit(c)     ((ctype + 1)[c] & _N)
# define isxdigit(c)    ((ctype + 1)[c] & _X)
# define isalnum(c)     ((ctype + 1)[c] & (_U | _L | _N))
# define isspace(c)     ((ctype + 1)[c] & _S)
# define ispunct(c)     ((ctype + 1)[c] & _P)
# define isprint(c)     ((ctype + 1)[c] & (_P | _U | _L | _N | _B))
# define isgraph(c)     ((ctype + 1)[c] & (_P | _U | _L | _N))
# define iscntrl(c)     ((ctype + 1)[c] & _C)
# define isascii(c)     (!((c) & ~0177))
# define toascii(c)     ((c) & 0177)

# ifdef __MSDOS__
# define tolower(c)     ((c) < 0240 ? (c) + 040 : (c) + 0100)
# else
# define toupper(c)     ((c) & 0200 ? (c) | 040 : (c) & ~040)
# define tolower(c)     ((c) & 0200 ? (c) & ~040 : (c) | 040)
# endif

# endif /* lint */
