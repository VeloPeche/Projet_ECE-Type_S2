#include "deplacements.h"
#include "utilitaires.h"
#include "deplacements.h"

// … vos autres includes …
// ... vos fonctions existantes ...

#define MARGE_BAS 12
#define MARGE_DROITE 30

Drapeau drapeau_niveau1;

Ennemi ennemis_niveau1[66] ={

    {640, 10,   0, 0},
    {645, 400,   0, 0},
    {650, 200,   0, 0},
    {655, 300,  54, 0},
    {660, 300,  70, 0},
    {665, 200,  54, 0},
    {670, 370,  90, 0},
    {675, 20,  120, 0},
    {680, 100,  90, 0},
    {685, 120, 162, 0},
    {690, 310, 140, 0},
    {695, 220, 125, 0},
    {640, 20,  186, 0},
    {645, 120,  216, 0},
    {650, 345, 216, 0},
    {655, 149, 240, 0},
    {660, 230, 270, 0},
    {665, 360, 270, 0},
    {670, 245, 324, 0},
    {675, 355, 324, 0},
    {680, 10, 324, 0},
    {685, 220, 378, 0},
    {690, 20, 358, 0},
    {695, 360, 390, 0},
    {640, 30,  432, 0},
    {645, 120,  452, 0},
    {650, 340, 402, 0},
    {655, 110, 486, 0},
    {660, 180, 486, 0},
    {665, 250, 486, 0},
    {670, 100, 500, 0},
    {675, 310, 490, 0},
    {680, 230, 550, 0},
    {685, 390, 594, 0},
    {690, 30, 504, 0},
    {695, 230, 594, 0},
    {640, 40,  640, 0},
    {645, 120,  608, 0},
    {650, 200, 657, 0},
    {655, 160, 702, 0},
    {660, 300, 702, 0},
    {665, 370, 702, 0},
    {670, 280, 736, 0},
    {675, 320, 750, 0},
    {680, 360, 795, 0},
    {685, 370, 810, 0},
    {690, 260, 810, 0},
    {695, 180, 810, 0},
    {640, 15,  854, 0},
    {645, 335,  854, 0},
    {650, 95,  870, 0},
    {655, 35, 900, 0},
    {660, 175, 900, 0},
    {665, 255, 930, 0},
    {670, 255, 972, 0},
    {675, 395, 972, 0},
    {680, 35, 972, 0},
    {685, 135, 998, 0},
    {690, 215, 1026, 0},
    {695, 355, 1026, 0},
    {640, 25, 1080, 0},
    {645, 165, 1080, 0},
    {650, 305,1080, 0},
    {655, 145,1134, 0},
    {660, 285,1134, 0},
    {665, 25,1134, 0},


};


int detecter_collision_missile_projectile(Missile *m, Projectile *p){
    if (!m->actif || !p->actif) return 0;
    // On considère le missile comme un carré
    const int R = 6;              // même rayon que pour le rendu
    const int PW = 5;
    const int PH = 5;     // taille du projectile joueur
    if (p->x < m->x + R && p->x + PW > m->x - R &&p->y < m->y + R && p->y + PH > m->y - R)
    {
        m->actif=0;
        p->actif=0;
        return 1;
    }
    return 0;
}

void deplacer_missile(Missile *m){
    if(!m->actif) return;
    m->x -= VITESSE_MISSILE;
    if(m->x < 0) m->actif=0;
}

void detecter_collision_missile_vers_vaisseau(Missile *m, Vaisseau *v){
    if(!m->actif) return;
    // AABB missile 1×1 (ou taille que vous voulez) vs vaisseau
    if(v->bouclier == 0 && v->x < m->x && v->x + v->largeur > m->x &&v->y < m->y && v->y + v->hauteur > m->y )
    {
        m->actif = 0;
        v->nb_vie--;
    }
}


void deplacer_vaisseau(Vaisseau *v, int dx, int dy){
    v->x += dx;
    v->y += dy;

    // Limite gauche
    if (v->x < 0)
        v->x=0;
    // Limite droite
    if (v->x > SCREEN_W - v->largeur - MARGE_DROITE)
        v->x=SCREEN_W - v->largeur - MARGE_DROITE;

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


void deplacer_ennemi(Ennemi* e, int vitesse){
    if (e->actif) {
        e->x -= vitesse;
        if (e->x < -LARGEUR_ENNEMI){
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

void deplacer_bouclier(Bouclier *b){
    if(b->actif){
        b->x-=VITESSE_BOUCLIER;
        if(b->x< -LARGEUR_BOUCLIER) b->actif=0;
    }
}

void detecter_collision_projectile(Projectile *pr,Ennemi *en,int *score){
    if(pr->actif && en->actif &&pr->x>=en->x && pr->x<=en->x+LARGEUR_ENNEMI &&pr->y>=en->y && pr->y<=en->y+HAUTEUR_ENNEMI)
    {
        pr->actif=0; en->actif=0; *score+=100;
    }
}

int collision_vaisseau_ennemi(const Vaisseau *v,const Ennemi *e){
    return e->actif &&v->x<e->x+LARGEUR_ENNEMI && v->x+v->largeur>e->x &&v->y<e->y+HAUTEUR_ENNEMI && v->y+v->hauteur>e->y;
}

int collision_vaisseau_coeur(const Vaisseau *v,const Coeur *c){
    return c->actif &&v->x<c->x+LARGEUR_COEUR && v->x+v->largeur>c->x &&v->y<c->y+HAUTEUR_COEUR && v->y+v->hauteur>c->y;
}

int collision_vaisseau_bouclier(const Vaisseau *v,const Bouclier *b){
    return b->actif &&v->x<b->x+LARGEUR_COEUR && v->x+v->largeur>b->x &&v->y<b->y+HAUTEUR_COEUR && v->y+v->hauteur>b->y;
}

void initialiser_etoiles(Etoile_ennemie etoiles[],Etoile_ennemie obstacles[],int niveau) {
    int nombre_etoiles=0;
    int nombre_obstacles=0;

    if (niveau == 1){ //diff selon les niveaux
        nombre_etoiles=MAX_ETOILES_NIVEAU_1;
        nombre_obstacles=15;
        int positions_etoiles[15][2] ={
            {100, 100}, {200, 150}, {300, 200}, {400, 250}, {500, 300},
            {150, 350}, {250, 400}, {350, 450}, {450, 500}, {550, 550},
            {600, 100}, {650, 150}, {700, 200}, {750, 250}, {800, 300}
        };
        int positions_obstacles[15][2] ={
            {120, 120}, {220, 180}, {320, 220}, {420, 260}, {520, 310},
            {160, 360}, {260, 410}, {360, 460}, {460, 510}, {560, 560},
            {610, 120}, {660, 160}, {710, 210}, {760, 260}, {810, 310}
        };
        for (int i = 0; i < nombre_etoiles; i++){
            etoiles[i].x=positions_etoiles[i][0];
            etoiles[i].y=positions_etoiles[i][1];
            etoiles[i].actif=1;
        }
        for (int i = 0; i < nombre_obstacles; i++){
            obstacles[i].x=positions_obstacles[i][0];
            obstacles[i].y=positions_obstacles[i][1];
            obstacles[i].actif=1;
        }
    }
    else if (niveau == 2){
        nombre_etoiles=MAX_ETOILES_NIVEAU_2;
        nombre_obstacles=20;
        int positions_etoiles[22][2] ={
            {100, 100}, {200, 120}, {300, 140}, {400, 160}, {500, 180},
            {600, 200}, {150, 220}, {250, 240}, {350, 260}, {450, 280},
            {550, 300}, {650, 320}, {750, 340}, {850, 360}, {950, 380},
            {100, 400}, {200, 420}, {300, 440}, {400, 460}, {500, 480},
            {600, 500}, {700, 520}
        };
        int positions_obstacles[20][2] ={
            {120, 100}, {220, 140}, {320, 160}, {420, 180}, {520, 200},
            {620, 220}, {170, 240}, {270, 260}, {370, 280}, {470, 300},
            {570, 320}, {670, 340}, {770, 360}, {870, 380}, {970, 400},
            {120, 420}, {220, 440}, {320, 460}, {420, 480}, {520, 500}
        };
        for (int i = 0; i < nombre_etoiles; i++){
            etoiles[i].x=positions_etoiles[i][0];
            etoiles[i].y=positions_etoiles[i][1];
            etoiles[i].actif=1;
        }
        for (int i = 0; i < nombre_obstacles; i++){
            obstacles[i].x=positions_obstacles[i][0];
            obstacles[i].y=positions_obstacles[i][1];
            obstacles[i].actif=1;
        }
    }
    else if (niveau == 3){
        nombre_etoiles =MAX_ETOILES_NIVEAU_3;
        nombre_obstacles =25;
        int positions_etoiles[25][2] ={
            {100, 100}, {150, 120}, {200, 140}, {250, 160}, {300, 180},
            {350, 200}, {400, 220}, {450, 240}, {500, 260}, {550, 280},
            {600, 300}, {650, 320}, {700, 340}, {750, 360}, {800, 380},
            {850, 400}, {900, 420}, {950, 440}, {100, 460}, {150, 480},
            {200, 500}, {250, 520}, {300, 540}, {350, 560}, {400, 580}
        };
        int positions_obstacles[25][2] ={
            {110, 110}, {160, 130}, {210, 150}, {260, 170}, {310, 190},
            {360, 210}, {410, 230}, {460, 250}, {510, 270}, {560, 290},
            {610, 310}, {660, 330}, {710, 350}, {760, 370}, {810, 390},
            {860, 410}, {910, 430}, {960, 450}, {110, 470}, {160, 490},
            {210, 510}, {260, 530}, {310, 550}, {360, 570}, {410, 590}
        };
        for (int i = 0; i < nombre_etoiles; i++){
            etoiles[i].x =positions_etoiles[i][0];
            etoiles[i].y =positions_etoiles[i][1];
            etoiles[i].actif =1;
        }
        for (int i = 0; i < nombre_obstacles; i++){
            obstacles[i].x = positions_obstacles[i][0];
            obstacles[i].y =positions_obstacles[i][1];
            obstacles[i].actif = 1;
        }
    }
}


int collision_vaisseau_obstacle(Vaisseau* v,Etoile_ennemie* e){
    return e->actif &&v->x < e->x + 32 &&v->x + v->largeur > e->x &&v->y < e->y + 32 &&v->y + v->hauteur > e->y;
}

int collision_vaisseau_drapeau(const Vaisseau* v,const Drapeau* d){
    if (!d->actif) return 0;
    return (v->x < d->x + d->largeur) &&(v->x + v->largeur > d->x) &&(v->y < d->y + d->hauteur) &&(v->y + v->hauteur > d->y);
}


void init_ennemis_niveau1(){
    for (int i = 0; i < NOMBRE_ENNEMIS_NIVEAU1; i++){
        ennemis_niveau1[i].actif=0;
    }
}

void init_drapeau_niveau1() {
    drapeau_niveau1.x=50;   // Position proche du bord gauche, par ex.
    drapeau_niveau1.y=SCREEN_H / 2; // milieu de l'écran en Y
    drapeau_niveau1.largeur=40;     // taille du drapeau
    drapeau_niveau1.hauteur=60;
    drapeau_niveau1.actif=1;
}





