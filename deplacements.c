#include "deplacements.h"
#include "utilitaires.h"

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
