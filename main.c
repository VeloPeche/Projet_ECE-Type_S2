#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_PROJECTILES 10
#define MAX_ENNEMIS    20
#define LARGEUR_ENNEMI 80
#define HAUTEUR_ENNEMI 74
#define VITESSE_ENNEMI 5
#define VIES_INITIALES 3
#define DUREE_NIVEAU   30

typedef struct { int x,y,actif; } Projectile;
typedef struct { int x,y,vitesse,largeur,hauteur,nb_vie; } Vaisseau;
typedef struct { int x,y,actif; } Ennemi;

// Prototype sauvegarde
int lire_sauvegarde(const char *pseudo, int *out_score, int *out_level);
void ecrire_sauvegarde(const char *pseudo, int sc, int lvl);

// Chargement sécurisé
BITMAP *load_bitmap_check(const char *f){
    BITMAP *b = load_bitmap(f,NULL);
    if(!b){ allegro_message("Erreur: %s non trouvé",f); exit(EXIT_FAILURE); }
    return b;
}

// Déplacements et collisions
void deplacement_vaisseau(Vaisseau *v,int dx,int dy){
    v->x+=dx; v->y+=dy;
    if(v->x<0) v->x=0;
    if(v->x>SCREEN_W-v->largeur) v->x=SCREEN_W-v->largeur;
    if(v->y<0) v->y=0;
    if(v->y>SCREEN_H-v->hauteur) v->y=SCREEN_H-v->hauteur;
}
void deplacement_projectile(Projectile *p){
    if(p->actif){ p->x+=5; if(p->x>SCREEN_W) p->actif=0; }
}
void deplacement_ennemi(Ennemi *e){
    if(e->actif){ e->x-=VITESSE_ENNEMI; if(e->x< -LARGEUR_ENNEMI) e->actif=0; }
}
void detecter_collisions(Projectile *pr,Ennemi *en,int *score){
    if(pr->actif && en->actif &&
       pr->x>=en->x && pr->x<=en->x+LARGEUR_ENNEMI &&
       pr->y>=en->y && pr->y<=en->y+HAUTEUR_ENNEMI) {
        pr->actif=0; en->actif=0; *score+=100;
    }
}
int collision_vaisseau_ennemi(Vaisseau *v,Ennemi *e){
    return e->actif &&
           v->x<e->x+LARGEUR_ENNEMI && v->x+v->largeur>e->x &&
           v->y<e->y+HAUTEUR_ENNEMI && v->y+v->hauteur>e->y;
}

// Barre de vie
void afficher_barre_vie(BITMAP *buf,Vaisseau *v){
    int max=VIES_INITIALES,w=100,h=10,x=10,y=50;
    int cur=v->nb_vie*w/max;

    int couleur_vie;
    if (v->nb_vie > 2) {
        couleur_vie = makecol(0, 255, 0); // vert
    } else if (v->nb_vie == 2) {
        couleur_vie = makecol(255, 255, 0); // jaune
    } else {
        couleur_vie = makecol(255, 0, 0); // rouge
    }

    rect(buf,x-1,y-1,x+w+1,y+h+1,makecol(255,255,255));
    rectfill(buf,x,y,x+w,y+h,makecol(100,0,0));
    rectfill(buf,x,y,x+cur,y+h,couleur_vie);

}

// Affichage du menu et stats
void afficher_menu(BITMAP *page,const char *pseudo,int sel,
                   BITMAP *fm,int best_score,int best_level)
{
    clear_bitmap(page);
    draw_sprite(page,fm,0,0);

    if(best_score>=0){
        char s[80];
        sprintf(s,"Meilleur Score: %d       Meilleur Niveau: %d",
                best_score,best_level);

        textout_ex(page,font,s,10,455,makecol(255,255,0),-1);
    }

    textout_centre_ex(page,font,"-----ECE-RTYPE-----",
                      SCREEN_W/2,10,makecol(255,255,255),-1);
    char buf[64];
    sprintf(buf,"Bienvenue, %s !",pseudo);
    textout_centre_ex(page,font,buf,SCREEN_W/2,80,makecol(255,255,255),-1);

    const char *opts[3]={"1. Commencer","2. Reprendre","3. Quitter"};
    for(int i=0;i<3;i++){
        int col=(i+1==sel)?makecol(0,255,0):makecol(255,255,255);
        textout_centre_ex(page,font,opts[i],
                          SCREEN_W/2,SCREEN_H/2 + (i*30) - 30,
                          col,-1);
    }
    blit(page,screen,0,0,0,0,SCREEN_W,SCREEN_H);
}

// Lecture/écriture de la sauvegarde
int lire_sauvegarde(const char *pseudo, int *out_score, int *out_level){
    FILE *f = fopen("sauvegarde.txt","r");
    if(!f) return 0;
    char nom[50]; int sc,lvl;
    while(fscanf(f,"%49s %d %d\n",nom,&sc,&lvl)==3){
        if(strcmp(nom,pseudo)==0){
            *out_score=sc; *out_level=lvl;
            fclose(f); return 1;
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
        while(fscanf(f,"%49s %d %d\n",nom,&s,&l)==3){
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

int main(){
    srand(time(NULL));
    // Init Allegro
    allegro_init();
    install_keyboard(); install_mouse();
    set_color_depth(32);
    if(set_gfx_mode(GFX_AUTODETECT_WINDOWED,640,480,0,0)){
        allegro_message("Erreur mode graphique");
        exit(EXIT_FAILURE);
    }

    // Chargement images
    BITMAP *page         = create_bitmap(SCREEN_W,SCREEN_H);
    BITMAP *fond_menu    = load_bitmap_check("menu5.bmp");
    BITMAP *fond         = load_bitmap_check("Fond1600x600.bmp");
    BITMAP *vaisseau_img = load_bitmap_check("Vaisseau.bmp");
    BITMAP *ennemi_img   = load_bitmap_check("Fantome_rose.bmp");

    // 1) Saisie du pseudo
    char pseudo[50] = "";
    while(1){
        clear_bitmap(page);
        draw_sprite(page,fond_menu,0,0);
        textout_centre_ex(page,font,
            "Entrez votre pseudo:",SCREEN_W/2,100,
            makecol(255,255,255),-1);
        textout_ex(page,font,pseudo,
            SCREEN_W/2-50,130,makecol(0,255,0),-1);
        blit(page,screen,0,0,0,0,SCREEN_W,SCREEN_H);
        if(keypressed()){
            int k=readkey(); char c=k&0xFF;
            if(c==8 && strlen(pseudo)>0) pseudo[strlen(pseudo)-1]=0;
            else if(c==13 && strlen(pseudo)>0) break;
            else if(isprint(c) && strlen(pseudo)<49)
                strncat(pseudo,&c,1);
        }
    }

    // 2) Chargement sauvegarde
    int best_score=-1, best_level=-1;
    lire_sauvegarde(pseudo,&best_score,&best_level);

    // 3) Boucle MENU → JEU → MENU ...
    while(1){
        // --- MENU ---
        int selection=1;
        while(1){
            afficher_menu(page,pseudo,selection,
                          fond_menu,best_score,best_level);

            if(key[KEY_UP])   { if(selection>1) selection--; rest(150); }
            if(key[KEY_DOWN]) { if(selection<3) selection++; rest(150); }
            if(key[KEY_ENTER]){
                if(selection==3){
                    ecrire_sauvegarde(pseudo,best_score,best_level);
                    allegro_exit(); return 0;
                }
                break;
            }
            if(mouse_b&1){
                int mx=mouse_x, my=mouse_y;
                if(mx>SCREEN_W/2-100 && mx<SCREEN_W/2+100 &&
                   my>SCREEN_H/2-30 && my<SCREEN_H/2+60)
                {
                    selection = ((my-(SCREEN_H/2-30))/30) + 1;
                    if(selection==3){
                        ecrire_sauvegarde(pseudo,best_score,best_level);
                        allegro_exit(); return 0;
                    }
                    break;
                }
            }
        }

        // --- INITIALISATION JEU ---
        Vaisseau v = {50,240,5,32,32,VIES_INITIALES};
        Projectile projectiles[MAX_PROJECTILES] = {{0}};
        Ennemi    ennemis[MAX_ENNEMIS]         = {{0}};
        int vies = VIES_INITIALES, score = 0, level = 1;
        int screenx = 0;
        clock_t last_tir = 0; const int tir_delay=500;
        time_t debut = time(NULL);

        // --- BOUCLE DE JEU ---
        while(!key[KEY_ESC] && v.nb_vie>0
              && difftime(time(NULL),debut)<DUREE_NIVEAU)
        {
            // scrolling
            screenx+=2;
            if(screenx>fond->w-SCREEN_W) screenx=0;
            clear_bitmap(page);
            blit(fond,page,screenx,0,0,0,SCREEN_W,SCREEN_H);

            // vaisseau
            int dx=0, dy=0;
            if(key[KEY_UP])    dy=-v.vitesse;
            if(key[KEY_DOWN])  dy= v.vitesse;
            if(key[KEY_LEFT])  dx=-v.vitesse;
            if(key[KEY_RIGHT]) dx= v.vitesse;
            deplacement_vaisseau(&v,dx,dy);

            // tir joueur
            if(key[KEY_SPACE] && clock()-last_tir>=tir_delay){
                for(int i=0;i<MAX_PROJECTILES;i++){
                    if(!projectiles[i].actif){
                        projectiles[i].x     = v.x + v.largeur+25;
                        projectiles[i].y     = v.y + v.hauteur+1/2;
                        projectiles[i].actif = 1;
                        last_tir = clock();
                        break;
                    }
                }
            }

            // maj projectiles
            for(int i=0;i<MAX_PROJECTILES;i++)
                deplacement_projectile(&projectiles[i]);

            // spawn ennemis
            if(rand()%30==0){
                for(int i=0;i<MAX_ENNEMIS;i++){
                    if(!ennemis[i].actif){
                        ennemis[i].actif=1;
                        ennemis[i].x   = SCREEN_W;
                        ennemis[i].y   = rand()%(SCREEN_H-HAUTEUR_ENNEMI);
                        break;
                    }
                }
            }

            // maj ennemis & collisions
            for(int i=0;i<MAX_ENNEMIS;i++){
                deplacement_ennemi(&ennemis[i]);
                if(collision_vaisseau_ennemi(&v,&ennemis[i])){
                    v.nb_vie--; ennemis[i].actif=0;
                }
                for(int j=0;j<MAX_PROJECTILES;j++){
                    detecter_collisions(&projectiles[j],&ennemis[i],&score);
                }
            }

            // affichage
            draw_sprite(page,vaisseau_img,v.x,v.y);
            for(int i=0;i<MAX_PROJECTILES;i++){
                if(projectiles[i].actif)
                    rectfill(page,
                        projectiles[i].x, projectiles[i].y,
                        projectiles[i].x+5,projectiles[i].y+5,
                        makecol(255,0,0)
                    );
            }
            for(int i=0;i<MAX_ENNEMIS;i++){
                if(ennemis[i].actif)
                    draw_sprite(page,ennemi_img,
                                ennemis[i].x,ennemis[i].y);
            }

            // HUD
            textprintf_ex(page,font,10,10, makecol(255,255,255),-1,
                          "Score: %d",score);
            textprintf_ex(page,font,10,30, makecol(255,255,255),-1,
                          "Vies : %d",v.nb_vie);
            int rem = DUREE_NIVEAU - (int)difftime(time(NULL),debut);
            if(rem<0) rem=0;
            textprintf_ex(page,font,SCREEN_W-100,10,
                          makecol(255,255,0),-1,
                          "Temps: %ds",rem);
            afficher_barre_vie(page,&v);

            blit(page,screen,0,0,0,0,SCREEN_W,SCREEN_H);
            rest(30);
        }

        // fin partie → maj sauvegarde
        if(score>best_score)    best_score=score;
        if(level>best_level)    best_level=level;
        ecrire_sauvegarde(pseudo,best_score,best_level);

        rest(300);
    }

    return 0;
}
END_OF_MAIN();
