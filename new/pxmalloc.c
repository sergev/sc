/*
 * A safer saner malloc, for careless programmers
 * $Revision: 6.16 $
 */

#include <stdio.h>
#include <curses.h>
#include "sc.h"

extern	char *malloc();
extern	char *realloc();
extern	void free();
void	fatal();

#ifdef SYSV3
extern void free();
extern void exit();
#endif

#define	MAGIC	(double)1234567890.1234456789

char *
xmalloc(n)
unsigned n;
{
	register char *ptr;

	if ((ptr = malloc(n + sizeof(double))) == NULL)
		fatal("xmalloc: no memory");
	*((double *) ptr) = MAGIC;		/* magic number */
	return(ptr + sizeof(double));
}

/* we make sure realloc will do a malloc if needed */
char *
xrealloc(ptr, n)
char	*ptr;
unsigned n;
{
	if (ptr == NULL)
		return(xmalloc(n));

	ptr -= sizeof(double);
	if (*((double *) ptr) != MAGIC)
		fatal("xrealloc: storage not xmalloc'ed");

	if ((ptr = realloc(ptr, n + sizeof(double))) == NULL)
		fatal("xmalloc: no memory");
	*((double *) ptr) = MAGIC;		/* magic number */
	return(ptr + sizeof(double));
}

void
xfree(p)
char *p;
{
	if (p == NULL)
		fatal("xfree: NULL");
	p -= sizeof(double);
	if (*((double *) p) != MAGIC)
		fatal("xfree: storage not malloc'ed");
	free(p);
}

#ifdef PSC
void
fatal(str)
char *str;
{
    (void) fprintf(stderr,"%s\n", str);
    exit(1);
}
#else
void
fatal(str)
char *str;
{
    deraw();
    (void) fprintf(stderr,"%s\n", str);
    diesave();
    exit(1);
}
#endif /* PSC */
