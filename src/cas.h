#pragma once
#include <gint/display.h>
#include <gint/keyboard.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

/* ── screen geometry (fx-CG 50: 396×224 px, but gint uses 396×224) ── */
#define SCR_W       396
#define SCR_H       224
#define HEADER_H     18
#define FOOTER_H     16
#define BODY_Y      (HEADER_H + 2)
#define BODY_H      (SCR_H - HEADER_H - FOOTER_H - 4)

/* ── tuning ── */
#define SIMP_N      20    /* Simpson steps for 2-D integrals (must be even) */
#define SIMP_N3     10    /* steps for 3-D (speed vs. accuracy trade-off)   */
#define DIFF_H      1e-5  /* step for numerical differentiation             */
#define MAX_EXPR    40    /* max chars in one expression field               */
#define MAX_FIELDS  11    /* max fields in any single operation              */
#define MAX_RES      5    /* max result lines shown                         */

/* ── colours (gint RGB565 helpers) ── */
#define C_BG        C_WHITE
#define C_FG        C_BLACK
#define C_HILIGHT   C_RGB(0,12,31)   /* dark navy – selected row bg  */
#define C_HIFG      C_WHITE
#define C_ACCENT    C_RGB(0,12,31)
#define C_SOFT      C_RGB(14,14,14)  /* soft-key bar bg              */
#define C_SOFTFG    C_WHITE
#define C_ERR       C_RGB(28,0,0)

/* ── operations ── */
typedef enum {
    OP_CURLDIV   = 0,
    OP_SURFAREA  = 1,
    OP_SURFINT   = 2,
    OP_FLUX      = 3,
    OP_VOLUME    = 4,
    OP_TRIPLE    = 5,
    OP_TANGPLANE = 6,
    OP_COUNT     = 7
} Op;

/* ── coordinate systems (triple integral) ── */
typedef enum { COORD_CART=0, COORD_CYL=1, COORD_SPH=2 } Coords;

/* ── one input field ── */
typedef struct {
    char label[14];          /* e.g. "P(x,y,z)"  */
    char val[MAX_EXPR];      /* current value     */
    char def[MAX_EXPR];      /* default / example */
} Field;

/* ── one result line ── */
typedef struct {
    char label[20];
    char value[52];
} ResLine;

/* ── global app state ── */
typedef struct {
    /* navigation */
    int  screen;        /* 0=menu 1=input 2=result 3=orient 4=coords */
    int  menu_cur;
    int  field_cur;

    /* current operation */
    Op      op;
    int     orient;     /* +1 upward/outward  -1 downward/inward */
    Coords  coords;

    /* fields */
    Field   f[MAX_FIELDS];
    int     nf;

    /* results */
    ResLine res[MAX_RES];
    int     nres;
    char    err[64];
    double  raw;        /* primary numeric result (for π display) */
} State;

extern State G;

/* ── eval.c ── */
double eval(const char *e, double x, double y, double z);
double eval_lim(const char *s);           /* parse "pi/2", "2*pi", etc. */

/* ── compute.c ── */
void compute(void);

/* ── fields.c ── */
void fields_load(Op op, Coords coords);   /* populate G.f[] with labels+defaults */
void fields_defaults(void);               /* reset all vals to defaults           */

/* ── ui.c ── */
void ui_menu(void);
void ui_input(void);
void ui_result(void);
void ui_orient(void);
void ui_coords(void);
void ui_draw_softbar(const char *f1,const char *f2,const char *f3,
                     const char *f4,const char *f5,const char *f6);
void ui_header(const char *title);
void handle_input_key(int key);

/* ── helpers ── */
static inline double nd_x(const char *e,double x,double y,double z)
{ return (eval(e,x+DIFF_H,y,z)-eval(e,x-DIFF_H,y,z))/(2*DIFF_H); }
static inline double nd_y(const char *e,double x,double y,double z)
{ return (eval(e,x,y+DIFF_H,z)-eval(e,x,y-DIFF_H,z))/(2*DIFF_H); }
static inline double nd_z(const char *e,double x,double y,double z)
{ return (eval(e,x,y,z+DIFF_H)-eval(e,x,y,z-DIFF_H))/(2*DIFF_H); }
static inline double nd_u(const char *e,double u,double v)
{ return (eval(e,u+DIFF_H,v,0)-eval(e,u-DIFF_H,v,0))/(2*DIFF_H); }
static inline double nd_v(const char *e,double u,double v)
{ return (eval(e,u,v+DIFF_H,0)-eval(e,u,v-DIFF_H,0))/(2*DIFF_H); }

/* cross product magnitude */
static inline double cross_mag(double ax,double ay,double az,
                                double bx,double by,double bz){
    double cx=ay*bz-az*by, cy=az*bx-ax*bz, cz=ax*by-ay*bx;
    return sqrt(cx*cx+cy*cy+cz*cz);
}
/* cross product components */
static inline void cross3(double ax,double ay,double az,
                           double bx,double by,double bz,
                           double*cx,double*cy,double*cz){
    *cx=ay*bz-az*by; *cy=az*bx-ax*bz; *cz=ax*by-ay*bx;
}

/* format a double nicely (rounds near-integers) */
static inline void fmt(char *buf, int n, double v){
    double rnd = round(v*1e6)/1e6;
    if(rnd == (long long)rnd && fabs(rnd)<1e9)
        snprintf(buf,n,"%lld",(long long)rnd);
    else
        snprintf(buf,n,"%.6g",v);
}

/* add a result line */
static inline void add_res(const char *lbl, const char *val){
    if(G.nres>=MAX_RES) return;
    strncpy(G.res[G.nres].label, lbl, 19);
    strncpy(G.res[G.nres].value, val, 51);
    G.nres++;
}
