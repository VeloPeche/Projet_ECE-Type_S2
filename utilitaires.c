#include "utilitaires.h"
#include <stdlib.h>

BITMAP *charger_bitmap_sure(const char *f){
    BITMAP *b = load_bitmap(f, NULL);
    if(!b){
        allegro_message("Erreur : %s introuvable", f);
        exit(EXIT_FAILURE);
    }
    return b;
}
