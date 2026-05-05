/* ui.c ── all rendering and input handling */
#include "cas.h"
#include <gint/display.h>

/* ── font shorthand (gint's built-in small font) ── */
#define F3  &font_4x6    /* tiny — field values          */
#define F5  &font_5x9    /* body — labels, results       */

/* CG50 screen: 396 wide × 224 tall
   We lay out:
     row 0-17   : header bar
     row 18-205 : body
     row 206-223: soft-key bar (6 keys × ~66px wide)          */

static const char *OP_NAMES[] = {
    "CURL & DIV",
    "SURFACE AREA",
    "SURF INTEGRAL",
    "FLUX INTEGRAL",
    "VOLUME",
    "TRIPLE INTEGRAL",
    "TANGENT PLANE"
};

/* ── helper: draw a filled rectangle ── */
static void fillrect(int x,int y,int w,int h,color_t c){
    for(int r=y;r<y+h;r++)
        dline(x,r,x+w-1,r,c);
}

/* ── header bar ── */
void ui_header(const char *title){
    fillrect(0,0,396,18,C_ACCENT);
    dtext(4,4,C_WHITE,title);
    /* small right-side hint */
    dtext(300,4,C_SOFTFG,"EXE=OK EXIT=Back");
}

/* ── 6-key soft bar at bottom ── */
void ui_draw_softbar(const char *f1,const char *f2,const char *f3,
                     const char *f4,const char *f5,const char *f6){
    fillrect(0,207,396,17,C_SOFT);
    const char *lbs[]={f1,f2,f3,f4,f5,f6};
    for(int i=0;i<6;i++){
        int x=i*66+2;
        /* divider */
        if(i>0) dline(i*66,207,i*66,223,C_SOFTFG);
        dtext(x,210,C_SOFTFG,lbs[i]);
    }
}

/* ════════════════════════════════════════════════════════════
   MAIN MENU
   ════════════════════════════════════════════════════════════ */
void ui_menu(void){
    ui_header("MAT267 CAS  v1.0");
    /* subtitle */
    dtext(4,22,C_FG,"Use arrows or 1-7 to select:");

    for(int i=0;i<OP_COUNT;i++){
        int y=36+i*24;
        if(i==G.menu_cur){
            fillrect(0,y-2,396,20,C_HILIGHT);
            char buf[32];
            snprintf(buf,32," %d: %s",i+1,OP_NAMES[i]);
            dtext(4,y,C_HIFG,buf);
        } else {
            char buf[32];
            snprintf(buf,32," %d: %s",i+1,OP_NAMES[i]);
            dtext(4,y,C_FG,buf);
        }
    }
    ui_draw_softbar("SEL","","","","","QUIT");
}

/* ════════════════════════════════════════════════════════════
   ORIENTATION SCREEN  (Flux only)
   ════════════════════════════════════════════════════════════ */
void ui_orient(void){
    ui_header("FLUX: Orientation");
    dtext(4,28,C_FG,"Normal vector direction:");
    dtext(4,50,C_FG,"(affects sign of result)");

    int y1=80,y2=110;
    if(G.orient==1){
        fillrect(0,y1-2,396,20,C_HILIGHT);
        dtext(8,y1,C_HIFG,"> UPWARD / OUTWARD");
        dtext(8,y2,C_FG,  "  DOWNWARD / INWARD");
    } else {
        dtext(8,y1,C_FG,  "  UPWARD / OUTWARD");
        fillrect(0,y2-2,396,20,C_HILIGHT);
        dtext(8,y2,C_HIFG,"> DOWNWARD / INWARD");
    }
    ui_draw_softbar("UP","DOWN","","","NEXT","BACK");
}

/* ════════════════════════════════════════════════════════════
   COORDINATE SYSTEM SCREEN  (Triple only)
   ════════════════════════════════════════════════════════════ */
void ui_coords(void){
    ui_header("TRIPLE: Coordinates");
    dtext(4,28,C_FG,"Choose coordinate system:");

    const char *names[]={"CARTESIAN  (x,y,z)","CYLINDRICAL (r,th,z)","SPHERICAL (rho,phi,th)"};
    for(int i=0;i<3;i++){
        int y=60+i*30;
        if(G.coords==i){
            fillrect(0,y-2,396,22,C_HILIGHT);
            dtext(8,y,C_HIFG,names[i]);
        } else {
            dtext(8,y,C_FG,names[i]);
        }
    }
    dtext(4,165,C_FG,"f(x,y,z) always — coords");
    dtext(4,178,C_FG,"set the Jacobian & limits.");
    ui_draw_softbar("CART","CYL","SPH","","NEXT","BACK");
}

/* ════════════════════════════════════════════════════════════
   INPUT SCREEN
   ════════════════════════════════════════════════════════════ */

/* visible window: up to 7 fields fit; scroll if more */
#define VISIBLE_FIELDS 7

void ui_input(void){
    ui_header(OP_NAMES[G.op]);

    /* orientation reminder for flux */
    if(G.op==OP_FLUX){
        char ob[32];
        snprintf(ob,32,"Orient: %s",G.orient==1?"UP/OUT":"DOWN/IN");
        dtext(260,4,C_SOFTFG,ob);
    }

    int scroll = 0;
    if(G.field_cur >= VISIBLE_FIELDS)
        scroll = G.field_cur - VISIBLE_FIELDS + 1;

    for(int i=0;i<VISIBLE_FIELDS && (i+scroll)<G.nf;i++){
        int fi = i+scroll;
        int y  = 22 + i*26;
        int active = (fi == G.field_cur);

        /* label */
        dtext(4, y, active?C_ACCENT:C_FG, G.f[fi].label);

        /* value box */
        int bx=110, bw=280, bh=18;
        if(active){
            fillrect(bx,y-1,bw,bh,C_HILIGHT);
            /* value + blinking cursor */
            char buf[MAX_EXPR+2];
            snprintf(buf,sizeof(buf),"%s|",G.f[fi].val);
            dtext(bx+3,y,C_HIFG,buf);
        } else {
            drect(bx,y-1,bx+bw,y+bh-1,C_ACCENT);
            dtext(bx+3,y,C_FG,G.f[fi].val);
        }
    }

    /* scroll indicator */
    if(G.nf > VISIBLE_FIELDS){
        char si[12];
        snprintf(si,12,"%d/%d",G.field_cur+1,G.nf);
        dtext(360,4,C_SOFTFG,si);
    }

    ui_draw_softbar("DEL","CLR","PI","NEG","NEXT","CALC");
}

/* ── key handler for input screen ── */
void handle_input_key(int key){
    Field *f = &G.f[G.field_cur];
    int len  = strlen(f->val);

    switch(key){
    /* ── navigation ── */
    case KEY_UP:
        if(G.field_cur>0) G.field_cur--; break;
    case KEY_DOWN:
    case KEY_EXE:
    case KEY_F5: /* NEXT */
        if(G.field_cur < G.nf-1) G.field_cur++;
        else { compute(); G.screen=2; }
        break;

    /* ── soft keys ── */
    case KEY_F1: /* DEL – backspace */
        if(len>0) f->val[len-1]='\0'; break;
    case KEY_F2: /* CLR – clear field */
        f->val[0]='\0'; break;
    case KEY_F3: /* PI */
        if(len<MAX_EXPR-3){ strncat(f->val,"pi",MAX_EXPR-len-1); } break;
    case KEY_F4: /* NEG – toggle leading minus */
        if(f->val[0]=='-'){
            memmove(f->val,f->val+1,len);
        } else {
            if(len<MAX_EXPR-1){
                memmove(f->val+1,f->val,len+1);
                f->val[0]='-';
            }
        }
        break;
    case KEY_F6: /* CALC – run immediately */
        compute(); G.screen=2; break;

    case KEY_EXIT:
        G.screen=0; break;

    /* ── digits ── */
    case KEY_0: if(len<MAX_EXPR-1){f->val[len]='0';f->val[len+1]=0;} break;
    case KEY_1: if(len<MAX_EXPR-1){f->val[len]='1';f->val[len+1]=0;} break;
    case KEY_2: if(len<MAX_EXPR-1){f->val[len]='2';f->val[len+1]=0;} break;
    case KEY_3: if(len<MAX_EXPR-1){f->val[len]='3';f->val[len+1]=0;} break;
    case KEY_4: if(len<MAX_EXPR-1){f->val[len]='4';f->val[len+1]=0;} break;
    case KEY_5: if(len<MAX_EXPR-1){f->val[len]='5';f->val[len+1]=0;} break;
    case KEY_6: if(len<MAX_EXPR-1){f->val[len]='6';f->val[len+1]=0;} break;
    case KEY_7: if(len<MAX_EXPR-1){f->val[len]='7';f->val[len+1]=0;} break;
    case KEY_8: if(len<MAX_EXPR-1){f->val[len]='8';f->val[len+1]=0;} break;
    case KEY_9: if(len<MAX_EXPR-1){f->val[len]='9';f->val[len+1]=0;} break;

    /* ── operators ── */
    case KEY_PLUS:    if(len<MAX_EXPR-1){f->val[len]='+';f->val[len+1]=0;} break;
    case KEY_MINUS:   if(len<MAX_EXPR-1){f->val[len]='-';f->val[len+1]=0;} break;
    case KEY_TIMES:   if(len<MAX_EXPR-1){f->val[len]='*';f->val[len+1]=0;} break;
    case KEY_DIV:     if(len<MAX_EXPR-1){f->val[len]='/';f->val[len+1]=0;} break;
    case KEY_POWER:   if(len<MAX_EXPR-1){f->val[len]='^';f->val[len+1]=0;} break;
    case KEY_DOT:     if(len<MAX_EXPR-1){f->val[len]='.';f->val[len+1]=0;} break;
    case KEY_LPAR:    if(len<MAX_EXPR-1){f->val[len]='(';f->val[len+1]=0;} break;
    case KEY_RPAR:    if(len<MAX_EXPR-1){f->val[len]=')';f->val[len+1]=0;} break;

    /* ── trig / functions (append with open paren) ── */
    case KEY_SIN:
        if(len<MAX_EXPR-5) strncat(f->val,"sin(",MAX_EXPR-len-1); break;
    case KEY_COS:
        if(len<MAX_EXPR-5) strncat(f->val,"cos(",MAX_EXPR-len-1); break;
    case KEY_TAN:
        if(len<MAX_EXPR-5) strncat(f->val,"tan(",MAX_EXPR-len-1); break;
    case KEY_LN:
        if(len<MAX_EXPR-4) strncat(f->val,"ln(",MAX_EXPR-len-1);  break;
    case KEY_LOG:
        if(len<MAX_EXPR-5) strncat(f->val,"log(",MAX_EXPR-len-1); break;
    case KEY_SQRT:
        if(len<MAX_EXPR-6) strncat(f->val,"sqrt(",MAX_EXPR-len-1);break;
    case KEY_EXP: /* e^ key → exp( */
        if(len<MAX_EXPR-5) strncat(f->val,"exp(",MAX_EXPR-len-1); break;
    /* x² shortcut → ^2 */
    case KEY_SQUARE:
        if(len<MAX_EXPR-3) strncat(f->val,"^2",MAX_EXPR-len-1);   break;

    default: break;
    }
}

/* ════════════════════════════════════════════════════════════
   RESULT SCREEN
   ════════════════════════════════════════════════════════════ */
void ui_result(void){
    ui_header(OP_NAMES[G.op]);

    if(G.err[0]){
        dtext(4,28,C_ERR,"ERROR:");
        dtext(4,46,C_ERR,G.err);
    } else {
        int y=26;
        for(int i=0;i<G.nres;i++){
            /* label in accent, value in normal */
            dtext(4,y,C_ACCENT,G.res[i].label);
            dtext(10,y+14,C_FG,G.res[i].value);
            y+=34;
        }
        /* divider before pi note */
        dline(0,y,396,y,C_SOFT);
        y+=4;
        char piline[48];
        snprintf(piline,48,"raw = %.8g",G.raw);
        dtext(4,y,C_SOFT,piline);
    }

    ui_draw_softbar("","","","","REDO","MENU");
}
