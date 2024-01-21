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

# define AREA   	0x0001
# define SFILE  	0x0002
# define ROW    	0x0003
# define COLUMN 	0x0004
# define JUMP   	0x0005
# define CELL   	0x0006
# define UTILITY	0x0007
# define GRAPH  	0x0008
# define SHAPE  	(0x0010 | GRAPH)
# define CFORMAT	(0x0010 | CELL)
# define AFORMAT	(0x0010 | AREA)
# define RFORMAT 	(0x0010 | ROW)
# define OFORMAT	(0x0010 | COLUMN)

# define LEVEL0 0x000f
# define LEVEL1 0x00f0
# define LEVEL2 0x0f00
# define LEVEL3 0xf000

extern State;
