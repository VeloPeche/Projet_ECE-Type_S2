#ifndef DONNEES_H
#define DONNEES_H

#define MAX_PROJECTILES 10

#define LARGEUR_ENNEMI 80
#define HAUTEUR_ENNEMI 74
#define VITESSE_ENNEMI 5
// dans donnees.h
#define MAX_ENNEMIS 100

#define MAX_COEURS   5
#define LARGEUR_COEUR 80
#define HAUTEUR_COEUR 74
#define VITESSE_COEUR 5

#define VIES_INITIALES 3

#define DUREE_NIVEAU   60
#define NOMBRE_ENNEMIS_NIVEAU1 66


#define MAX_ETOILES_NIVEAU_1 15
#define MAX_ETOILES_NIVEAU_2 22
#define MAX_ETOILES_NIVEAU_3 25

#define MAX_BOUCLIERS 5
#define DUREE_BOUCLIER 10
#define LARGEUR_BOUCLIER 80
#define HAUTEUR_BOUCLIER 74
#define VITESSE_BOUCLIER 5
#include <time.h>

typedef struct { int x, y, actif; } Projectile;
typedef struct {
    int x, y;
    int vitesse;
    int largeur, hauteur;
    int nb_vie;
    int bouclier;
    time_t bouclierdebut;
} Vaisseau;
typedef struct { int x,y,apparition,actif; } Ennemi;
typedef struct { int x, y, actif; } Coeur;
typedef struct {int x,y,actif; } Etoile_ennemie;
typedef struct { int x, y,largeur, hauteur,actif;} Drapeau;
typedef struct { int x, y,actif;} Bouclier;

#endif // DONNEES_H

