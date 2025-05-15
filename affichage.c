#include "affichage.h"
#include <stdio.h>

void afficher_barre_de_vie(BITMAP *buf, const Vaisseau *v){
    int max = VIES_INITIALES, w = 100, h = 10, x = 10, y = 50;
    int cur = v->nb_vie * w / max;
    int couleur;

    if (v->nb_vie > 2)       couleur = makecol(0,255,0);
    else if (v->nb_vie == 2) couleur = makecol(255,255,0);
    else                     couleur = makecol(255,0,0);

    rect(buf,     x-1,y-1, x+w+1,y+h+1, makecol(255,255,255));
    rectfill(buf, x,  y,   x+w,  y+h,   makecol(100,0,0));
    rectfill(buf, x,  y,   x+cur,y+h,   couleur);
}

void afficher_menu_principal(BITMAP *page, const char *pseudo, int sel,
                             BITMAP *fondMenu, int meilleur_score, int meilleur_niveau)
{
    clear_bitmap(page);
    draw_sprite(page, fondMenu, 0, 0);

    if(meilleur_score >= 0){
        char s[80];
        sprintf(s,
          "Meilleur Score : %d    Meilleur Niveau : %d",
          meilleur_score, meilleur_niveau);
        textout_ex(page, font, s, 10,455, makecol(255,255,0), -1);
    }

    textout_centre_ex(page,font,"-----ECE-RTYPE-----",
                      SCREEN_W/2,10, makecol(255,255,255), -1);
    {
        char buf[64];
        sprintf(buf, "Bienvenue, %s !", pseudo);
        textout_centre_ex(page,font,buf,
                          SCREEN_W/2,80, makecol(255,255,255), -1);
    }

    const char *opts[3] = {
        "1. Commencer",
        "2. Reprendre",
        "3. Quitter"
    };
    for(int i=0;i<3;i++){
        int col = (i+1==sel)
                ? makecol(0,255,0)
                : makecol(255,255,255);
        textout_centre_ex(page,font,opts[i],
            SCREEN_W/2, SCREEN_H/2 + (i*30) - 30,
            col, -1);
    }
    blit(page, screen, 0,0, 0,0, SCREEN_W, SCREEN_H);
}
