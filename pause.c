#include <allegro.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

// Created by camil on 15/05/2025.
//
void gerer_pause(BITMAP *page){
    textout_centre_ex(page,font,"Le jeu est en pause. Appuyez sur 'P' pour reprendre",SCREEN_W/2, SCREEN_H/2,makecol(255, 255, 0), -1);
    blit(page, screen, 0,0,0,0,SCREEN_W,SCREEN_H);
    while(key[KEY_P]) rest(10); //contrôle touche
    while(!key[KEY_P]) rest(10);
    while(key[KEY_P]) rest(10);
}