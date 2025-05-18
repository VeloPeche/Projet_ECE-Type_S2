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

// Structure décrivant un événement de spawn
typedef struct {
    int t_spawn;   // seconde depuis le début
    int y;         // ordonnée de spawn
} SpawnEvent;

int main() {
    /* --- Initialisation Allegro --- */
    srand(time(NULL));
    allegro_init();
    install_keyboard();
    install_mouse();
    set_color_depth(32);
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0)) {
        allegro_message("Erreur mode graphique");
        return 1;
    }

    /* --- Chargement des bitmaps --- */
    BITMAP *page            = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP *fond_menu       = charger_bitmap_sure("menu5.bmp");
    BITMAP *fond            = charger_bitmap_sure("Fond1600x600.bmp");
    BITMAP *vaisseau_img    = charger_bitmap_sure("Vaisseau.bmp");
    BITMAP *fantome_img     = charger_bitmap_sure("Fantome_rose.bmp");
    BITMAP *ennemi_img      = charger_bitmap_sure("opps.bmp");
    BITMAP *coeur_img       = charger_bitmap_sure("coeur_magenta.bmp");
    BITMAP *sprite_obstacle = charger_bitmap_sure("etoile_ennemie.bmp");

    /* --- Saisie du pseudo --- */
    char pseudo[50] = "";
    while (1) {
        clear_bitmap(page);
        draw_sprite(page, fond_menu, 0, 0);
        textout_centre_ex(page, font, "Entrez votre pseudo:",
                          SCREEN_W/2, 100, makecol(255,255,255), -1);
        textout_ex(page, font, pseudo,
                   SCREEN_W/2 - 50, 130, makecol(0,255,0), -1);
        blit(page, screen, 0,0, 0,0, SCREEN_W, SCREEN_H);
        if (keypressed()) {
            int k = readkey(), c = k & 0xFF;
            if      (c == 8  && strlen(pseudo) > 0) pseudo[strlen(pseudo)-1] = 0;
            else if (c == 13 && strlen(pseudo) > 0) break;
            else if (isprint(c) && strlen(pseudo) < sizeof(pseudo)-1)
                strncat(pseudo, (char[]){c,0}, 1);
        }
        rest(30);
    }
    clear_keybuf();
    rest(500);

    /* --- Chargement de la sauvegarde --- */
    int best_score = -1, best_level = -1;
    lire_sauvegarde(pseudo, &best_score, &best_level);

    /* --- Boucle principale menu → jeu → menu --- */
    while (1) {
        int selection = 1;
        /* --- Affichage du menu --- */
        while (1) {
            afficher_menu_principal(page, pseudo, selection,
                                    fond_menu, best_score, best_level);
            if      (key[KEY_UP])    { if (selection>1) selection--; rest(150); }
            else if (key[KEY_DOWN])  { if (selection<3) selection++; rest(150); }
            else if (key[KEY_ENTER]) {
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

        /* --- Choix du niveau --- */
        int niveau = demander_niveau(page, fond_menu);

        /* --- Définition des patterns selon le niveau --- */
        static SpawnEvent patternF1[] = {
            {3,64},{6,80},{9,200},{10,50},{12,112},
            {12,64},{15,64},{18,104},{18,321},{21,221},
            {21,64},{24,316},{27,46},{6,222}
        };
        static SpawnEvent patternF2[] = {
            {2,300},{3,250},{4,200},{5,150},{6,100},
            {10, 80},{10,160},{10,240},{10,320},
            {14,120},{15,170},{16,220},{17,270},{18,320}
        };
        static SpawnEvent patternF3[] = {
            {1,60},{1,120},{1,180},{1,240},{1,300},{1,360},
            {4,100},{5,200},{6,80},{7,280},{8,120},{9,320},
            {12,60},{12,120},{12,180},{12,240},{12,300},{12,360},
            {16,150},{16,200},{16,250},{16,300}
        };

        static SpawnEvent patternE2[] = {
            {6,100},{6,160},{6,220},
            {12, 80},{12,140},{12,200},
            {18,120},{18,180},{18,240}
        };
        static SpawnEvent patternE3[] = {
            {5,300},{6,260},{7,220},{8,180},{9,140},
            {12,100},{13,140},{14,180},{15,220},{16,260}
        };

        static SpawnEvent patternO2[] = {
            {4,200},{8,200},{12,200},{16,200},{20,200}
        };
        static SpawnEvent patternO3[] = {
            {3,80},{6,80},{9,80},{12,80},{15,80},
            {3,300},{6,300},{9,300},{12,300},{15,300},
            {18,120},{19,160},{20,200},{21,240},{22,280}
        };

        SpawnEvent *patF = NULL, *patE = NULL, *patO = NULL;
        int lenF=0,lenE=0,lenO=0, idxF=0,idxE=0,idxO=0;

        if (niveau==1) {
            patF = patternF1; lenF = sizeof(patternF1)/sizeof(*patternF1);
        } else if (niveau==2) {
            patF = patternF2; lenF = sizeof(patternF2)/sizeof(*patternF2);
            patE = patternE2; lenE = sizeof(patternE2)/sizeof(*patternE2);
            patO = patternO2; lenO = sizeof(patternO2)/sizeof(*patternO2);
        } else {
            patF = patternF3; lenF = sizeof(patternF3)/sizeof(*patternF3);
            patE = patternE3; lenE = sizeof(patternE3)/sizeof(*patternE3);
            patO = patternO3; lenO = sizeof(patternO3)/sizeof(*patternO3);
        }

        /* --- Initialisation des entités --- */
        Vaisseau v                         = {50,240,5,32,32,VIES_INITIALES};
        Projectile projectiles[MAX_PROJECTILES] = {{0}};
        Ennemi fantomes[MAX_ENNEMIS]            = {{0}};
        Ennemi ennemisSpeciaux[MAX_ENNEMIS]     = {{0}};
        Missile missiles[MAX_MISSILES]          = {{0}};
        Coeur coeurs[MAX_COEURS]                = {{0}};
        Etoile_ennemie obstacles[30]            = {{0}};
        int nbObs = 0, score = 0;

        clock_t last_tir      = 0;
        const int tir_delay   = 500;
        time_t debut          = time(NULL);
        int vitE              = vitesse_ennemi_selon_niveau(niveau);
        int vitC              = vitesse_coeur_selon_niveau(niveau);
        int screenx           = 0;

        /* --- Boucle de jeu --- */
        while (!key[KEY_ESC]
               && v.nb_vie > 0
               && difftime(time(NULL), debut) < DUREE_NIVEAU)
        {
            int t = (int)difftime(time(NULL), debut);

            // 1) Scrolling
            screenx = (screenx + 2) % (fond->w - SCREEN_W);
            clear_bitmap(page);
            blit(fond, page, screenx, 0, 0, 0, SCREEN_W, SCREEN_H);

            // 2) Déplacement & pause
            int dx = 0, dy = 0;
            if (key[KEY_UP])    dy = -v.vitesse;
            if (key[KEY_DOWN])  dy =  v.vitesse;
            if (key[KEY_LEFT])  dx = -v.vitesse;
            if (key[KEY_RIGHT]) dx =  v.vitesse;
            if (key[KEY_P])     gerer_pause(page);
            deplacer_vaisseau(&v, dx, dy);

            // 3) Tir du joueur
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
            for (int i = 0; i < MAX_PROJECTILES; i++)
                deplacer_projectile(&projectiles[i]);

            // 4) Spawn fantômes
            while (idxF < lenF && patF[idxF].t_spawn == t) {
                for (int i = 0; i < MAX_ENNEMIS; i++) {
                    if (!fantomes[i].actif) {
                        fantomes[i].actif = 1;
                        fantomes[i].x     = SCREEN_W;
                        fantomes[i].y     = patF[idxF].y;
                        break;
                    }
                }
                idxF++;
            }

            // 5) Spawn opps (ennemis spéciaux)
            if (patE) {
                while (idxE < lenE && patE[idxE].t_spawn == t) {
                    for (int i = 0; i < MAX_ENNEMIS; i++) {
                        if (!ennemisSpeciaux[i].actif) {
                            ennemisSpeciaux[i].actif = 1;
                            ennemisSpeciaux[i].x     = SCREEN_W;
                            ennemisSpeciaux[i].y     = patE[idxE].y;
                            break;
                        }
                    }
                    idxE++;
                }
            }

            // 6) Spawn obstacles
            if (patO) {
                while (idxO < lenO && patO[idxO].t_spawn == t) {
                    for (int i = 0; i < 30; i++) {
                        if (!obstacles[i].actif) {
                            obstacles[i].actif = 1;
                            obstacles[i].x     = SCREEN_W;
                            obstacles[i].y     = patO[idxO].y;
                            nbObs++;
                            break;
                        }
                    }
                    idxO++;
                }
            }

            // 7) MAJ & collisions fantômes
            for (int i = 0; i < MAX_ENNEMIS; i++) {
                if (fantomes[i].actif) {
                    fantomes[i].x -= vitE;
                    if (fantomes[i].x < -LARGEUR_ENNEMI)
                        fantomes[i].actif = 0;
                }
                if (collision_vaisseau_ennemi(&v, &fantomes[i])) {
                    v.nb_vie--;
                    fantomes[i].actif = 0;
                }
                for (int j = 0; j < MAX_PROJECTILES; j++)
                    detecter_collision_projectile(&projectiles[j],
                                                  &fantomes[i],
                                                  &score);
            }

            // 8) MAJ & tirs opps
            for (int i = 0; i < MAX_ENNEMIS; i++) {
                if (ennemisSpeciaux[i].actif) {
                    ennemisSpeciaux[i].x -= vitE;
                    if (ennemisSpeciaux[i].x < -LARGEUR_ENNEMI)
                        ennemisSpeciaux[i].actif = 0;
                    if (collision_vaisseau_ennemi(&v, &ennemisSpeciaux[i])) {
                        v.nb_vie--;
                        ennemisSpeciaux[i].actif = 0;
                    }
                    if (rand() % 150 == 0) {
                        for (int m = 0; m < MAX_MISSILES; m++) {
                            if (!missiles[m].actif) {
                                missiles[m].actif = 1;
                                missiles[m].x     = ennemisSpeciaux[i].x;
                                missiles[m].y     = ennemisSpeciaux[i].y + HAUTEUR_ENNEMI/2;
                                break;
                            }
                        }
                    }
                    // collision projectile↔opps
                    for (int j = 0; j < MAX_PROJECTILES; j++)
                        detecter_collision_projectile(&projectiles[j],
                                                      &ennemisSpeciaux[i],
                                                      &score);
                }
            }

            // 9) MAJ missiles & collisions
            for (int m = 0; m < MAX_MISSILES; m++) {
                if (missiles[m].actif) {
                    missiles[m].x -= (vitE + 2);
                    if (missiles[m].x < 0)
                        missiles[m].actif = 0;
                }
                detecter_collision_missile_vers_vaisseau(&missiles[m], &v);
                for (int i = 0; i < MAX_PROJECTILES; i++)
                    detecter_collision_missile_projectile(&missiles[m],
                                                          &projectiles[i]);
            }

            // 10) Spawn & MAJ cœurs
            if (rand() % 200 == 0) {
                for (int c = 0; c < MAX_COEURS; c++) {
                    if (!coeurs[c].actif) {
                        coeurs[c].actif = 1;
                        coeurs[c].x     = SCREEN_W;
                        coeurs[c].y     = rand() % (SCREEN_H - HAUTEUR_COEUR);
                        break;
                    }
                }
            }
            for (int c = 0; c < MAX_COEURS; c++) {
                if (coeurs[c].actif) {
                    coeurs[c].x -= vitC;
                    if (coeurs[c].x < -LARGEUR_COEUR)
                        coeurs[c].actif = 0;
                    else if (collision_vaisseau_coeur(&v, &coeurs[c])) {
                        if (v.nb_vie < VIES_INITIALES)
                            v.nb_vie++;
                        coeurs[c].actif = 0;
                    }
                }
            }

            // 11) MAJ obstacles & collisions
            for (int i = 0; i < 30; i++) {
                if (obstacles[i].actif) {
                    obstacles[i].x -= 2;
                    if (obstacles[i].x < -32) {
                        obstacles[i].actif = 0;
                        nbObs--;
                    }
                    if (collision_vaisseau_obstacle(&v, &obstacles[i])) {
                        v.nb_vie--;
                        obstacles[i].actif = 0;
                        nbObs--;
                    }
                }
            }

            // 12) Affichage final
            draw_sprite(page, vaisseau_img, v.x, v.y);
            for (int i = 0; i < MAX_PROJECTILES; i++) {
                if (projectiles[i].actif)
                    rectfill(page,
                             projectiles[i].x, projectiles[i].y,
                             projectiles[i].x + 5, projectiles[i].y + 5,
                             makecol(255,0,0));
            }
            for (int m = 0; m < MAX_MISSILES; m++) {
                if (missiles[m].actif)
                    circlefill(page,
                               missiles[m].x, missiles[m].y,
                               8, makecol(0,255,255));
            }
            for (int i = 0; i < MAX_ENNEMIS; i++) {
                if (fantomes[i].actif)
                    draw_sprite(page, fantome_img,
                                fantomes[i].x, fantomes[i].y);
                if (ennemisSpeciaux[i].actif)
                    draw_sprite(page, ennemi_img,
                                ennemisSpeciaux[i].x, ennemisSpeciaux[i].y);
            }
            for (int c = 0; c < MAX_COEURS; c++) {
                if (coeurs[c].actif)
                    stretch_sprite(page, coeur_img,
                                   coeurs[c].x, coeurs[c].y,
                                   coeur_img->w/5, coeur_img->h/5);
            }
            for (int i = 0; i < 30; i++) {
                if (obstacles[i].actif)
                    draw_sprite(page, sprite_obstacle,
                                obstacles[i].x, obstacles[i].y);
            }

            textprintf_ex(page, font, 10,10, makecol(255,255,255), -1,
                          "Score: %d", score);
            textprintf_ex(page, font, 10,30, makecol(255,255,255), -1,
                          "Vies: %d", v.nb_vie);
            textprintf_ex(page, font, SCREEN_W-100,10,
                          makecol(255,255,0), -1,
                          "Temps: %ds", DUREE_NIVEAU - t);
            afficher_barre_de_vie(page, &v);
            textprintf_ex(page, font, 10,80, makecol(255,255,255), -1,
                          "Pause: P");

            blit(page, screen, 0,0, 0,0, SCREEN_W, SCREEN_H);
            rest(30);
        }

        /* --- Fin de partie → sauvegarde --- */
        ecrire_sauvegarde(pseudo,
                          score > best_score ? score : best_score,
                          niveau > best_level ? niveau : best_level);
    }

    destroy_bitmap(sprite_obstacle);
    return 0;
}
END_OF_MAIN();
