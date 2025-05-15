#ifndef AFFICHAGE_H
#define AFFICHAGE_H

#include <allegro.h>
#include "donnees.h"

void afficher_barre_de_vie(BITMAP *buf, const Vaisseau *v);
void afficher_menu_principal(BITMAP *page, const char *pseudo, int sel,
                             BITMAP *fondMenu, int meilleur_score, int meilleur_niveau);

#endif // AFFICHAGE_H
