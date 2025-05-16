#include "deplacements.h"
#include "utilitaires.h"
#include "deplacements.h"

// … vos autres includes …
// ... vos fonctions existantes ...

#define MARGE_BAS 12
#define MARGE_DROITE 30

Ennemi ennemis_niveau1[150] = {
    // Groupe 1, apparition = 0
    {640, 10,   0, 0},
    {645, 400,   0, 0},
    {650, 200,   0, 0},

    // Groupe 2, apparition = 54
    {655, 300,  54, 0},
    {660, 300,  70, 0},
    {665, 200,  54, 0},

    // Groupe 3, apparition = 90
    {670, 370,  90, 0},
    {675, 20,  90, 0},
    {680, 100,  90, 0},

    // Groupe 4, apparition = 162
    {685, 120, 162, 0},
    {690, 310, 162, 0},
    {695, 420, 162, 0},

    // Groupe 5, apparition = 216
    {640, 20,  216, 0},
    {645, 420,  216, 0},
    {650, 345, 216, 0},

    // Groupe 6, apparition = 270
    {655, 149, 270, 0},
    {660, 230, 270, 0},
    {665, 360, 270, 0},

    // Groupe 7, apparition = 324
    {670, 265, 324, 0},
    {675, 355, 324, 0},
    {680, 10, 324, 0},

    // Groupe 8, apparition = 378
    {685, 320, 378, 0},
    {690, 20, 378, 0},
    {695, 360, 378, 0},

    // Groupe 9, apparition = 432
    {640, 30,  432, 0},
    {645, 120,  432, 0},
    {650, 340, 432, 0},

    // Groupe 10, apparition = 486
    {655, 110, 486, 0},
    {660, 190, 486, 0},
    {665, 260, 486, 0},

    // Groupe 11, apparition = 540
    {670, 270, 540, 0},
    {675, 310, 540, 0},
    {680, 350, 540, 0},

    // Groupe 12, apparition = 594
    {685, 390, 594, 0},
    {690, 30, 594, 0},
    {695, 230, 594, 0},

    // Groupe 13, apparition = 648
    {640, 40,  648, 0},
    {645, 100,  648, 0},
    {650, 190, 648, 0},

    // Groupe 14, apparition = 702
    {655, 160, 702, 0},
    {660, 300, 702, 0},
    {665, 340, 702, 0},

    // Groupe 15, apparition = 756
    {670, 280, 756, 0},
    {675, 320, 756, 0},
    {680, 360, 756, 0},

    // Groupe 16, apparition = 810
    {685, 400, 810, 0},
    {690, 240, 810, 0},
    {695, 180, 810, 0},

    // Groupe 17, apparition = 864
    {640, 15,  864, 0},
    {645, 335,  864, 0},
    {650, 95,  864, 0},

    // Groupe 18, apparition = 918
    {655, 35, 918, 0},
    {660, 175, 918, 0},
    {665, 255, 918, 0},

    // Groupe 19, apparition = 972
    {670, 255, 972, 0},
    {675, 395, 972, 0},
    {680, 35, 972, 0},

    // Groupe 20, apparition = 1026
    {685, 375, 1026, 0},
    {690, 215, 1026, 0},
    {695, 355, 1026, 0},

    // Groupe 21, apparition = 1080
    {640, 25, 1080, 0},
    {645, 165, 1080, 0},
    {650, 105,1080, 0},

    // Groupe 22, apparition = 1134
    {655, 145,1134, 0},
    {660, 285,1134, 0},
    {665, 25,1134, 0},

    // Groupe 23, apparition = 1188
    {670, 265,1188, 0},
    {675, 405,1188, 0},
    {680, 345,1188, 0},

    // Groupe 24, apparition = 1242
    {685, 85,1242, 0},
    {690, 425,1242, 0},
    {695, 365,1242, 0},

    // Groupe 25, apparition = 1296
    {640, 35, 1296, 0},
    {645, 175, 1296, 0},
    {650, 115,1296, 0},

    // Groupe 26, apparition = 1350
    {655, 155,1350, 0},
    {660, 95,1350, 0},
    {665, 235,1350, 0},

    // Groupe 27, apparition = 1404
    {670, 275,1404, 0},
    {675, 15,1404, 0},
    {680, 355,1404, 0},

    // Groupe 28, apparition = 1458
    {685, 395,1458, 0},
    {690, 35,1458, 0},
    {695, 355,1458, 0},

    // Groupe 29, apparition = 1512
    {640, 45, 1512, 0},
    {645, 185, 1512, 0},
    {650, 325,1512, 0},

    // Groupe 30, apparition = 1566
    {655, 135,1566, 0},
    {660, 205,1566, 0},
    {665, 345,1566, 0},

    // Groupe 31, apparition = 1620
    {670, 285,1620, 0},
    {675, 25,1620, 0},
    {680, 365,1620, 0},

    // Groupe 32, apparition = 1674
    {685, 405,1674, 0},
    {690, 45,1674, 0},
    {695, 385,1674, 0},

    // Groupe 33, apparition = 1728
    {640, 5,  1728, 0},
    {645, 345, 1728, 0},
    {650, 85, 1728, 0},
    {655, 125,1782, 0},
{640,  20,    0, 0},
{645, 120,   30, 0},
{650, 220,   60, 0},
{655, 320,   90, 0},
{660, 420,  120, 0},
{665,  60,  150, 0},
{670, 160,  180, 0},
{675, 260,  210, 0},
{680, 360,  240, 0},
{685, 100,  270, 0},
{690, 200,  300, 0},
{695, 300,  330, 0},
{640, 400,  360, 0},
{645,  40,  390, 0},
{650, 140,  420, 0},
{655, 240,  450, 0},
{660, 340,  480, 0},
{665, 440,  510, 0},
{670,  80,  540, 0},
{675, 180,  570, 0},
{680, 280,  600, 0},
{685, 380,  630, 0},
{690,  20,  660, 0},
{695, 120,  690, 0},
{640, 220,  720, 0},
{645, 320,  750, 0},
{650, 420,  780, 0},
{655,  60,  810, 0},
{660, 160,  840, 0},
{665, 260,  870, 0},
{670, 360,  900, 0},
{675, 100,  930, 0},
{680, 200,  960, 0},
{685, 300,  990, 0},
{690, 400, 1020, 0},
{695,  40, 1050, 0},
{640, 140, 1080, 0},
{645, 240, 1110, 0},
{650, 340, 1140, 0},
{655, 440, 1170, 0},
{660,  80, 1200, 0},
{665, 180, 1230, 0},
{670, 280, 1260, 0},
{675, 380, 1290, 0},
{680,  20, 1320, 0},
{685, 120, 1350, 0},
{690, 220, 1380, 0},
{695, 320, 1410, 0},
{640, 420, 1440, 0},
{645,  60, 1470, 0}
};


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


void deplacer_vaisseau(Vaisseau *v, int dx, int dy) {
    v->x += dx;
    v->y += dy;

    // Limite gauche
    if (v->x < 0)
        v->x = 0;
    // Limite droite
    if (v->x > SCREEN_W - v->largeur - MARGE_DROITE)
        v->x = SCREEN_W - v->largeur - MARGE_DROITE;

    // Limite haut
    if (v->y < 0)
        v->y = 0;
    // Limite bas avec marge
    if (v->y > SCREEN_H - v->hauteur - MARGE_BAS)
        v->y = SCREEN_H - v->hauteur - MARGE_BAS;
}






void deplacer_projectile(Projectile *p){
    if(p->actif){
        p->x+=5;
        if(p->x>SCREEN_W) p->actif=0;
    }
}


void deplacer_ennemi(Ennemi* e, int vitesse) {
    if (e->actif) {
        e->x -= vitesse;
        if (e->x < -LARGEUR_ENNEMI) {
            e->actif = 0;
        }
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

void init_ennemis_niveau1() {
    for (int i = 0; i < NOMBRE_ENNEMIS_NIVEAU1; i++) {
        ennemis_niveau1[i].actif = 0;
    }
}




