#ifndef AFFICHAGE_H
#define AFFICHAGE_H

#include <allegro.h>
#include "donnees.h"

void afficher_barre_de_vie(BITMAP *buf, const Vaisseau *v);
void afficher_menu_principal(BITMAP *page, const char *pseudo, int sel,
                             BITMAP *fondMenu, int meilleur_score, int meilleur_niveau);
void afficher_etoiles(BITMAP *page, Etoile_ennemie etoiles[], int nombre_etoiles);
void afficher_obstacles(BITMAP *page, BITMAP *sprite_obstacle, Etoile_ennemie obstacles[], int nombre);

void afficher_drapeau(BITMAP *page, Drapeau *drapeau, BITMAP *sprite_drapeau);
#endif // AFFICHAGE_H