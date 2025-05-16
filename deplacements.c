#include "deplacements.h"
#include "utilitaires.h"
#include "deplacements.h"

// … vos autres includes …
// ... vos fonctions existantes ...

int detecter_collision_missile_projectile(Missile *m, Projectile *p) {
    if (!m->actif || !p->actif) return 0;
    // On considère le missile comme un carré de 2*R x 2*R centré sur (x,y)
    const int R = 6;              // même rayon que pour le rendu
    const int PW = 5, PH = 5;     // taille du projectile joueur
    if (p->x < m->x + R && p->x + PW > m->x - R &&
        p->y < m->y + R && p->y + PH > m->y - R)
    {
        m->actif = 0;
        p->actif = 0;
        return 1;
    }
    return 0;
}

void deplacer_missile(Missile *m){
    if(!m->actif) return;
    m->x -= VITESSE_MISSILE;
    if(m->x < 0) m->actif = 0;
}

void detecter_collision_missile_vers_vaisseau(Missile *m, Vaisseau *v){
    if(!m->actif) return;
    // AABB missile 1×1 (ou taille que vous voulez) vs vaisseau
    if( v->x < m->x && v->x + v->largeur > m->x &&
        v->y < m->y && v->y + v->hauteur > m->y )
    {
        m->actif = 0;
        v->nb_vie--;
    }
}

void deplacer_vaisseau(Vaisseau *v,int dx,int dy){
    v->x+=dx; v->y+=dy;
    if(v->x<0) v->x=0;
    if(v->x>SCREEN_W-v->largeur) v->x=SCREEN_W-v->largeur;
    if(v->y<0) v->y=0;
    if(v->y>SCREEN_H-v->hauteur) v->y=SCREEN_H-v->hauteur;
}

void deplacer_projectile(Projectile *p){
    if(p->actif){
        p->x+=5;
        if(p->x>SCREEN_W) p->actif=0;
    }
}

void deplacer_ennemi(Ennemi *e){
    if(e->actif){
        e->x-=VITESSE_ENNEMI;
        if(e->x< -LARGEUR_ENNEMI) e->actif=0;
    }
}

void deplacer_coeur(Coeur *c){
    if(c->actif){
        c->x-=VITESSE_COEUR;
        if(c->x< -LARGEUR_COEUR) c->actif=0;
    }
}

void detecter_collision_projectile(Projectile *pr, Ennemi *en, int *score){
    if(pr->actif && en->actif &&
       pr->x>=en->x && pr->x<=en->x+LARGEUR_ENNEMI &&
       pr->y>=en->y && pr->y<=en->y+HAUTEUR_ENNEMI)
    {
        pr->actif=0; en->actif=0; *score+=100;
    }
}

int collision_vaisseau_ennemi(const Vaisseau *v, const Ennemi *e){
    return e->actif &&
           v->x<e->x+LARGEUR_ENNEMI && v->x+v->largeur>e->x &&
           v->y<e->y+HAUTEUR_ENNEMI && v->y+v->hauteur>e->y;
}

int collision_vaisseau_coeur(const Vaisseau *v, const Coeur *c){
    return c->actif &&
           v->x<c->x+LARGEUR_COEUR && v->x+v->largeur>c->x &&
           v->y<c->y+HAUTEUR_COEUR && v->y+v->hauteur>c->y;
}

void initialiser_etoiles(Etoile_ennemie etoiles[], Etoile_ennemie obstacles[], int niveau) {
    int nombre_etoiles = 0;
    int nombre_obstacles = 0;

    if (niveau == 1) {
        nombre_etoiles = MAX_ETOILES_NIVEAU_1;
        nombre_obstacles = 15;
        int positions_etoiles[15][2] = {
            {100, 100}, {200, 150}, {300, 200}, {400, 250}, {500, 300},
            {150, 350}, {250, 400}, {350, 450}, {450, 500}, {550, 550},
            {600, 100}, {650, 150}, {700, 200}, {750, 250}, {800, 300}
        };
        int positions_obstacles[15][2] = {
            {120, 120}, {220, 180}, {320, 220}, {420, 260}, {520, 310},
            {160, 360}, {260, 410}, {360, 460}, {460, 510}, {560, 560},
            {610, 120}, {660, 160}, {710, 210}, {760, 260}, {810, 310}
        };
        for (int i = 0; i < nombre_etoiles; i++) {
            etoiles[i].x = positions_etoiles[i][0];
            etoiles[i].y = positions_etoiles[i][1];
            etoiles[i].actif = 1;
        }
        for (int i = 0; i < nombre_obstacles; i++) {
            obstacles[i].x = positions_obstacles[i][0];
            obstacles[i].y = positions_obstacles[i][1];
            obstacles[i].actif = 1;
        }
    }
    else if (niveau == 2) {
        nombre_etoiles = MAX_ETOILES_NIVEAU_2;
        nombre_obstacles = 20;
        int positions_etoiles[22][2] = {
            {100, 100}, {200, 120}, {300, 140}, {400, 160}, {500, 180},
            {600, 200}, {150, 220}, {250, 240}, {350, 260}, {450, 280},
            {550, 300}, {650, 320}, {750, 340}, {850, 360}, {950, 380},
            {100, 400}, {200, 420}, {300, 440}, {400, 460}, {500, 480},
            {600, 500}, {700, 520}
        };
        int positions_obstacles[20][2] = {
            {120, 100}, {220, 140}, {320, 160}, {420, 180}, {520, 200},
            {620, 220}, {170, 240}, {270, 260}, {370, 280}, {470, 300},
            {570, 320}, {670, 340}, {770, 360}, {870, 380}, {970, 400},
            {120, 420}, {220, 440}, {320, 460}, {420, 480}, {520, 500}
        };
        for (int i = 0; i < nombre_etoiles; i++) {
            etoiles[i].x = positions_etoiles[i][0];
            etoiles[i].y = positions_etoiles[i][1];
            etoiles[i].actif = 1;
        }
        for (int i = 0; i < nombre_obstacles; i++) {
            obstacles[i].x = positions_obstacles[i][0];
            obstacles[i].y = positions_obstacles[i][1];
            obstacles[i].actif = 1;
        }
    }
    else if (niveau == 3) {
        nombre_etoiles = MAX_ETOILES_NIVEAU_3;
        nombre_obstacles = 25;
        int positions_etoiles[25][2] = {
            {100, 100}, {150, 120}, {200, 140}, {250, 160}, {300, 180},
            {350, 200}, {400, 220}, {450, 240}, {500, 260}, {550, 280},
            {600, 300}, {650, 320}, {700, 340}, {750, 360}, {800, 380},
            {850, 400}, {900, 420}, {950, 440}, {100, 460}, {150, 480},
            {200, 500}, {250, 520}, {300, 540}, {350, 560}, {400, 580}
        };
        int positions_obstacles[25][2] = {
            {110, 110}, {160, 130}, {210, 150}, {260, 170}, {310, 190},
            {360, 210}, {410, 230}, {460, 250}, {510, 270}, {560, 290},
            {610, 310}, {660, 330}, {710, 350}, {760, 370}, {810, 390},
            {860, 410}, {910, 430}, {960, 450}, {110, 470}, {160, 490},
            {210, 510}, {260, 530}, {310, 550}, {360, 570}, {410, 590}
        };
        for (int i = 0; i < nombre_etoiles; i++) {
            etoiles[i].x = positions_etoiles[i][0];
            etoiles[i].y = positions_etoiles[i][1];
            etoiles[i].actif = 1;
        }
        for (int i = 0; i < nombre_obstacles; i++) {
            obstacles[i].x = positions_obstacles[i][0];
            obstacles[i].y = positions_obstacles[i][1];
            obstacles[i].actif = 1;
        }
    }
}


int collision_vaisseau_obstacle(Vaisseau* v, Etoile_ennemie* e) {
    return e->actif &&
           v->x < e->x + 32 &&
           v->x + v->largeur > e->x &&
           v->y < e->y + 32 &&
           v->y + v->hauteur > e->y;
}

