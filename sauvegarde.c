#include "sauvegarde.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int lire_sauvegarde(const char *pseudo, int *out_score, int *out_level){
    FILE *f = fopen("sauvegarde.txt","r");
    if(!f) return 0;
    char nom[50]; int sc,lvl;
    while(fscanf(f,"%49s %d %d\n", nom, &sc, &lvl)==3){
        if(strcmp(nom,pseudo)==0){
            *out_score = sc;
            *out_level = lvl;
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

void ecrire_sauvegarde(const char *pseudo, int sc, int lvl){
    FILE *f = fopen("sauvegarde.txt","r");
    FILE *t = fopen("tmp.txt","w");
    char nom[50]; int s,l; int found=0;
    if(f){
        while(fscanf(f,"%49s %d %d\n", nom, &s, &l)==3){
            if(strcmp(nom,pseudo)==0){
                found=1;
                fprintf(t,"%s %d %d\n",
                        nom,
                        sc> s? sc: s,
                        lvl> l? lvl: l);
            } else {
                fprintf(t,"%s %d %d\n",nom,s,l);
            }
        }
        fclose(f);
    }
    if(!found){
        fprintf(t,"%s %d %d\n",pseudo,sc,lvl);
    }
    fclose(t);
    remove("sauvegarde.txt");
    rename("tmp.txt","sauvegarde.txt");
}