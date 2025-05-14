#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_PROJECTILES 10
#define MAX_ENNEMIS 10
#define LARGEUR_ENNEMI 32
#define HAUTEUR_ENNEMI 32
#define VITESSE_ENNEMI 2
#define VIES_INITIALES 3

typedef struct {
    int x, y;
    int actif;
} Projectile;

typedef struct {
    int x, y;
    int vitesse;
    int largeur, hauteur;
    int nb_vie;
} Vaisseau;

typedef struct {
    int x, y;
    int actif;
} Ennemi;

BITMAP *load_bitmap_check(char *nomImage) {
    BITMAP *bmp = load_bitmap(nomImage, NULL);
    if (!bmp) {
        allegro_message("Erreur: %s non trouvé", nomImage);
        exit(EXIT_FAILURE);
    }
    return bmp;
}

void deplacement_vaisseau(Vaisseau *vaisseau, int dx, int dy) {
    vaisseau->x += dx;
    vaisseau->y += dy;
    if (vaisseau->x < 0) vaisseau->x = 0;
    if (vaisseau->x > SCREEN_W - vaisseau->largeur) vaisseau->x = SCREEN_W - vaisseau->largeur;
    if (vaisseau->y < 0) vaisseau->y = 0;
    if (vaisseau->y > SCREEN_H - vaisseau->hauteur) vaisseau->y = SCREEN_H - vaisseau->hauteur;
}

void deplacement_projectile(Projectile *p) {
    if (p->actif) {
        p->x += 5;
        if (p->x > SCREEN_W) p->actif = 0;
    }
}

void deplacement_ennemi(Ennemi *e) {
    if (e->actif) {
        e->x -= VITESSE_ENNEMI;
        if (e->x < -LARGEUR_ENNEMI) e->actif = 0;
    }
}

void detecter_collisions(Projectile *proj, Ennemi *ennemi, int *score) {
    if (proj->actif && ennemi->actif) {
        if (proj->x >= ennemi->x && proj->x <= ennemi->x + LARGEUR_ENNEMI &&
            proj->y >= ennemi->y && proj->y <= ennemi->y + HAUTEUR_ENNEMI) {
            proj->actif = 0;
            ennemi->actif = 0;
            (*score) += 100;
        }
    }
}

int collision_vaisseau_ennemi(Vaisseau *vaisseau, Ennemi *ennemi) {
    return ennemi->actif &&
           vaisseau->x < ennemi->x + LARGEUR_ENNEMI && vaisseau->x + vaisseau->largeur > ennemi->x &&
           vaisseau->y < ennemi->y + HAUTEUR_ENNEMI && vaisseau->y + vaisseau->hauteur > ennemi->y;
}

void afficher_barre_vie(BITMAP* buffer, Vaisseau* vaisseau) {
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


int main() {
    allegro_init(); install_keyboard(); install_mouse();
    set_color_depth(32);
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0)) {
        allegro_message("Erreur mode graphique"); exit(EXIT_FAILURE);
    }

    BITMAP *page = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP *fond_menu = load_bitmap_check("menu5.bmp");
    BITMAP *fond = load_bitmap_check("Fond1600x600.bmp");
    BITMAP *vaisseau_img = load_bitmap_check("Pacman4.bmp");
    BITMAP *ennemi_img = load_bitmap_check("Fantome_rose.bmp");

    // Menu nom joueur
    char nom[50] = "";
    int entree_valide = 0;

    while (!entree_valide) {
        clear_bitmap(page);
        draw_sprite(page, fond_menu, 0, 0);
        textout_centre_ex(page, font, "Entrez votre nom:", SCREEN_W / 2, 100, makecol(255, 255, 255), -1);
        textout_ex(page, font, nom, SCREEN_W / 2 - 50, 130, makecol(0, 255, 0), -1);
        blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        if (keypressed()) {
            int k = readkey();
            char c = k & 0xff;
            if (c == 8 && strlen(nom) > 0) nom[strlen(nom)-1] = '\0';
            else if (c == 13 && strlen(nom) > 0) entree_valide = 1;
            else if (isprint(c) && strlen(nom) < 49) {
                nom[strlen(nom)] = c;
                nom[strlen(nom) + 1] = '\0';
            }
        }
    }

    // Initialisation du jeu
    Vaisseau vaisseau = {50, 240, 4, 32, 32};
    Projectile projectiles[MAX_PROJECTILES] = {0};
    Ennemi ennemis[MAX_ENNEMIS] = {0};
    int vies = VIES_INITIALES;
    int score = 0;
    int screenx = 0;
    clock_t dernier_tir = 0;
    const int temps_tir = 500;

    // Boucle principale du jeu
    while (!key[KEY_ESC] && vies > 0) {
        clear_bitmap(page);
        screenx += 2;
        if (screenx > fond->w - SCREEN_W) screenx = 0;
        blit(fond, page, screenx, 0, 0, 0, SCREEN_W, SCREEN_H);

        int dx = 0, dy = 0;
        if (key[KEY_UP]) dy = -vaisseau.vitesse;
        if (key[KEY_DOWN]) dy = vaisseau.vitesse;
        if (key[KEY_LEFT]) dx = -vaisseau.vitesse;
        if (key[KEY_RIGHT]) dx = vaisseau.vitesse;
        deplacement_vaisseau(&vaisseau, dx, dy);

        if (key[KEY_SPACE] && (clock() - dernier_tir >= temps_tir)) {
            for (int i = 0; i < MAX_PROJECTILES; i++) {
                if (!projectiles[i].actif) {
                    projectiles[i].x = vaisseau.x + vaisseau.largeur;
                    projectiles[i].y = vaisseau.y + vaisseau.hauteur / 2;
                    projectiles[i].actif = 1;
                    dernier_tir = clock();
                    break;
                }
            }
        }

        // Spawn ennemi aléatoire
        if (rand() % 60 == 0) {
            for (int i = 0; i < MAX_ENNEMIS; i++) {
                if (!ennemis[i].actif) {
                    ennemis[i].actif = 1;
                    ennemis[i].x = SCREEN_W;
                    ennemis[i].y = rand() % (SCREEN_H - HAUTEUR_ENNEMI);
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_PROJECTILES; i++) {
            deplacement_projectile(&projectiles[i]);
        }

        for (int i = 0; i < MAX_ENNEMIS; i++) {
            deplacement_ennemi(&ennemis[i]);
            if (collision_vaisseau_ennemi(&vaisseau, &ennemis[i])) {
                vies--;
                ennemis[i].actif = 0;
            }
            for (int j = 0; j < MAX_PROJECTILES; j++) {
                detecter_collisions(&projectiles[j], &ennemis[i], &score);
            }
        }

        draw_sprite(page, vaisseau_img, vaisseau.x, vaisseau.y);
        for (int i = 0; i < MAX_PROJECTILES; i++) {
            if (projectiles[i].actif)
                rectfill(page, projectiles[i].x, projectiles[i].y, projectiles[i].x + 5, projectiles[i].y + 5, makecol(255, 0, 0));
        }
        for (int i = 0; i < MAX_ENNEMIS; i++) {
            if (ennemis[i].actif)
                draw_sprite(page, ennemi_img, ennemis[i].x, ennemis[i].y);
        }

        textprintf_ex(page, font, 10, 10, makecol(255, 255, 255), -1, "Vies: %d", vies);
        textprintf_ex(page, font, 10, 25, makecol(255, 255, 0), -1, "Score: %d", score);
        afficher_barre_vie(page, &vaisseau);
        blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
        rest(30);
    }

    if (vies == 0) {
        textout_centre_ex(screen, font, "GAME OVER", SCREEN_W / 2, SCREEN_H / 2, makecol(255, 0, 0), -1);
        rest(2000);
    }

    destroy_bitmap(page);
    destroy_bitmap(fond);
    destroy_bitmap(fond_menu);
    destroy_bitmap(vaisseau_img);
    destroy_bitmap(ennemi_img);
    allegro_exit();
    return 0;
}
END_OF_MAIN();
