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

/*      Grphics drivers      */

# ifdef __MSDOS__
# include "tpmsdos.c"
# endif

# ifdef E85DEMOS
# include "tp85.c"
# endif

# ifdef M_XENIX
# include "tpcgi.c"
# endif
