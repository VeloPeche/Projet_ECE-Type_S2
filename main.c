#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MAX_NAME_LENGTH 50
#define LEVEL_TIME_LIMIT 30
#define NUM_ENEMIES 15
#define INVINCIBILITY_TIME 30

typedef struct {
    float x;
    float y;
    float vitesse;
    int nb_vie;
    int score_joueur;
} Vaisseau_joueur;

typedef struct {
    float x;
    float y;
    float vitesse;
    int vivant;
    int temps_apparition;
    int deja_apparu;
} Ennemi;

typedef struct {
    float x;
    float y;
    float vitesse;
    int actif;
} Projectile;

typedef struct {
    int vitesseEnnemi;
    int delaiApparition;
    int maxEnnemisActifs;
    float vitesseTirsEnnemis;
} Niveau;


Niveau obtenirParametresNiveau(int niveau) {
    Niveau n;
    switch (niveau) {
        case 1:
            n.vitesseEnnemi = 2;
        n.delaiApparition = 200;
        n.maxEnnemisActifs = 5;
        n.vitesseTirsEnnemis = 4.0f;
        break;
        case 2:
            n.vitesseEnnemi = 4; // plus rapide
        n.delaiApparition = 100; // apparaissent plus vite
        n.maxEnnemisActifs = 10; // plus d’ennemis en même temps
        n.vitesseTirsEnnemis = 6.0f; // tirs plus rapides
        break;
        default:
            n.vitesseEnnemi = 2;
        n.delaiApparition = 200;
        n.maxEnnemisActifs = 5;
        n.vitesseTirsEnnemis = 4.0f;
    }
    return n;
}

float positions_x[NUM_ENEMIES]={
    SCREEN_WIDTH + 50,SCREEN_WIDTH + 200,SCREEN_WIDTH + 350,
    SCREEN_WIDTH + 100,SCREEN_WIDTH + 250,SCREEN_WIDTH + 400,
    SCREEN_WIDTH + 150,SCREEN_WIDTH + 300,SCREEN_WIDTH + 450,
    SCREEN_WIDTH + 200,SCREEN_WIDTH + 350,SCREEN_WIDTH + 500,
    SCREEN_WIDTH + 150,SCREEN_WIDTH + 300,SCREEN_WIDTH + 450
};

float positions_y[NUM_ENEMIES]={
    100,200,300,150,250,350,
    100,200,300,150,250,350,
    100,200,300
};

int temps_apparition[NUM_ENEMIES]={
    3,5,5,6,8,8,
    12,15,16,18,20,20,
    22,23,24
};

void deplacement_vaisseau(Vaisseau_joueur* vaisseau,int direction_x,int direction_y){
    vaisseau->x+= direction_x * vaisseau->vitesse;
    vaisseau->y+= direction_y * vaisseau->vitesse;
    if (vaisseau->x < 0)vaisseau->x=0;
    if (vaisseau->x >SCREEN_WIDTH-32)vaisseau->x=SCREEN_WIDTH-32;
    if (vaisseau->y < 0)vaisseau->y=0;
    if (vaisseau->y >SCREEN_HEIGHT-32)vaisseau->y=SCREEN_HEIGHT-32;
}

void deplacement_projectile(Projectile* p){
    if (p->actif) {
        p->x+=p->vitesse;
        if (p->x > SCREEN_WIDTH){
            p->actif=0;
        }
    }
}

void deplacement_ennemi(Ennemi* e){
    if (e->vivant){
        e->x-= e->vitesse;
        if (e->x < -32){
            e->vivant=0;
        }
    }
}

void afficher_vaisseau(Vaisseau_joueur* v,BITMAP* buffer,BITMAP* img){
    masked_blit(img,buffer,0,0,(int)v->x,(int)v->y,img->w,img->h);
}

void afficher_ennemi(Ennemi* ennemi1,BITMAP* buffer,BITMAP* ennemi_img) {
    if (ennemi1->vivant) {
        masked_blit(ennemi_img,buffer,0,0,(int)ennemi1->x,(int)ennemi1->y,ennemi_img->w,ennemi_img->h);
    }
}

void afficher_projectile(Projectile* projectile1,BITMAP* buffer){
    if (projectile1->actif){
        rectfill(buffer,projectile1->x,projectile1->y,projectile1->x + 5,projectile1->y + 5,makecol(255,255,0));
    }
}

void verrifier_collisions(Vaisseau_joueur* vaisseau,Ennemi* ennemi1,int nb,Projectile* projectile1,int* invincible,int* invincible_timer){
    // Collision projectile-ennemi
    if (projectile1->actif){
        for (int i = 0; i < nb; i++){
            if (ennemi1[i].vivant && projectile1->x + 5 > ennemi1[i].x && projectile1->x < ennemi1[i].x + 32 && projectile1->y + 5 > ennemi1[i].y && projectile1->y < ennemi1[i].y + 32){
                ennemi1[i].vivant=0;
                projectile1->actif=0;
                vaisseau->score_joueur+= 50;
                break;
            }
        }
    }

    // Collision vaisseau-ennemi
    if (!(*invincible)){
        for (int i = 0; i < nb; i++){
            if (ennemi1[i].vivant &&
                vaisseau->x + 32 > ennemi1[i].x && vaisseau->x < ennemi1[i].x + 32 && vaisseau->y + 32 > ennemi1[i].y && vaisseau->y < ennemi1[i].y + 32) {
                vaisseau->nb_vie--;
                *invincible=1;
                *invincible_timer=INVINCIBILITY_TIME;
                break;
            }
        }
    }
}

void reinit_game(Vaisseau_joueur* vaisseau,Ennemi* e,int nb,Projectile* p){
    vaisseau->x=400;
    vaisseau->y=300;
    vaisseau->nb_vie=3;
    vaisseau->score_joueur=0;

    for (int i = 0; i < nb; i++) {
        e[i].vivant = 0;
        e[i].deja_apparu = 0;
        e[i].x = positions_x[i];
        e[i].y = positions_y[i];
        e[i].vitesse = 3.0f + (i * 0.15f);  // Augmentation de la vitesse des ennemis dans le niveau 2
        e[i].temps_apparition = temps_apparition[i] * 60;
    }

    p->actif = 0;
}

void update_ennemis_apparition(Ennemi* e,int nb,int temps_jeu) {
    for (int i = 0; i < nb; i++) {
        if (!e[i].vivant && !e[i].deja_apparu && temps_jeu >= e[i].temps_apparition) {
            e[i].vivant = 1;
            e[i].deja_apparu = 1;
        }
    }
}


void menu(BITMAP* buffer,BITMAP* fond,const char* pseudo,int sel) {
    stretch_blit(fond, buffer,
        0,0,fond->w,fond->h,
        0,0,SCREEN_WIDTH,SCREEN_HEIGHT);

    textout_centre_ex(buffer, font, "----ECE-type----", SCREEN_WIDTH / 2, 80, makecol(255, 255, 0), -1);
    textprintf_ex(buffer,font,280,130,makecol(255, 255, 255), -1,"Bienvenue %s !", pseudo);

    const char* options[] ={"Nouvelle Partie","Reprendre une partie","Quitter le jeu"};
    int option_height = 200;
    for (int i = 0; i < 3; i++){
        int color = (i == sel) ? makecol(0, 255, 0) : makecol(255, 255, 255);
        textout_centre_ex(buffer, font, options[i], SCREEN_WIDTH / 2, option_height + i * 40, color, -1);
    }

    blit(buffer,screen,0,0,0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
}

void afficher_barre_vie(BITMAP* buffer, Vaisseau_joueur* vaisseau) {
    int max_vie = 3;
    int largeur_max = 100;
    int hauteur = 10;
    int x = 10;
    int y = 50;

    int largeur_vie = (vaisseau->nb_vie * largeur_max) / max_vie;

    // Cadre
    rect(buffer, x - 1, y - 1, x + largeur_max + 1, y + hauteur + 1, makecol(255, 255, 255));
    // Fond rouge
    rectfill(buffer, x, y, x + largeur_max, y + hauteur, makecol(100, 0, 0));
    // Vie verte
    rectfill(buffer, x, y, x + largeur_vie, y + hauteur, makecol(0, 255, 0));
}


int jouer_niveau(Vaisseau_joueur* vaisseau, Ennemi* ennemis, Projectile* projectile, BITMAP* buffer, BITMAP* fond, BITMAP* vaisseau_image, BITMAP* ennemi_image, int positions_x[], int positions_y[], int temps_apparition[], int num_ennemis) {
    int game_time = 0;
    int game_over = 0;
    int dernier_tir = 0;
    int invincible = 0;
    int invincible_timer = 0;

    reinit_game(vaisseau, ennemis, num_ennemis, projectile);

    clear_to_color(buffer, makecol(0,0,0));
    textout_centre_ex(buffer, font, "Nouveau niveau !", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, makecol(255,255,255), -1);
    blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    rest(2000);


    while (!key[KEY_ESC] && !game_over) {
        int temps_actuel = clock();

        if (game_time >= LEVEL_TIME_LIMIT * 60) {
            break;  // Niveau réussi
        }

        update_ennemis_apparition(ennemis, num_ennemis, game_time);
        clear_to_color(buffer, makecol(0,0,0));
        draw_sprite(buffer, fond, 0, 0);

        int dx = 0, dy = 0;
        if (key[KEY_UP]) dy = -1;
        if (key[KEY_DOWN]) dy = 1;
        if (key[KEY_LEFT]) dx = -1;
        if (key[KEY_RIGHT]) dx = 1;
        deplacement_vaisseau(vaisseau, dx, dy);

        if (key[KEY_SPACE] && (temps_actuel - dernier_tir >= 500)) {
            if (!projectile->actif) {
                projectile->x = vaisseau->x + 32;
                projectile->y = vaisseau->y + 16;
                projectile->actif = 1;
                dernier_tir = temps_actuel;
            }
        }

        deplacement_projectile(projectile);

        for (int i = 0; i < num_ennemis; i++) {
            if (ennemis[i].vivant) {
                deplacement_ennemi(&ennemis[i]);
            }
        }

        if (invincible) {
            invincible_timer--;
            if (invincible_timer <= 0) {
                invincible = 0;
            }
        }

        if (!invincible || (game_time % 6 < 3)) {
            afficher_vaisseau(vaisseau, buffer, vaisseau_image);
        }

        for (int i = 0; i < num_ennemis; i++) {
            if (ennemis[i].vivant) {
                afficher_ennemi(&ennemis[i], buffer, ennemi_image);
            }
        }

        afficher_projectile(projectile, buffer);
        verrifier_collisions(vaisseau, ennemis, num_ennemis, projectile, &invincible, &invincible_timer);

        textprintf_ex(buffer, font, 10, 10, makecol(255,255,255), -1, "Score: %d", vaisseau->score_joueur);
        textprintf_ex(buffer, font, 10, 30, makecol(255,255,255), -1, "Temps restant: %d", LEVEL_TIME_LIMIT - game_time / 60);
        textprintf_ex(buffer, font, SCREEN_WIDTH - 100, 10, makecol(255,255,255), -1, "Vies: %d", vaisseau->nb_vie);
        afficher_barre_vie(buffer, vaisseau);


        if (vaisseau->nb_vie <= 0) {
            game_over = 1;
        }

        blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        rest(1000 / 60);
        game_time++;
    }

    return (vaisseau->nb_vie > 0) ? 1 : 0;
}

int main(){
    srand(time(NULL));
    allegro_init();
    install_keyboard();
    install_mouse();
    set_color_depth(32);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
    show_mouse(screen);

    BITMAP* buffer= create_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT);
    BITMAP* fond= load_bitmap("Fond2.bmp", NULL);
    BITMAP* vaisseau_image= load_bitmap("Pacman4.bmp", NULL);
    BITMAP* fond_menu= load_bitmap("menu5.bmp", NULL);
    BITMAP* ennemi_image= load_bitmap("Fantome_rose.bmp", NULL);

    float positions_x_lvl2[25] = {
        SCREEN_WIDTH + 60, SCREEN_WIDTH + 180, SCREEN_WIDTH + 300,
        SCREEN_WIDTH + 120, SCREEN_WIDTH + 240, SCREEN_WIDTH + 360,
        SCREEN_WIDTH + 150, SCREEN_WIDTH + 270, SCREEN_WIDTH + 390,
        SCREEN_WIDTH + 90,  SCREEN_WIDTH + 210, SCREEN_WIDTH + 330,
        SCREEN_WIDTH + 170, SCREEN_WIDTH + 290, SCREEN_WIDTH + 410,
        SCREEN_WIDTH + 200, SCREEN_WIDTH + 320, SCREEN_WIDTH + 440,
        SCREEN_WIDTH + 220, SCREEN_WIDTH + 340, SCREEN_WIDTH + 460,
        SCREEN_WIDTH + 250, SCREEN_WIDTH + 370, SCREEN_WIDTH + 490,
        SCREEN_WIDTH + 280
    };

    float positions_y_lvl2[25] = {
        60, 110, 160, 210, 260, 310,
        360, 410, 460, 100, 150, 200,
        250, 300, 350, 120, 170, 220,
        270, 320, 370, 140, 190, 240,
        290
    };

    int temps_apparition_lvl2[25] = {
        1, 2, 4, 5, 6,
        7, 8, 9, 10, 11, 12,
        13, 14, 16, 17, 18,
        19, 20, 21, 22, 23, 24,
        25, 27,28
    };

    if (!buffer || !fond || !vaisseau_image || !fond_menu || !ennemi_image){
        allegro_message("Erreur de chargement des images !");
        return -1;
    }

    char username[50]= "";
    clear_to_color(buffer,makecol(0,0,0));
    textout_ex(buffer, font,"Veuillez entrer votre pseudo:", 300, 100, makecol(255, 255, 255), -1);
    textout_ex(buffer, font,"Nom:",300,150,makecol(255,255,255),-1);
    blit(buffer,screen,0,0,0,0,SCREEN_WIDTH,SCREEN_HEIGHT);

    while (!key[KEY_ENTER] && !key[KEY_ESC]){
        if (keypressed()){
            char ch = readkey() & 0xFF;
            if (ch == 8 && strlen(username) > 0)
                username[strlen(username) - 1] = '\0';
            else if (ch >= 32 && ch <= 126 && strlen(username) < 49) {
                username[strlen(username)] = ch;
                username[strlen(username) + 1] = '\0';
            }
        }
        rectfill(buffer, 450, 150,650, 170,makecol(0, 0, 0));
        textout_ex(buffer, font, username,450,150, makecol(255, 255, 255), -1);
        blit(buffer, screen,0, 0, 0,0, SCREEN_WIDTH, SCREEN_HEIGHT);
        rest(10);
    }

    while (key[KEY_ENTER]) rest(10);

    Vaisseau_joueur vaisseau ={400, 300, 5.0f, 3, 0};
    Ennemi ennemis[NUM_ENEMIES];
    Projectile projectile ={0, 0, 10.0f, 0};
    reinit_game(&vaisseau,ennemis,NUM_ENEMIES,&projectile);

    int selection =0;
    int max_selection =2;
    int confirm_selection =0;

    while (!key[KEY_ESC] && !confirm_selection){
        menu(buffer,fond_menu,username,selection);

        if (key[KEY_DOWN]){
            selection=(selection + 1)%(max_selection + 1);
            rest(150);
        }
        if (key[KEY_UP]){
            selection=(selection - 1 +(max_selection + 1))%(max_selection + 1);
            rest(150);
        }

        if (mouse_x >= 300 && mouse_x <= 500){
            if (mouse_y >= 200 && mouse_y <= 240) selection=0;
            else if (mouse_y >= 240 && mouse_y <= 280) selection=1;
            else if (mouse_y >= 280 && mouse_y <= 320) selection=2;
        }

        if (mouse_b & 1) {
            confirm_selection = 1;
        }

        rest(50);
    }

    if (selection == 0){
        reinit_game(&vaisseau,ennemis,NUM_ENEMIES,&projectile);
    } else if (selection == 1){
        // Charger une sauvegarde (non implémenté)
    } else{
        destroy_bitmap(buffer);
        destroy_bitmap(fond);
        destroy_bitmap(vaisseau_image);
        destroy_bitmap(fond_menu);
        destroy_bitmap(ennemi_image);
        return 0;
    }

    clear_to_color(buffer,makecol(0,0,0));
    textout_centre_ex(buffer,font,"Niveau 1",SCREEN_WIDTH / 2,SCREEN_HEIGHT / 2,makecol(255,255,255),-1);
    blit(buffer,screen,0,0,0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
    rest(2000);

    int game_time=0;
    int game_over=0;
    int dernier_tir=0;
    int invincible=0;
    int invincible_timer=0;

    while (!key[KEY_ESC] && !game_over){
        int temps_actuel=clock();

        if (game_time >= LEVEL_TIME_LIMIT * 60){
            game_over=1;
        }

        update_ennemis_apparition(ennemis,NUM_ENEMIES,game_time);

        clear_to_color(buffer,makecol(0,0,0));
        draw_sprite(buffer,fond,0,0);

        int directionX=0,directionY=0;
        if (key[KEY_UP]) directionY=-1;
        if (key[KEY_DOWN]) directionY=1;
        if (key[KEY_LEFT]) directionX=-1;
        if (key[KEY_RIGHT]) directionX=1;

        deplacement_vaisseau(&vaisseau, directionX, directionY);

        if (key[KEY_SPACE] && (temps_actuel - dernier_tir>= 500)){
            if (!projectile.actif){
                projectile.x=vaisseau.x + 32;
                projectile.y=vaisseau.y + 16;
                projectile.actif=1;
                dernier_tir=temps_actuel;
            }
        }

        deplacement_projectile(&projectile);

        for (int i = 0; i < NUM_ENEMIES; i++){
            if (ennemis[i].vivant){
                deplacement_ennemi(&ennemis[i]);
            }
        }

        if (invincible){
            invincible_timer--;
            if (invincible_timer<= 0){
                invincible=0;
            }
        }

        if (!invincible || (game_time % 6 < 3)){
            afficher_vaisseau(&vaisseau,buffer,vaisseau_image);
        }

        for (int i = 0; i < NUM_ENEMIES; i++){
            if (ennemis[i].vivant){
                afficher_ennemi(&ennemis[i],buffer,ennemi_image);
            }
        }
        afficher_projectile(&projectile,buffer); //affichage des projectiles

        verrifier_collisions(&vaisseau,ennemis,NUM_ENEMIES,&projectile,&invincible,&invincible_timer);

        textprintf_ex(buffer, font, 10, 10, makecol(255, 255, 255), -1, "Score: %d", vaisseau.score_joueur);
        textprintf_ex(buffer, font, 10, 30, makecol(255, 255, 255), -1, "Temps restant: %d", LEVEL_TIME_LIMIT - game_time / 60);
        textprintf_ex(buffer, font, SCREEN_WIDTH - 100, 10, makecol(255, 255, 255), -1, "Vies: %d", vaisseau.nb_vie);

        if (vaisseau.nb_vie <= 0) {
            game_over = 1;
        }

        blit(buffer,screen,0,0,0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
        rest(1000 / 60);
        game_time++;
    }

    if (jouer_niveau(&vaisseau, ennemis, &projectile, buffer, fond, vaisseau_image, ennemi_image, positions_x, positions_y, temps_apparition, NUM_ENEMIES)) {
        // Niveau 1 réussi
        textout_centre_ex(buffer, font, "Niveau 1 réussi !", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 20, makecol(0,255,0), -1);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        rest(2000);

        // Préparation du niveau 2
        Ennemi ennemis_niv2[NUM_ENEMIES];
        if (jouer_niveau(&vaisseau, ennemis_niv2, &projectile, buffer, fond, vaisseau_image, ennemi_image, positions_x_lvl2, positions_y_lvl2, temps_apparition_lvl2, NUM_ENEMIES)) {
            textout_centre_ex(buffer, font, "Niveau 2 réussi !", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 20, makecol(0,255,0), -1);
            textprintf_centre_ex(buffer, font, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 40, makecol(255,255,255), -1, "Score final : %d", vaisseau.score_joueur);
            blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            rest(3000);
        } else {
            textout_centre_ex(buffer, font, "Game Over au niveau 2 !", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, makecol(255,0,0), -1);
            rest(3000);
        }
    } else {
        textout_centre_ex(buffer, font, "Game Over au niveau 1 !", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, makecol(255,0,0), -1);
        rest(3000);
    }


    destroy_bitmap(buffer);
    destroy_bitmap(fond);
    destroy_bitmap(vaisseau_image);
    destroy_bitmap(fond_menu);
    destroy_bitmap(ennemi_image);
    return 0;

}
END_OF_MAIN()



