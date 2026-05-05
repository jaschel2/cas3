#include <gint/gint.h>
#include "cas.h"

State G;

int main(void)
{
    /* ── initial state ── */
    memset(&G, 0, sizeof G);
    G.screen   = 0;
    G.menu_cur = 0;
    G.orient   = 1;
    G.coords   = COORD_SPH;

    while(1){
        dclear(C_BG);

        switch(G.screen){
        case 0: ui_menu();   break;
        case 1: ui_input();  break;
        case 2: ui_result(); break;
        case 3: ui_orient(); break;
        case 4: ui_coords(); break;
        }

        dupdate();

        /* ── key dispatch ── */
        key_event_t ev = getkey();
        int k = ev.key;

        if(G.screen == 0){
            /* ── MAIN MENU ── */
            if(k==KEY_UP   && G.menu_cur>0)          G.menu_cur--;
            if(k==KEY_DOWN && G.menu_cur<OP_COUNT-1) G.menu_cur++;

            /* number shortcuts 1-7 */
            if(k>=KEY_1 && k<=KEY_7) G.menu_cur = k-KEY_1;

            if(k==KEY_EXE || k==KEY_F1){
                G.op = (Op)G.menu_cur;
                G.nres = 0; G.err[0]='\0';
                if(G.op==OP_FLUX)      { G.screen=3; }
                else if(G.op==OP_TRIPLE){ G.screen=4; }
                else {
                    fields_load(G.op, G.coords);
                    fields_defaults();
                    G.field_cur=0;
                    G.screen=1;
                }
            }
            if(k==KEY_EXIT) gint_osmenu();  /* return to calc OS menu */
        }
        else if(G.screen == 1){
            handle_input_key(k);
        }
        else if(G.screen == 2){
            /* result screen */
            if(k==KEY_EXIT || k==KEY_F5) G.screen=1;
            if(k==KEY_F6  || k==KEY_MENU){ G.screen=0; G.menu_cur=(int)G.op; }
        }
        else if(G.screen == 3){
            /* orient screen */
            if(k==KEY_UP  ||k==KEY_F1) G.orient= 1;
            if(k==KEY_DOWN||k==KEY_F2) G.orient=-1;
            if(k==KEY_EXE ||k==KEY_F5){
                fields_load(G.op, G.coords);
                fields_defaults();
                G.field_cur=0;
                G.screen=1;
            }
            if(k==KEY_EXIT||k==KEY_F6) G.screen=0;
        }
        else if(G.screen == 4){
            /* coords screen */
            if(k==KEY_UP   && G.coords>0) G.coords--;
            if(k==KEY_DOWN && G.coords<2) G.coords++;
            if(k==KEY_F1) G.coords=COORD_CART;
            if(k==KEY_F2) G.coords=COORD_CYL;
            if(k==KEY_F3) G.coords=COORD_SPH;
            if(k==KEY_EXE ||k==KEY_F5){
                fields_load(G.op, G.coords);
                fields_defaults();
                G.field_cur=0;
                G.screen=1;
            }
            if(k==KEY_EXIT||k==KEY_F6) G.screen=0;
        }
    }
    return 0;
}
