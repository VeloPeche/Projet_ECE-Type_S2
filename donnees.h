#ifndef DONNEES_H
#define DONNEES_H

#define MAX_PROJECTILES 10

#define MAX_ENNEMIS    20
#define LARGEUR_ENNEMI 80
#define HAUTEUR_ENNEMI 74
#define VITESSE_ENNEMI 5

#define MAX_COEURS   5
#define LARGEUR_COEUR 80
#define HAUTEUR_COEUR 74
#define VITESSE_COEUR 5

#define VIES_INITIALES 3

#define DUREE_NIVEAU   30

typedef struct { int x, y, actif; } Projectile;
typedef struct { int x, y, vitesse, largeur, hauteur, nb_vie; } Vaisseau;
typedef struct { int x, y, actif; } Ennemi;
typedef struct { int x, y, actif; } Coeur;


#endif // DONNEES_H

