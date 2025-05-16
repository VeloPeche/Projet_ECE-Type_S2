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
#include "donnees.h"

int main() {
    srand(time(NULL));
    allegro_init();
    install_keyboard();
    install_mouse();
    set_color_depth(32);

    if(set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0)) {
        allegro_message("Erreur mode graphique");
        exit(EXIT_FAILURE);
    }

    // Chargement des bitmaps
    BITMAP *page             = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP *fond_menu        = charger_bitmap_sure("menu5.bmp");
    BITMAP *fond             = charger_bitmap_sure("Fond1600x600.bmp");
    BITMAP *vaisseau_img     = charger_bitmap_sure("Vaisseau.bmp");
    BITMAP *ennemi_img       = charger_bitmap_sure("Fantome_rose.bmp");
    BITMAP *coeur_img        = charger_bitmap_sure("coeur_magenta.bmp");
    BITMAP *sprite_obstacle  = charger_bitmap_sure("etoile_ennemie.bmp");

    // --- Saisie du pseudo ---
    char pseudo[50] = "";
    while (1) {
        clear_bitmap(page);
        draw_sprite(page, fond_menu, 0, 0);
        textout_centre_ex(page, font,
            "Entrez votre pseudo:", SCREEN_W/2, 100,
            makecol(255,255,255), -1);
        textout_ex(page, font, pseudo,
            SCREEN_W/2 - 50, 130, makecol(0,255,0), -1);
        blit(page, screen, 0,0, 0,0, SCREEN_W, SCREEN_H);
        if (keypressed()) {
            int k = readkey(), c = k & 0xFF;
            if (c == 8 && strlen(pseudo) > 0)
                pseudo[strlen(pseudo)-1] = 0;
            else if (c == 13 && strlen(pseudo) > 0)
                break;
            else if (isprint(c) && strlen(pseudo) < sizeof(pseudo)-1)
                strncat(pseudo, (char[]){c,0}, 1);
        }
        rest(30);
    }
    clear_keybuf();
    rest(500);

    // --- Chargement de la sauvegarde ---
    int best_score = -1, best_level = -1;
    lire_sauvegarde(pseudo, &best_score, &best_level);

    // --- Boucle principale menu → jeu → menu ---
    while (1) {
        int selection = 1;
        // Affichage du menu
        while (1) {
            afficher_menu_principal(
                page, pseudo, selection,
                fond_menu, best_score, best_level
            );
            if (key[KEY_UP])    { if (selection>1) selection--; rest(150); }
            if (key[KEY_DOWN])  { if (selection<3) selection++; rest(150); }
            if (key[KEY_ENTER]) {
                if (selection==3) {
                    ecrire_sauvegarde(pseudo, best_score, best_level);
                    allegro_exit();
                    return 0;
                }
                break;
            }
            if (mouse_b & 1) {
                int mx = mouse_x, my = mouse_y;
                if (mx > SCREEN_W/2-100 && mx < SCREEN_W/2+100 &&
                    my > SCREEN_H/2-30  && my < SCREEN_H/2+60)
                {
                    selection = ((my - (SCREEN_H/2 - 30)) / 30) + 1;
                    if (selection==3) {
                        ecrire_sauvegarde(pseudo, best_score, best_level);
                        allegro_exit();
                        return 0;
                    }
                    break;
                }
            }
            rest(30);
        }

        // --- Configuration du niveau choisi ---
        int niveau_choisi = demander_niveau(page, fond_menu);

        // === Initialisation dynamique selon niveau ===
        Etoile_ennemie obstacles[30] = {{0}};  // capacité max
        int nombre_obstacles   = 0;
        int nb_etoiles_actives = 0;
        int taux_spawn_ennemis;               // plus petit ⇒ spawn plus fréquent

        switch (niveau_choisi) {
            case 1:
                nombre_obstacles   = 0;   // pas d’obstacles au niveau 1
                taux_spawn_ennemis = 30;  // spawn rare d’ennemis
                break;
            case 2:
                nombre_obstacles   = 20;  // obstacles présents
                taux_spawn_ennemis = 25;  // plus d’ennemis
                break;
            case 3:
                nombre_obstacles   = 30;  // encore plus d’obstacles
                taux_spawn_ennemis = 20;  // spawn très fréquent
                break;
            default:
                nombre_obstacles   = 0;
                taux_spawn_ennemis = 30;
                break;
        }

        // Lancer une partie
        if (selection == 1) {
            int vitesse_ennemi = vitesse_ennemi_selon_niveau(niveau_choisi);
            int vitesse_coeur  = vitesse_coeur_selon_niveau(niveau_choisi);

            // Initialisation des entités
            Vaisseau  v          = {50,240,5,32,32,VIES_INITIALES};
            Projectile projectiles[MAX_PROJECTILES] = {{0}};
            Ennemi    ennemis[MAX_ENNEMIS]          = {{0}};
            Missile   missiles[MAX_MISSILES]        = {{0}};
            Coeur     coeurs[MAX_COEURS]            = {{0}};

            int score = 0, level = niveau_choisi;
            int screenx = 0;
            clock_t last_tir = 0;
            const int tir_delay = 500;
            time_t debut = time(NULL);

            // Boucle de jeu
            while (!key[KEY_ESC] && v.nb_vie > 0
                   && difftime(time(NULL), debut) < DUREE_NIVEAU)
            {
                // 1) Scrolling
                screenx += 2;
                if (screenx > fond->w - SCREEN_W) screenx = 0;
                clear_bitmap(page);
                blit(fond, page, screenx,0, 0,0, SCREEN_W, SCREEN_H);

                // 2) Déplacement & pause
                int dx = 0, dy = 0;
                if (key[KEY_UP])    dy = -v.vitesse;
                if (key[KEY_DOWN])  dy =  v.vitesse;
                if (key[KEY_LEFT])  dx = -v.vitesse;
                if (key[KEY_RIGHT]) dx =  v.vitesse;
                if (key[KEY_P]) {
                    gerer_pause(page);
                }
                deplacer_vaisseau(&v, dx, dy);

                // 3) Tir joueur
                if (key[KEY_SPACE] && clock() - last_tir >= tir_delay) {
                    for (int i = 0; i < MAX_PROJECTILES; i++) {
                        if (!projectiles[i].actif) {
                            projectiles[i].x     = v.x + v.largeur + 25;
                            projectiles[i].y     = v.y + v.hauteur/2;
                            projectiles[i].actif = 1;
                            last_tir = clock();
                            break;
                        }
                    }
                }

                // 4) Mise à jour des projectiles joueurs
                for (int i = 0; i < MAX_PROJECTILES; i++)
                    deplacer_projectile(&projectiles[i]);

                // 5) Spawn d’ennemis (fréquence selon niveau)
                if (rand() % taux_spawn_ennemis == 0) {
                    for (int i = 0; i < MAX_ENNEMIS; i++) {
                        if (!ennemis[i].actif) {
                            ennemis[i].actif = 1;
                            ennemis[i].x     = SCREEN_W;
                            ennemis[i].y     = rand() % (SCREEN_H - HAUTEUR_ENNEMI);
                            break;
                        }
                    }
                }

                // 6) Spawn de missiles ennemis
                for (int i = 0; i < MAX_ENNEMIS; i++) {
                    if (ennemis[i].actif && rand()%200 == 0) {
                        for (int m = 0; m < MAX_MISSILES; m++) {
                            if (!missiles[m].actif) {
                                missiles[m].actif = 1;
                                missiles[m].x     = ennemis[i].x;
                                missiles[m].y     = ennemis[i].y + HAUTEUR_ENNEMI/2;
                                break;
                            }
                        }
                    }
                }

                // 7) Spawn de cœurs bonus
                if (rand()%200 == 0) {
                    for (int c = 0; c < MAX_COEURS; c++) {
                        if (!coeurs[c].actif) {
                            coeurs[c].actif = 1;
                            coeurs[c].x     = SCREEN_W;
                            coeurs[c].y     = rand()%(SCREEN_H-HAUTEUR_COEUR);
                            break;
                        }
                    }
                }

                // 8) Spawn progressif des obstacles (uniquement si level ≥2)
                if (nombre_obstacles > 0 &&
                    nb_etoiles_actives < nombre_obstacles &&
                    rand() % 60 == 0)
                {
                    for (int i = 0; i < nombre_obstacles; i++) {
                        if (!obstacles[i].actif) {
                            obstacles[i].actif = 1;
                            obstacles[i].x     = SCREEN_W;
                            obstacles[i].y     = rand() % (SCREEN_H - 32);
                            nb_etoiles_actives++;
                            break;
                        }
                    }
                }

                // 9) Mise à jour ennemis & collisions
                for (int i = 0; i < MAX_ENNEMIS; i++) {
                    deplacer_ennemi(&ennemis[i]);
                    if (collision_vaisseau_ennemi(&v, &ennemis[i])) {
                        v.nb_vie--;
                        ennemis[i].actif = 0;
                    }
                    for (int j = 0; j < MAX_PROJECTILES; j++) {
                        detecter_collision_projectile(
                            &projectiles[j],
                            &ennemis[i],
                            &score
                        );
                    }
                }

                // 10) Mise à jour missiles & collisions
                for (int m = 0; m < MAX_MISSILES; m++) {
                    deplacer_missile(&missiles[m]);
                    detecter_collision_missile_vers_vaisseau(&missiles[m], &v);
                }
                // 10bis) neutralisation missile↔projectile
                for (int m = 0; m < MAX_MISSILES; m++) {
                    for (int i = 0; i < MAX_PROJECTILES; i++) {
                        detecter_collision_missile_projectile(
                            &missiles[m], &projectiles[i]
                        );
                    }
                }

                // 11) Mise à jour cœurs & collision bonus
                for (int c = 0; c < MAX_COEURS; c++) {
                    if (coeurs[c].actif) {
                        coeurs[c].x -= vitesse_coeur;
                        if (coeurs[c].x < -LARGEUR_COEUR)
                            coeurs[c].actif = 0;
                        else if (collision_vaisseau_coeur(&v, &coeurs[c])) {
                            if (v.nb_vie < VIES_INITIALES)
                                v.nb_vie++;
                            coeurs[c].actif = 0;
                        }
                    }
                }

                // 12) Déplacement et collision obstacles
                for (int i = 0; i < nombre_obstacles; i++) {
                    if (obstacles[i].actif) {
                        obstacles[i].x -= 2;
                        if (obstacles[i].x < -32) {
                            obstacles[i].actif     = 0;
                            nb_etoiles_actives--;
                        }
                        if (collision_vaisseau_obstacle(&v, &obstacles[i])) {
                            v.nb_vie--;
                            obstacles[i].actif     = 0;
                            nb_etoiles_actives--;
                        }
                    }
                }

                // 13) Affichage final
                draw_sprite(page, vaisseau_img, v.x, v.y);
                // tirs joueur
                for (int i = 0; i < MAX_PROJECTILES; i++) {
                    if (projectiles[i].actif)
                        rectfill(page,
                            projectiles[i].x, projectiles[i].y,
                            projectiles[i].x+5, projectiles[i].y+5,
                            makecol(255,0,0)
                        );
                }
                // missiles ennemis agrandis
                for (int m = 0; m < MAX_MISSILES; m++) {
                    if (missiles[m].actif)
                        circlefill(page,
                            missiles[m].x, missiles[m].y,
                            8, makecol(0,255,255)
                        );
                }
                // ennemis
                for (int i = 0; i < MAX_ENNEMIS; i++) {
                    if (ennemis[i].actif)
                        draw_sprite(page, ennemi_img,
                                    ennemis[i].x, ennemis[i].y);
                }
                // cœurs bonus
                for (int c = 0; c < MAX_COEURS; c++) {
                    if (coeurs[c].actif)
                        stretch_sprite(page, coeur_img,
                            coeurs[c].x, coeurs[c].y,
                            coeur_img->w/5, coeur_img->h/5
                        );
                }

                // 14) HUD
                textprintf_ex(page, font, 10,10, makecol(255,255,255), -1,
                              "Score: %d", score);
                textprintf_ex(page, font, 10,30, makecol(255,255,255), -1,
                              "Vies: %d", v.nb_vie);
                int rem = DUREE_NIVEAU - (int)difftime(time(NULL), debut);
                if (rem<0) rem=0;
                textprintf_ex(page, font, SCREEN_W-100,10,
                              makecol(255,255,0), -1,
                              "Temps: %ds", rem);
                afficher_barre_de_vie(page, &v);
                textprintf_ex(page, font, 10,80, makecol(255,255,255), -1,
                              "Pause : P");
                afficher_obstacles(page, sprite_obstacle, obstacles, nombre_obstacles);

                blit(page, screen, 0,0, 0,0, SCREEN_W, SCREEN_H);
                rest(30);
            }

            // fin de partie → mise à jour de la sauvegarde
            if (score > best_score) best_score = score;
            if (level > best_level) best_level = level;
            ecrire_sauvegarde(pseudo, best_score, best_level);
            rest(300);
        }
        else if (selection == 2) {
            // code pour “Reprendre” si nécessaire
        }
    }

    destroy_bitmap(sprite_obstacle);
    return 0;
}
END_OF_MAIN();
