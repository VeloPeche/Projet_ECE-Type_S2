#ifndef DONNEES_H
#define DONNEES_H

#define MAX_PROJECTILES 10

#define LARGEUR_ENNEMI 80
#define HAUTEUR_ENNEMI 74
#define VITESSE_ENNEMI 5
// dans donnees.h
#define MAX_ENNEMIS 20

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

typedef struct { int x, y, actif; } Projectile;
typedef struct { int x, y, vitesse, largeur, hauteur, nb_vie; } Vaisseau;
typedef struct { int x,y,apparition,actif; } Ennemi;
typedef struct { int x, y, actif; } Coeur;
typedef struct {int x,y,actif; } Etoile_ennemie;
typedef struct { int x, y,largeur, hauteur,actif;} Drapeau;

#endif // DONNEES_H

