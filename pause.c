#include <allegro.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <stdio.h>

// Created by camil on 15/05/2025.
//
void gerer_pause(BITMAP *page) {
    textout_centre_ex(page, font,
                      "Le jeu est en pause. ",
                      SCREEN_W/2, SCREEN_H/2 - 20,
                      makecol(255, 255, 0), -1);
    textout_centre_ex(page, font,
                      "1 - Reprendre la partie : P",
                      SCREEN_W/2, SCREEN_H/2,
                      makecol(255, 255, 0), -1);
    textout_centre_ex(page, font,
                      "2 - Quitter : P puis ESC",
                      SCREEN_W/2, SCREEN_H/2 + 20,
                      makecol(255, 255, 0), -1);
    blit(page, screen, 0,0, 0,0, SCREEN_W, SCREEN_H);


    // Attente du relâchement de la touche P
    while (key[KEY_P]) rest(10);

    // Attente de l'appui suivant
    while (!key[KEY_P]) rest(10);

    // Attente du relâchement de la touche P à nouveau
    while (key[KEY_P]) rest(10);
}
