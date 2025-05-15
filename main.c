#include <allegro.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "utilitaires.h"
#include "deplacements.h"
#include "affichage.h"
#include "sauvegarde.h"
#include "niveau.h"
#include "pause.h"

int main(){
    srand(time(NULL));
    allegro_init();
    install_keyboard(); install_mouse();
    set_color_depth(32);
    if(set_gfx_mode(GFX_AUTODETECT_WINDOWED,640,480,0,0)){
        allegro_message("Erreur mode graphique");
        exit(EXIT_FAILURE);
    }

    // Chargement images
    BITMAP *page         = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP *fond_menu    = charger_bitmap_sure("menu5.bmp");
    BITMAP *fond         = charger_bitmap_sure("Fond1600x600.bmp");
    BITMAP *vaisseau_img = charger_bitmap_sure("Vaisseau.bmp");
    BITMAP *ennemi_img   = charger_bitmap_sure("Fantome_rose.bmp");
    BITMAP *coeur_img    = charger_bitmap_sure("Pacman4.bmp");


    // 1) saisie pseudo
    char pseudo[50] = "";
    while(1){
        clear_bitmap(page);
        draw_sprite(page, fond_menu, 0, 0);
        textout_centre_ex(page,font,
            "Entrez votre pseudo:", SCREEN_W/2,100,
            makecol(255,255,255), -1);
        textout_ex(page,font,pseudo,
            SCREEN_W/2-50,130, makecol(0,255,0), -1);
        blit(page, screen, 0,0,0,0,SCREEN_W,SCREEN_H);
        if(keypressed()){
            int k = readkey(); char c=k&0xFF;
            if(c==8 && strlen(pseudo)>0)      pseudo[strlen(pseudo)-1]=0;
            else if(c==13 && strlen(pseudo)>0) break;
            else if(isprint(c) && strlen(pseudo)<49)
                strncat(pseudo, &c, 1);
        }
    }

    clear_keybuf();
    rest(500);

    // 2) chargement sauvegarde
    int best_score=-1, best_level=-1;
    lire_sauvegarde(pseudo, &best_score, &best_level);

    // 3) boucle menu→jeu→menu
    while(1){
        int selection=1;
        while(1){
            afficher_menu_principal(
                page, pseudo, selection,
                fond_menu, best_score, best_level
            );

            if(key[KEY_UP])    { if(selection>1) selection--; rest(150); }
            if(key[KEY_DOWN])  { if(selection<3) selection++; rest(150); }
            if(key[KEY_ENTER]){
                if(selection==3){
                    ecrire_sauvegarde(pseudo,best_score,best_level);
                    allegro_exit();
                    return 0;
                }
                break;
            }
            if(mouse_b & 1){
                int mx=mouse_x, my=mouse_y;
                if(mx>SCREEN_W/2-100 && mx<SCREEN_W/2+100 &&
                   my>SCREEN_H/2-30  && my<SCREEN_H/2+60)
                {
                    selection = ((my-(SCREEN_H/2-30))/30) + 1;
                    if(selection==3){
                        ecrire_sauvegarde(pseudo,best_score,best_level);
                        allegro_exit();
                        return 0;
                    }
                    break;
                }
            }
            rest(30);
        }

        if(selection == 1) {
            // Utilisation de la fonction demander_niveau pour choisir le niveau
            int niveau_choisi = demander_niveau(page, fond_menu);

            // Récupération de la vitesse ennemis selon le niveau choisi
            int vitesse_ennemi = vitesse_ennemi_selon_niveau(niveau_choisi);

            // Récupération de la vitesse coeurs selon le niveau choisi
            int vitesse_coeur = vitesse_coeur_selon_niveau(niveau_choisi);

            // – initialisation jeu –
            Vaisseau v = {50,240,5,32,32,VIES_INITIALES};
            Projectile projectiles[MAX_PROJECTILES] = {{0}};
            Ennemi    ennemis[MAX_ENNEMIS]         = {{0}};
            Coeur    coeurs[MAX_COEURS]         = {{0}};
            int vies=VIES_INITIALES, score=0, level=niveau_choisi;
            int screenx=0;
            clock_t last_tir=0; const int tir_delay=500;
            time_t debut = time(NULL);

            // – boucle de jeu –
            while(!key[KEY_ESC] && v.nb_vie>0
                  && difftime(time(NULL),debut)<DUREE_NIVEAU)
            {
                screenx+=2;
                if(screenx>fond->w-SCREEN_W) screenx=0;
                clear_bitmap(page);
                blit(fond,page,screenx,0,0,0,SCREEN_W,SCREEN_H);

                int dx=0, dy=0;
                if(key[KEY_UP])    dy=-v.vitesse;
                if(key[KEY_DOWN])  dy= v.vitesse;
                if(key[KEY_LEFT])  dx=-v.vitesse;
                if(key[KEY_RIGHT]) dx= v.vitesse;
                if(key[KEY_P]){
                    gerer_pause(page);
                }

                deplacer_vaisseau(&v,dx,dy);

                if(key[KEY_SPACE] && clock()-last_tir>=tir_delay){
                    for(int i=0;i<MAX_PROJECTILES;i++){
                        if(!projectiles[i].actif){
                            projectiles[i].x     = v.x + v.largeur + 25;
                            projectiles[i].y     = v.y + v.hauteur+1/2;
                            projectiles[i].actif = 1;
                            last_tir = clock();
                            break;
                        }
                    }
                }

                for(int i=0;i<MAX_PROJECTILES;i++)
                    deplacer_projectile(&projectiles[i]);

                if(rand()%30==0){
                    for(int i=0;i<MAX_ENNEMIS;i++){
                        if(!ennemis[i].actif){
                            ennemis[i].actif=1;
                            ennemis[i].x   = SCREEN_W;
                            ennemis[i].y   = rand()%(SCREEN_H-HAUTEUR_ENNEMI);
                            break;
                        }
                    }
                }

                if(rand()%100==0){
                    for(int i=0;i<MAX_COEURS;i++){
                        if(!coeurs[i].actif){
                            coeurs[i].actif=1;
                            coeurs[i].x   = SCREEN_W;
                            coeurs[i].y   = rand()%(SCREEN_H-HAUTEUR_COEUR);
                            break;
                        }
                    }
                }

                for(int i=0;i<MAX_ENNEMIS;i++){
                    if(ennemis[i].actif){
                        ennemis[i].x -= vitesse_ennemi;
                        if(ennemis[i].x < -HAUTEUR_ENNEMI)
                            ennemis[i].actif = 0;
                    }
                    if(collision_vaisseau_ennemi(&v,&ennemis[i])){
                        v.nb_vie--;
                        ennemis[i].actif=0;
                    }
                    for(int j=0;j<MAX_PROJECTILES;j++){
                        detecter_collision_projectile(
                            &projectiles[j],
                            &ennemis[i],
                            &score
                        );
                    }
                }

                for(int i=0;i<MAX_COEURS;i++){
                    if(coeurs[i].actif){
                        coeurs[i].x -= vitesse_coeur;
                        if(coeurs[i].x < -HAUTEUR_COEUR)
                            coeurs[i].actif = 0;
                    }
                    if(collision_vaisseau_coeur(&v,&coeurs[i])){
                        if (v.nb_vie==3) {
                            coeurs[i].actif=0;
                        }
                        else {
                            v.nb_vie++;
                        }
                    }
                }

                draw_sprite(page, vaisseau_img, v.x, v.y);
                for(int i=0;i<MAX_PROJECTILES;i++){
                    if(projectiles[i].actif)
                        rectfill(page,
                            projectiles[i].x, projectiles[i].y,
                            projectiles[i].x+5,projectiles[i].y+5,
                            makecol(255,0,0)
                        );
                }
                for(int i=0;i<MAX_ENNEMIS;i++){
                    if(ennemis[i].actif)
                        draw_sprite(page,ennemi_img,
                                    ennemis[i].x, ennemis[i].y);
                }

                for(int i=0;i<MAX_COEURS;i++){
                    if(coeurs[i].actif)
                        stretch_sprite(
                                    page,
                                    coeur_img,
                                    coeurs[i].x,
                                    coeurs[i].y,
                                    coeur_img->w / 2,   // Largeur divisée par 2
                                    coeur_img->h / 2    // Hauteur divisée par 2
                                    );
                }

                textprintf_ex(page,font,10,10,makecol(255,255,255),-1,
                              "Score: %d", score);
                textprintf_ex(page,font,10,30,makecol(255,255,255),-1,
                              "Vies : %d", v.nb_vie);
                int rem = DUREE_NIVEAU - (int)difftime(time(NULL), debut);
                if(rem<0) rem=0;
                textprintf_ex(page,font,SCREEN_W-100,10,
                              makecol(255,255,0),-1,
                              "Temps: %ds", rem);
                afficher_barre_de_vie(page, &v);
                textprintf_ex(page,font,10,80,makecol(255,255,255),-1,
                              "Pause : P");

                blit(page, screen, 0,0, 0,0, SCREEN_W, SCREEN_H);
                rest(30);
            }

            if(score>best_score)    best_score=score;
            if(level>best_level)    best_level=level;
            ecrire_sauvegarde(pseudo,best_score,best_level);

            rest(300);
        }
        else if(selection == 2) {
            // Continuer partie (à implémenter selon ta logique)
            // Par exemple lancer la partie au niveau best_level
        }
    }

    return 0;
}
END_OF_MAIN();
