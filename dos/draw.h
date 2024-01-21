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

# define BOUND   	01
# define SHADE   	02
# define PERSENT 	04
# define SLASH  	010
# define OTHERS		020
# define VERTICAL       040
# define AREA		0100
# define GREEDX		0200
# define GREEDY		0400
# define AUTOROT	01000

# ifdef DRAWFILE
int (*r_openpl)();
int (*r_closepl)();
int (*r_erase)();
int (*r_space)();
int (*r_color)();
int (*r_point)();
int (*r_circle)();
int (*r_sector)();
int (*r_cont)();
int (*r_move)();
int (*r_label)();
int (*r_charsize)();
int (*r_linemod)();
int (*r_rectan)();
int (*r_line)();
int (*r_grline)();
int (*r_grarc)();
# else
extern (*r_openpl)();
extern (*r_closepl)();
extern (*r_erase)();
extern (*r_space)();
extern (*r_color)();
extern (*r_point)();
extern (*r_circle)();
extern (*r_sector)();
extern (*r_cont)();
extern (*r_move)();
extern (*r_label)();
extern (*r_charsize)();
extern (*r_linemod)();
extern (*r_rectan)();
extern (*r_line)();
extern (*r_grarc)();
extern (*r_grline)();
# endif

# define openpl() (*r_openpl)()
# define closepl() (*r_closepl)()
# define erase() (*r_erase)()
# define space(x,y,x1,y1) (*r_space)(x,y,x1,y1)
# define color(c) (*r_color)(c)
# define point(x,y) (*r_point)(x,y)
# define circle(x,y,r) (*r_circle)(x,y,r)
# define grarc(x,y,r,f1,f2) (*r_grarc)(x,y,r,f1,f2)
# define sector(x,y,r,f1,f2,j) (*r_sector)(x,y,r,f1,f2,j)
# define cont(x,y) (*r_cont)(x,y)
# define move(x,y) (*r_move)(x,y)
# define label(s) (*r_label)(s)
# define charsize(x,y) (*r_charsize)(x,y)
# define linemod(s) (*r_linemod)(s)
# define rectan(x,y,x1,y1,c) (*r_rectan)(x,y,x1,y1,c)
# define line(x,y,x1,y1) (*r_line)(x,y,x1,y1)
# define grline(x,y,r,f) (*r_grline)(x,y,r,f)
