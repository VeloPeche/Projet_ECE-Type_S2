#ifndef SAUVEGARDE_H
#define SAUVEGARDE_H

int lire_sauvegarde(const char *pseudo, int *out_score, int *out_level);
void ecrire_sauvegarde(const char *pseudo, int sc, int lvl);

#endif // SAUVEGARDE_H
