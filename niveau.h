#ifndef NIVEAU_H
#define NIVEAU_H


#include <allegro.h>

// Constantes niveaux possibles
#define NIVEAU_MIN 1
#define NIVEAU_MAX 3

// Durée du niveau en secondes (exemple)
#define DUREE_NIVEAU 30

// Prototypes
int demander_niveau(BITMAP *page, BITMAP *fond_menu);

#endif

