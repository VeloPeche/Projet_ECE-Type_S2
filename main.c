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
    int t_spawn;   // seconde depuis le début du niveau
    int y;         // ordonnée de spawn
} SpawnEvent;

// Structure pour le boss de fin de niveau 3
typedef struct {
    int x,y,w,h;
    int hp;
    int actif;
    clock_t last_shot;
} Boss;

int main() {
    /* --- Initialisation Allegro --- */
    srand(time(NULL));
    allegro_init();
    install_keyboard();
    install_mouse();
    set_color_depth(32);
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0,0)) {
        allegro_message("Erreur mode graphique");
        return 1;
    }

    /* --- Chargement des bitmaps --- */
    BITMAP *page            = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP *fond_menu       = charger_bitmap_sure("menu5.bmp");
    BITMAP *fond            = charger_bitmap_sure("Fond1600x600.bmp");
    BITMAP *vaisseau_img    = charger_bitmap_sure("Vaisseau.bmp");
    BITMAP *fantome_img     = charger_bitmap_sure("Fantome_rose.bmp");
    BITMAP *ennemi_img      = charger_bitmap_sure("Fantome_rouge.bmp");
    BITMAP *boss_img        = charger_bitmap_sure("boss.bmp");
    BITMAP *coeur_img       = charger_bitmap_sure("coeur_magenta.bmp");
    BITMAP *sprite_obstacle = charger_bitmap_sure("etoile_ennemie.bmp");
    BITMAP *bouclier_img    = charger_bitmap_sure("bouclier.bmp");

    /* --- Saisie du pseudo --- */
    char pseudo[50] = "";
    while (1) {
        clear_bitmap(page);
        draw_sprite(page, fond_menu, 0,0);
        textout_centre_ex(page, font, "Entrez votre pseudo:",
                          SCREEN_W/2, 100, makecol(255,255,255), -1);
        textout_ex(page, font, pseudo,
                   SCREEN_W/2 - 50, 130, makecol(0,255,0), -1);
        blit(page, screen, 0,0, 0,0, SCREEN_W, SCREEN_H);
        if (keypressed()) {
            int k = readkey(), c = k & 0xFF;
            if      (c==8  && strlen(pseudo)>0)    pseudo[strlen(pseudo)-1]=0;
            else if (c==13 && strlen(pseudo)>0)    break;
            else if (isprint(c) && strlen(pseudo)<sizeof(pseudo)-1)
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
                int mx=mouse_x, my=mouse_y;
                if (mx>SCREEN_W/2-100 && mx<SCREEN_W/2+100 &&
                    my>SCREEN_H/2-30  && my<SCREEN_H/2+60)
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
        static SpawnEvent patternF1[66] = {
            {0,10},{0,400},{0,200},{3,300},{3,300},{3,200},{4,370},{4,100},
            {5,20},{6,220},{7,120},{7,310},{8,20},{9,120},{10,345},{11,149},
            {12,230},{12,300},{14,245},{14,355},{14,10},{15,120},{16,50},
            {16,320},{17,150},{17,370},{19,130},{19,260},{19,20},{20,310},
            {20,200},{21,130},{21,20},{23,300},{23,90},{23,170},{24,370},
            {25,300},{27,160},{27,300},{27,370},{28,210},{29,320},{31,360},
            {31,170},{31,260},{31,80},{33,15},{33,335},{34,95},{35,15},
            {35,175},{36,255},{38,75},{38,395},{38,5},{39,135},{40,215},
            {40,355},{42,25},{42,165},{44,305},{44,145},{50,285},{47,25},
            {53,350}
        };
        static SpawnEvent patternF2[] = {
            {0,50},{1,130},{2,210},{3,290},{4,380},{5,80},{5,160},{6,150},
            {7,20},{10,210},{11,200},{11,120},{12,30},{13,110},{14,10},
            {16,80},{16,0},{17,80},{18,30},{19,70},{29,0},{30,80},{31,130},
            {32,210},{33,290},{34,385},{38,100},{38,210},{40,330},{41,190},
            {41,90},{43,70},{48,330},{48,190},{48,10},{49,200},{50,120}
        };
        static SpawnEvent patternF3[] = {
            {1,0},{1,80},{1,160},{1,240},{1,320},{1,400},
            {7,200},{7,280},{11,240},{12,350},{12,250},{15,286},
            {19,230},{19,395},{23,179},{25,230},{28,150},{28,0},
            {29,98},{30,178},{31,250},{32,388},{33,178},{33,108},
            {37,10},{38,108},{49,100},{50,0},{50,80},{52,160},{54,330}
        };

        static SpawnEvent patternE2[] = {
            {6,100},{6,160},{6,250},{8,20},{8,100},{9,200},
            {10,20},{10,100},{13,10},{14,120},{15,10},{16,152},
            {18,0},{19,150},{20,0},{20,152},{20,80},{21,80},
            {23,10},{27,360},{27,0},{27,80},{27,190},{27,280},
            {28,5},{28,85},{28,165},{28,245},{28,325},
            {30,0},{31,50},{32,130},{33,210},{34,290},{35,380},
            {36,380},{39,380},{39,250},{40,230},{40,90},
            {42,300},{42,80},{42,230},{43,159},{43,350},
            {45,0},{45,250},{46,325},{51,10},{52,40}
        };
        static SpawnEvent patternE3[] = {
            {2,15},{2,160},{2,250},{3,20},{3,140},{4,300},{4,390},
            {5,80},{5,220},{5,360},{6,80},{6,290},{6,380},
            {7,80},{8,260},{8,340},{8,180},{9,390},{9,150},
            {10,230},{10,160},{10,233},{11,390},{11,310},
            {13,160},{13,253},{13,335},{14,155},{15,213},
            {16,390},{17,160},{18,243},{18,385},{20,240},
            {20,380},{21,200},{21,390},{22,397},{23,392},
            {24,234},{24,160},{24,0},{25,10},{26,5},
            {26,395},{26,230},{27,0},{28,88},{29,178},
            {29,38},{30,250},{30,350},{31,388},{31,178},
            {34,328},{34,248},{35,0},{35,88},{35,178},
            {36,160},{36,233},{39,200},{39,280},{40,240},
            {40,160},{41,253},{41,100},{42,40},{42,180},
            {44,156},{44,268},{46,8},{46,230},{47,243},
            {47,385},{48,390},{49,310},{50,290},{50,380},
            {51,15},{53,250},{55,400}
        };

        static SpawnEvent patternO2[] = {
            {4,350},{8,280},{10,200},{10,350},{12,200},{12,280},
            {15,200},{16,280},{31,0},{45,350},{52,150},{52,250},
            {55,159},{55,250},{55,350}
        };
        static SpawnEvent patternO3[] = {
            {3,0},{6,10},{9,55},{12,55},{15,55},
            {3,300},{33,380},{33,300},{41,45}
        };

        SpawnEvent *patF=NULL,*patE=NULL,*patO=NULL;
        int lenF=0,lenE=0,lenO=0,idxF=0,idxE=0,idxO=0;
        if (niveau==1) {
            patF=patternF1; lenF=sizeof(patternF1)/sizeof(*patternF1);
        } else if (niveau==2) {
            patF=patternF2; lenF=sizeof(patternF2)/sizeof(*patternF2);
            patE=patternE2; lenE=sizeof(patternE2)/sizeof(*patternE2);
            patO=patternO2; lenO=sizeof(patternO2)/sizeof(*patternO2);
        } else {
            patF=patternF3; lenF=sizeof(patternF3)/sizeof(*patternF3);
            patE=patternE3; lenE=sizeof(patternE3)/sizeof(*patternE3);
            patO=patternO3; lenO=sizeof(patternO3)/sizeof(*patternO3);
        }

        /* --- Initialisation des entités --- */
        Vaisseau v                          = {50,240,5,32,32,VIES_INITIALES,0,0};
        Projectile projectiles[MAX_PROJECTILES] = {{0}};
        Ennemi fantomes[MAX_ENNEMIS]            = {{0}};
        Ennemi ennemisSpeciaux[MAX_ENNEMIS]     = {{0}};
        Missile missiles[MAX_MISSILES]          = {{0}};
        Coeur coeurs[MAX_COEURS]                = {{0}};
        Etoile_ennemie obstacles[30]            = {{0}};
        Bouclier boucliers[MAX_BOUCLIERS]       = {{0}};
        Boss boss                              = {0,0,80,74,20,0,0};

        int nbObs = 0, score = 0;
        clock_t last_tir      = 0;
        const int tir_delay   = 500;
        time_t debut          = time(NULL);
        int vitE              = vitesse_ennemi_selon_niveau(niveau);
        int vitC              = vitesse_coeur_selon_niveau(niveau);
        int screenx           = 0;
        int en_pause          = 0;
        time_t pause_debut    = 0;
        double total_pause    = 0;

        /* --- Boucle de jeu --- */
        while (!key[KEY_ESC]
               && v.nb_vie > 0
               && difftime(time(NULL), debut) - total_pause < DUREE_NIVEAU)
        {
            int t = (int)(difftime(time(NULL), debut) - total_pause);

            // 1) Scrolling
            int scrollSpeed = (niveau >= 2 ? vitE - 1 : 2);
            screenx = (screenx + scrollSpeed) % (fond->w - SCREEN_W);
            clear_bitmap(page);
            blit(fond, page, screenx,0, 0,0, SCREEN_W, SCREEN_H);

            // 2) Déplacement & pause
            int dx=0, dy=0;
            if      (key[KEY_UP])    dy = -v.vitesse;
            else if (key[KEY_DOWN])  dy =  v.vitesse;
            else if (key[KEY_LEFT])  dx = -v.vitesse;
            else if (key[KEY_RIGHT]) dx =  v.vitesse;
            if (key[KEY_P] && !en_pause) {
                en_pause    = 1;
                pause_debut = time(NULL);
                gerer_pause(page);
                total_pause += difftime(time(NULL), pause_debut);
                en_pause    = 0;
            }
            deplacer_vaisseau(&v, dx, dy);

            // 3) Tir joueur
            if (key[KEY_SPACE] && clock() - last_tir >= tir_delay) {
                for (int i=0; i<MAX_PROJECTILES; i++){
                    if (!projectiles[i].actif){
                        projectiles[i].x     = v.x + v.largeur + 25;
                        projectiles[i].y     = v.y + v.hauteur/2;
                        projectiles[i].actif = 1;
                        last_tir = clock();
                        break;
                    }
                }
            }
            for (int i=0; i<MAX_PROJECTILES; i++)
                deplacer_projectile(&projectiles[i]);

            // 4) Spawn fantômes
            while (idxF < lenF && patF[idxF].t_spawn <= t) {
                for (int i=0; i<MAX_ENNEMIS; i++){
                    if (!fantomes[i].actif){
                        fantomes[i].actif = 1;
                        fantomes[i].x     = SCREEN_W;
                        fantomes[i].y     = patF[idxF].y;
                        break;
                    }
                }
                idxF++;
            }

            // 5) Spawn opps
            if (patE) {
                while (idxE < lenE && patE[idxE].t_spawn <= t) {
                    for (int i=0; i<MAX_ENNEMIS; i++){
                        if (!ennemisSpeciaux[i].actif){
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
                while (idxO < lenO && patO[idxO].t_spawn <= t) {
                    for (int i=0; i<30; i++){
                        if (!obstacles[i].actif){
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
            for (int i=0; i<MAX_ENNEMIS; i++){
                if (fantomes[i].actif){
                    fantomes[i].x -= vitE;
                    if (fantomes[i].x < -LARGEUR_ENNEMI)
                        fantomes[i].actif = 0;
                }
                if (!v.bouclier && collision_vaisseau_ennemi(&v, &fantomes[i])){
                    v.nb_vie--;
                    fantomes[i].actif = 0;
                }
                for (int j=0; j<MAX_PROJECTILES; j++)
                    detecter_collision_projectile(&projectiles[j],
                                                  &fantomes[i],
                                                  &score);
            }

            // 8) MAJ & tirs opps + collisions
            for (int i=0; i<MAX_ENNEMIS; i++){
                if (ennemisSpeciaux[i].actif){
                    ennemisSpeciaux[i].x -= vitE;
                    if (ennemisSpeciaux[i].x < -LARGEUR_ENNEMI)
                        ennemisSpeciaux[i].actif = 0;
                    if (!v.bouclier && collision_vaisseau_ennemi(&v, &ennemisSpeciaux[i])){
                        v.nb_vie--;
                        ennemisSpeciaux[i].actif = 0;
                    }
                    if (rand()%150 == 0){
                        for (int m=0; m<MAX_MISSILES; m++){
                            if (!missiles[m].actif){
                                missiles[m].actif = 1;
                                missiles[m].x     = ennemisSpeciaux[i].x;
                                missiles[m].y     = ennemisSpeciaux[i].y + HAUTEUR_ENNEMI/2;
                                break;
                            }
                        }
                    }
                    for (int j=0; j<MAX_PROJECTILES; j++)
                        detecter_collision_projectile(&projectiles[j],
                                                      &ennemisSpeciaux[i],
                                                      &score);
                }
            }

            // 9) MAJ missiles & collisions
            for (int m=0; m<MAX_MISSILES; m++){
                if (missiles[m].actif){
                    missiles[m].x -= (vitE + 2);
                    if (missiles[m].x < 0) missiles[m].actif = 0;
                }
                detecter_collision_missile_vers_vaisseau(&missiles[m], &v);
                for (int i=0; i<MAX_PROJECTILES; i++)
                    detecter_collision_missile_projectile(&missiles[m],
                                                          &projectiles[i]);
            }

            // 10) Spawn & MAJ cœurs
            if (rand()%300 == 0){
                for (int c=0; c<MAX_COEURS; c++){
                    if (!coeurs[c].actif){
                        coeurs[c].actif = 1;
                        coeurs[c].x     = SCREEN_W;
                        coeurs[c].y     = rand()%(SCREEN_H - HAUTEUR_COEUR);
                        break;
                    }
                }
            }
            for (int c=0; c<MAX_COEURS; c++){
                if (coeurs[c].actif){
                    coeurs[c].x -= vitC;
                    if (coeurs[c].x < -LARGEUR_COEUR) coeurs[c].actif = 0;
                    else if (collision_vaisseau_coeur(&v, &coeurs[c])){
                        if (v.nb_vie < VIES_INITIALES) v.nb_vie++;
                        coeurs[c].actif = 0;
                    }
                }
            }

            // 10bis) Spawn & MAJ boucliers
            if (rand()%300 == 0){
                for (int b=0; b<MAX_BOUCLIERS; b++){
                    if (!boucliers[b].actif){
                        boucliers[b].actif = 1;
                        boucliers[b].x     = SCREEN_W;
                        boucliers[b].y     = rand()%(SCREEN_H - 32);
                        break;
                    }
                }
            }
            for (int b=0; b<MAX_BOUCLIERS; b++){
                if (boucliers[b].actif){
                    boucliers[b].x -= VITESSE_BOUCLIER;
                    if (boucliers[b].x < -LARGEUR_BOUCLIER) boucliers[b].actif = 0;
                    else if (collision_vaisseau_bouclier(&v, &boucliers[b])){
                        boucliers[b].actif = 0;
                        v.nb_vie = VIES_INITIALES;
                        v.bouclier = 1;
                        v.bouclierdebut = time(NULL);
                    }
                }
            }
            if (v.bouclier && difftime(time(NULL), v.bouclierdebut) >= DUREE_BOUCLIER)
                v.bouclier = 0;

            // 11) MAJ obstacles & collisions
            for (int i=0; i<30; i++){
                if (obstacles[i].actif){
                    obstacles[i].x -= 2;
                    if (obstacles[i].x < -32){
                        obstacles[i].actif = 0; nbObs--;
                    }
                    if (!v.bouclier && collision_vaisseau_obstacle(&v, &obstacles[i])){
                        v.nb_vie--; obstacles[i].actif = 0; nbObs--;
                    }
                }
            }

            // 12) Boss de fin de niveau 3
            if (!boss.actif && niveau==3 && t >= DUREE_NIVEAU) {
                boss.actif    = 1;
                boss.x        = SCREEN_W;
                boss.y        = SCREEN_H/2 - boss.h/2;
                boss.hp       = 20;
                boss.last_shot= clock();
            }
            if (boss.actif) {
                // déplacement lent vers la gauche
                if (boss.x > SCREEN_W - boss.w - 50) boss.x -= 1;
                draw_sprite(page, boss_img, boss.x, boss.y);
                // tirs en volées
                if (clock() - boss.last_shot > 800) {
                    boss.last_shot = clock();
                    for (int k=0; k<5; k++){
                        for (int m=0; m<MAX_MISSILES; m++){
                            if (!missiles[m].actif){
                                missiles[m].actif = 1;
                                missiles[m].x     = boss.x;
                                missiles[m].y     = boss.y + k*(boss.h/5);
                                break;
                            }
                        }
                    }
                }
                // collisions projectiles ↔ boss
                for (int p=0; p<MAX_PROJECTILES; p++){
                    if (projectiles[p].actif
                     && projectiles[p].x >= boss.x
                     && projectiles[p].x <= boss.x+boss.w
                     && projectiles[p].y >= boss.y
                     && projectiles[p].y <= boss.y+boss.h)
                    {
                        projectiles[p].actif = 0;
                        if (--boss.hp <= 0) {
                            boss.actif = 0;
                            score += 500;
                        }
                    }
                }
            }

            // 13) Affichage final de toutes les entités
            draw_sprite(page, vaisseau_img, v.x, v.y);
            for (int i=0; i<MAX_PROJECTILES; i++){
                if (projectiles[i].actif)
                    rectfill(page,
                             projectiles[i].x, projectiles[i].y,
                             projectiles[i].x+5, projectiles[i].y+5,
                             makecol(255,0,0));
            }
            for (int m=0; m<MAX_MISSILES; m++){
                if (missiles[m].actif)
                    circlefill(page,
                               missiles[m].x, missiles[m].y,
                               8, makecol(0,255,255));
            }
            for (int i=0; i<MAX_ENNEMIS; i++){
                if (fantomes[i].actif)
                    draw_sprite(page, fantome_img, fantomes[i].x, fantomes[i].y);
                if (ennemisSpeciaux[i].actif)
                    draw_sprite(page, ennemi_img, ennemisSpeciaux[i].x, ennemisSpeciaux[i].y);
            }
            for (int c=0; c<MAX_COEURS; c++){
                if (coeurs[c].actif)
                    stretch_sprite(page, coeur_img,
                                   coeurs[c].x, coeurs[c].y,
                                   coeur_img->w/5, coeur_img->h/5);
            }
            for (int b=0; b<MAX_BOUCLIERS; b++){
                if (boucliers[b].actif)
                    stretch_sprite(page, bouclier_img,
                                   boucliers[b].x, boucliers[b].y,
                                   bouclier_img->w/20, bouclier_img->h/20);
            }
            for (int i=0; i<30; i++){
                if (obstacles[i].actif)
                    draw_sprite(page, sprite_obstacle,
                                obstacles[i].x, obstacles[i].y);
            }

            // 14) HUD
            textprintf_ex(page, font, 10,10, makecol(255,255,255), -1,
                          "Score: %d", score);
            textprintf_ex(page, font, 10,30, makecol(255,255,255), -1,
                          "Vies: %d", v.nb_vie);
            textprintf_ex(page, font, SCREEN_W-100,10, makecol(255,255,0), -1,
                          "Temps: %ds", DUREE_NIVEAU - t);
            afficher_barre_de_vie(page, &v);
            textprintf_centre_ex(page, font, SCREEN_W/2,10,
                                 makecol(255,255,255), -1,
                                 "Niveau %d", niveau);
            textprintf_ex(page, font, SCREEN_W-100,30,
                          makecol(255,255,255), -1,
                          "Pause: P");

            // Barre bouclier si actif
            if (v.bouclier) {
                int restant = DUREE_BOUCLIER - (int)difftime(time(NULL), v.bouclierdebut);
                if (restant<0) restant=0;
                int maxW=100, curW = restant * maxW / DUREE_BOUCLIER;
                rectfill(page, 10,80, 10+maxW,80+10, makecol(100,100,100));
                rectfill(page, 10,80, 10+curW,80+10, makecol(0,255,255));
                rect(page, 10,80, 10+maxW,80+10, makecol(255,255,255));
                textprintf_ex(page, font, 10,70, makecol(0,255,255), -1,
                              "Bouclier : %ds", restant);
            }

            blit(page, screen, 0,0, 0,0, SCREEN_W, SCREEN_H);
            rest(30);
        }

        /* --- Fin du jeu (Game Over) --- */
        int nouvelle_largeur  = SCREEN_W / 1;
        int nouvelle_hauteur  = SCREEN_H / 1;

        int x_centre = (SCREEN_W - nouvelle_largeur) / 1;
        int y_centre = (SCREEN_H - nouvelle_hauteur) / 1;

        stretch_sprite(page, gameover_img, x_centre, y_centre, nouvelle_largeur, nouvelle_hauteur);

        textprintf_centre_ex(page, font, SCREEN_W/2, SCREEN_H/2 - 40,
                             makecol(255, 0, 0), -1,
                             "GAME OVER");

        // Lire la sauvegarde
        int best_score_sauvegarde = 0, best_level_sauvegarde = 0;
        if (lire_sauvegarde(pseudo, &best_score_sauvegarde, &best_level_sauvegarde)) {
            // Afficher les scores récupérés sous le message
            textprintf_centre_ex(page, font, SCREEN_W/2, SCREEN_H/2 + 10,
                                 makecol(255, 255, 255), -1,
                                 "Score : %d", score);
            textprintf_centre_ex(page, font, SCREEN_W/2, SCREEN_H/2 + 30,
                                 makecol(255, 255, 255), -1,
                                 "Meilleur score : %d", best_score_sauvegarde);
            textprintf_centre_ex(page, font, SCREEN_W/2, SCREEN_H/2 + 50,
                                 makecol(255, 255, 255), -1,
                                 "Meilleur niveau : %d", best_level_sauvegarde);
        } else {
            // Si pas de sauvegarde, juste afficher score actuel
            textprintf_centre_ex(page, font, SCREEN_W/2, SCREEN_H/2 + 10,
                                 makecol(255, 255, 255), -1,
                                 "Score : %d", score);
            textprintf_centre_ex(page, font, SCREEN_W/2, SCREEN_H/2 + 30,
                                 makecol(255, 255, 255), -1,
                                 "Aucune sauvegarde");
        }

        blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        ecrire_sauvegarde(pseudo,
                         score > best_score ? score : best_score,
                         niveau > best_level ? niveau : best_level);
        /* --- Sauvegarde --- */
        ecrire_sauvegarde(pseudo,
                         score > best_score ? score : best_score,
                         niveau > best_level ? niveau : best_level);

        while (!key[KEY_ENTER] && !key[KEY_ESC]) rest(100);
    }

    destroy_bitmap(sprite_obstacle);
    destroy_bitmap(boss_img);
    return 0;
}
END_OF_MAIN();
