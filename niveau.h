#ifndef NIVEAU_H
#define NIVEAU_H


#include <allegro.h>

// Vitesse ennemis par niveau
#define VITESSE_ENNEMI_N1 3
#define VITESSE_ENNEMI_N2 6
#define VITESSE_ENNEMI_N3 9

// Vitesse coeurs par niveau
#define VITESSE_COEUR_N1 3
#define VITESSE_COEUR_N2 6
#define VITESSE_COEUR_N3 9

int demander_niveau(BITMAP *page, BITMAP *fond_menu);

// Retourne la vitesse ennemis selon niveau
int vitesse_ennemi_selon_niveau(int niveau);

// Retourne la vitesse coeurs selon niveau
int vitesse_coeur_selon_niveau(int niveau);

void initialiser_niveau_1();
#endif

