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

# define HYSTO		0
# define CHYSTO		1
# define GRAPHIC	2
# define PIE		3
# define XYGRAPH	4

struct axis {
	short minr, minc, maxr, maxc;
	short valid;
};

struct graph {
	struct axis axisA, axisB, axisC, axisD;
	short shape;
};

extern struct graph graph;
