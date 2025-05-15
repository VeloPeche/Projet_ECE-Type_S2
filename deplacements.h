#ifndef DEPLACEMENTS_H
#define DEPLACEMENTS_H

#include "donnees.h"

void deplacer_vaisseau(Vaisseau *v, int dx, int dy);
void deplacer_projectile(Projectile *p);
void deplacer_ennemi(Ennemi *e);
void detecter_collision_projectile(Projectile *pr, Ennemi *en, int *score);
int  collision_vaisseau_ennemi(const Vaisseau *v, const Ennemi *e);

#endif // DEPLACEMENTS_H
