#ifndef TYPES_H
#define TYPES_H

#define MAX_PROJECTILES   10
#define MAX_ENNEMIS       20
#define MAX_MISSILES      10
#define LARGEUR_ENNEMI    80
#define HAUTEUR_ENNEMI    74
#define VITESSE_ENNEMI    5
#define VITESSE_MISSILE   (VITESSE_ENNEMI + 2)
#define VIES_INITIALES    3
#define DUREE_NIVEAU      30

typedef struct { int x, y, actif; } Projectile;
typedef struct { int x, y, vitesse, largeur, hauteur, nb_vie; } Vaisseau;
typedef struct { int x, y, actif; } Ennemi;
typedef struct { int x, y, actif; } Missile;

#endif // TYPES_H
