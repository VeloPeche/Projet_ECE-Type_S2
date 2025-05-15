#include "niveau.h"
#include <stdio.h>
#include <string.h>

int demander_niveau(BITMAP *page, BITMAP *fond_menu) {
    int niveau = 1;
    clear_keybuf();  // Vide le buffer clavier au début

    while (1) {
        clear_bitmap(page);
        draw_sprite(page, fond_menu, 0, 0);

        textout_centre_ex(page, font, "Choisissez le niveau :", SCREEN_W / 2, 100, makecol(255, 255, 255), -1);
        for (int i = 1; i <= 3; i++) {
            int couleur = (i == niveau) ? makecol(255, 255, 0) : makecol(255, 255, 255);
            char texte[20];
            sprintf(texte, "Niveau %d", i);
            textout_centre_ex(page, font, texte, SCREEN_W / 2, 150 + i * 30, couleur, -1);
        }

        blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        int touche = readkey();  // Attend une touche

        if ((touche >> 8) == KEY_UP) {
            if (niveau > 1) niveau--;
        }
        else if ((touche >> 8) == KEY_DOWN) {
            if (niveau < 3) niveau++;
        }
        else if ((touche & 0xFF) == 13) { // Entrée
            break;
        }

        rest(100); // petite pause pour éviter rebonds trop rapides
    }

    clear_keybuf();
    return niveau;
}


int vitesse_ennemi_selon_niveau(int niveau) {
    switch(niveau){
        case 1: return VITESSE_ENNEMI_N1;
        case 2: return VITESSE_ENNEMI_N2;
        case 3: return VITESSE_ENNEMI_N3;
        default: return VITESSE_ENNEMI_N1;
    }
}
int vitesse_coeur_selon_niveau(int niveau) {
    switch(niveau){
        case 1: return VITESSE_COEUR_N1;
        case 2: return VITESSE_COEUR_N2;
        case 3: return VITESSE_COEUR_N3;
        default: return VITESSE_COEUR_N1;
    }
}

