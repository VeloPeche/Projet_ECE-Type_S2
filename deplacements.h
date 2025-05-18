#ifndef DEPLACEMENTS_H
#define DEPLACEMENTS_H

#include "donnees.h"
#define MAX_MISSILES    10
// vitesse des missiles (un peu plus rapide que les ennemis)
#define VITESSE_MISSILE (VITESSE_ENNEMI + 2)
extern Ennemi ennemis_niveau1[NOMBRE_ENNEMIS_NIVEAU1];

void init_ennemis_niveau1(void);
void mise_a_jour_ennemis_niveau1(int tick_actuel, int vitesse);
typedef struct { int x,y,actif; } Missile;

void deplacer_missile(Missile *m);
void detecter_collision_missile_vers_vaisseau(Missile *m, Vaisseau *v);
void deplacer_vaisseau(Vaisseau *v, int dx, int dy);
void deplacer_projectile(Projectile *p);
void deplacer_ennemi(Ennemi* e, int vitesse);
void deplacer_coeur(Coeur *c);
void deplacer_blouclier(Bouclier *b);
void detecter_collision_projectile(Projectile *pr, Ennemi *en, int *score);
int  collision_vaisseau_ennemi(const Vaisseau *v, const Ennemi *e);
int collision_vaisseau_coeur(const Vaisseau *v, const Coeur *c);
int collision_vaisseau_bouclier(const Vaisseau *v, const Bouclier *b);
void initialiser_etoiles(Etoile_ennemie etoiles[], Etoile_ennemie obstacles[], int niveau);
int collision_vaisseau_obstacle(Vaisseau* v, Etoile_ennemie* e);
// Retourne 1 si collision, 0 sinon. Désactive les deux si collision.
int detecter_collision_missile_projectile(Missile *m, Projectile *p);
int collision_vaisseau_drapeau(const Vaisseau* v, const Drapeau* d);
void init_drapeau_niveau1();
#endif // DEPLACEMENTS_H
