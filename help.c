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

# ifdef STDHELPFILE			/* default */
# define HELPFILE "/usr/lib/sc.help"
# endif

# if defined (MAKEHELPFILE) || !defined (NOHELP) && !defined (HELPFILE)
# ifdef RUS
extern char *rhelp1[], *rhelp2[];
static char ** rhelp [] = { rhelp1, rhelp2, 0, };
# endif /* RUS */
extern char *lhelp1[], *lhelp2[];
static char ** lhelp [] = { lhelp1, lhelp2, 0, };
# endif /* MAKEHELPFILE || !NOHELP && !HELPFILE */

# ifdef MAKEHELPFILE
# include <stdio.h>
main ()
{
	register char **p, ***q;

	for (q=lhelp; *q; ++q) {
		for (p= *q; *p; ++p)
			printf (*p);
		if (q [1])
			printf ("@\n");
	}
	printf ("@@\n");
# ifdef RUS
	for (q=rhelp; *q; ++q) {
		for (p= *q; *p; ++p)
			printf (*p);
		if (q [1])
			printf ("@\n");
	}
	printf ("@@\n");
# endif
	exit (0);
}
# else
# include "sc.h"
# include "scr.h"

# ifdef NOHELP
help () { error (MSG (61)); }   /* "No help available" */
# else

# ifdef HELPFILE
# include <stdio.h>
help ()
{
	char s [128];
	register FILE *f;
	register char *pak = MSG (63);

	if (! (f = fopen (HELPFILE, "r")))
		/* "Cannot open %s" */
		cerror (MSG (62), HELPFILE);
# ifdef RUS
	if (rusout)
		while (!feof (f) && fgets (s, sizeof(s)-1, f) && strcmp (s, "@@\n"));
# endif
	do {
		VClear ();
		while (!feof (f) && fgets (s, sizeof(s)-1, f) && *s != '@')
			VPutString (s);
		/* "\1\16Press any key...\2\17" */
		VMPutString (LINES-1, (COLS - 1 - strlen (pak)) / 2, pak);
		VSync ();
		KeyGet ();
	} while (!feof (f) && s[1] != '@');
	VClear ();
	fclose (f);
	FullUpdate = 1;
}
# else
help ()
{
	register char **p, ***q;
	register char *pak = MSG (63);

# ifdef RUS
	for (q=rusout?rhelp:lhelp; *q; ++q) {
# else
	for (q=lhelp; *q; ++q) {
# endif
		VClear ();
		for (p= *q; *p; ++p)
			VPutString (*p);
		/* "\1\16Press any key...\2\17" */
		VMPutString (LINES-1, (COLS - 1 - strlen (pak)) / 2, pak);
		VSync ();
		KeyGet ();
	}
	VClear ();
	FullUpdate = 1;
}
# endif /* HELPFILE */
# endif /* NOHELP */
# endif /* MAKEHELPFILE */
