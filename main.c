#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MAX_NAME_LENGTH 50
#define LEVEL_TIME_LIMIT 30 // Temps du niveau en secondes

typedef struct {
    float x, y;
    float vitesse;
    int nb_vie;
    int score_joueur;
} Vaisseau_joueur;

typedef struct {
    float x, y;
    float vitesse;
    int vivant;
} Ennemi;

typedef struct {
    float x, y;
    float vitesse;
    int actif;
} Projectile;

typedef struct {
    float x, y;
    float vitesse;
    int vivant;
    Projectile tir; // Projetiles des ennemis
} Ennemi_tir; // Nouvelle structure pour les ennemis tirants

// === Fonctions de jeu ===

void move_vaisseau(Vaisseau_joueur* vaisseau, int dirX, int dirY) {
    vaisseau->x += dirX * vaisseau->vitesse;
    vaisseau->y += dirY * vaisseau->vitesse;
    if (vaisseau->x < 0) vaisseau->x = 0;
    if (vaisseau->x > SCREEN_WIDTH - 32) vaisseau->x = SCREEN_WIDTH - 32;
    if (vaisseau->y < 0) vaisseau->y = 0;
    if (vaisseau->y > SCREEN_HEIGHT - 32) vaisseau->y = SCREEN_HEIGHT - 32;
}

void move_ennemi(Ennemi* e) {
    if (e->vivant) {
        e->x -= e->vitesse;
        if (e->x < 0) {
            e->x = SCREEN_WIDTH;
            e->y = rand() % (SCREEN_HEIGHT - 30);
        }
    }
}

void move_ennemi_tir(Ennemi_tir* e) {
    if (e->vivant) {
        e->tir.y += e->tir.vitesse;
        if (e->tir.y > SCREEN_HEIGHT) {
            e->tir.actif = 0;
        }
    }
}

void draw_vaisseau(Vaisseau_joueur* v, BITMAP* buffer, BITMAP* img) {
    masked_blit(img, buffer, 0, 0, (int)v->x, (int)v->y, img->w, img->h);
}

void draw_ennemi(Ennemi* e, BITMAP* buffer, BITMAP* ennemi_img) {
    if (e->vivant)
        masked_blit(ennemi_img, buffer, 0, 0, (int)e->x, (int)e->y, ennemi_img->w, ennemi_img->h);
}

void draw_projectile(Projectile* p, BITMAP* buffer) {
    if (p->actif)
        rectfill(buffer, p->x, p->y, p->x + 5, p->y + 5, makecol(255, 255, 0));
}

void draw_ennemi_tir(Ennemi_tir* e, BITMAP* buffer) {
    if (e->tir.actif)
        rectfill(buffer, e->tir.x, e->tir.y, e->tir.x + 5, e->tir.y + 5, makecol(255, 0, 0));
}

void check_collisions(Vaisseau_joueur* v, Ennemi* e, int n, Ennemi_tir* et, int m, Projectile* p) {
    // Collision avec les ennemis
    for (int i = 0; i < n; i++) {
        if (e[i].vivant &&
            v->x < e[i].x + 32 && v->x + 32 > e[i].x &&
            v->y < e[i].y + 32 && v->y + 32 > e[i].y) {
            v->nb_vie--;
            e[i].vivant = 0;
        }

        if (p->actif && e[i].vivant &&
            p->x + 5 > e[i].x && p->x < e[i].x + 32 &&
            p->y + 5 > e[i].y && p->y < e[i].y + 32) {
            e[i].vivant = 0;
            p->actif = 0;
            v->score_joueur += 100;
        }
    }

    // Collision avec les tirs des ennemis
    for (int i = 0; i < m; i++) {
        if (et[i].tir.actif &&
            v->x < et[i].tir.x + 5 && v->x + 32 > et[i].tir.x &&
            v->y < et[i].tir.y + 5 && v->y + 32 > et[i].tir.y) {
            v->nb_vie--;
            et[i].tir.actif = 0;
        }
    }
}

void reset_game(Vaisseau_joueur* v, Ennemi* e, int n, Ennemi_tir* et, int m, Projectile* p) {
    v->x = 400;
    v->y = 300;
    v->nb_vie = 3;
    v->score_joueur = 0;
    for (int i = 0; i < n; i++) {
        e[i].vivant = 1;
        e[i].x = SCREEN_WIDTH + i * 150;
        e[i].y = rand() % (SCREEN_HEIGHT - 30);
        e[i].vitesse = 2.0f;
    }

    for (int i = 0; i < m; i++) {
        et[i].vivant = 0;
        et[i].tir.actif = 0;
    }

    p->actif = 0;
}

void display_menu(BITMAP* buffer, BITMAP* background, const char* username, int sel) {
    stretch_blit(background, buffer,
        0, 0, background->w, background->h,
        0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    textout_centre_ex(buffer, font, "=== SPACE INVADERS ===", SCREEN_WIDTH / 2, 80, makecol(255, 255, 0), -1);
    textprintf_ex(buffer, font, 280, 130, makecol(255, 255, 255), -1, "Bienvenue %s !", username);

    const char* options[] = { "Nouvelle Partie", "Reprendre Partie", "Quitter" };
    int option_height = 200;
    for (int i = 0; i < 3; i++) {
        int color = (i == sel) ? makecol(0, 255, 0) : makecol(255, 255, 255);
        textout_centre_ex(buffer, font, options[i], SCREEN_WIDTH / 2, option_height + i * 40, color, -1);
    }

    blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

int main() {
    srand(time(NULL));
    allegro_init();
    install_keyboard();
    install_mouse();
    set_color_depth(32);

    set_gfx_mode(GFX_AUTODETECT_WINDOWED, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);

    show_mouse(screen);

    BITMAP* buffer = create_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT);
    BITMAP* background = load_bitmap("Fond2.bmp", NULL);
    BITMAP* vaisseau_image = load_bitmap("Pacman4.bmp", NULL);
    BITMAP* menu_background = load_bitmap("menu5.bmp", NULL);
    BITMAP* ennemi_image = load_bitmap("Fantome_rose.bmp", NULL);

    if (!buffer || !background || !vaisseau_image || !menu_background || !ennemi_image) {
        allegro_message("Erreur de chargement des images !");
        return -1;
    }

    char username[MAX_NAME_LENGTH] = "";
    clear_to_color(buffer, makecol(0, 0, 0));
    textout_ex(buffer, font, "Entrez votre nom d'utilisateur:", 300, 100, makecol(255, 255, 255), -1);
    textout_ex(buffer, font, "Nom:", 300, 150, makecol(255, 255, 255), -1);
    blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    while (!key[KEY_ENTER] && !key[KEY_ESC]) {
        if (keypressed()) {
            char ch = readkey() & 0xFF;
            if (ch == 8 && strlen(username) > 0)
                username[strlen(username) - 1] = '\0';
            else if (ch >= 32 && ch <= 126 && strlen(username) < MAX_NAME_LENGTH - 1) {
                username[strlen(username)] = ch;
                username[strlen(username) + 1] = '\0';
            }
        }
        rectfill(buffer, 450, 150, 650, 170, makecol(0, 0, 0));
        textout_ex(buffer, font, username, 450, 150, makecol(255, 255, 255), -1);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        rest(10);
    }

    while (key[KEY_ENTER]) rest(10);

    Vaisseau_joueur vaisseau = {400, 300, 5.0f, 3, 0};
    Ennemi ennemis[5];
    Ennemi_tir ennemis_tirs[5]; // Tableau d'ennemis tirants
    Projectile projectile = {0, 0, 10.0f, 0};

    for (int i = 0; i < 5; i++) {
        ennemis[i].x = SCREEN_WIDTH + i * 150;
        ennemis[i].y = rand() % (SCREEN_HEIGHT - 30);
        ennemis[i].vitesse = 2.0f;
        ennemis[i].vivant = 1;

        ennemis_tirs[i].x = ennemis[i].x;
        ennemis_tirs[i].y = ennemis[i].y;
        ennemis_tirs[i].vitesse = 4.0f;
        ennemis_tirs[i].vivant = 0;
        ennemis_tirs[i].tir.actif = 0;
    }

    int selection = 0;
    int max_selection = 2;
    int confirm_selection = 0;

    while (!key[KEY_ESC] && !confirm_selection) {
        display_menu(buffer, menu_background, username, selection);

        if (key[KEY_DOWN]) {
            selection = (selection + 1) % (max_selection + 1);
            rest(150);
        }
        if (key[KEY_UP]) {
            selection = (selection - 1 + (max_selection + 1)) % (max_selection + 1);
            rest(150);
        }

        if (mouse_x >= 300 && mouse_x <= 500) {
            if (mouse_y >= 200 && mouse_y <= 240) selection = 0;
            else if (mouse_y >= 240 && mouse_y <= 280) selection = 1;
            else if (mouse_y >= 280 && mouse_y <= 320) selection = 2;
        }

        if (mouse_b & 1) {
            confirm_selection = 1;
        }

        rest(50);
    }

    if (selection == 0) {
        reset_game(&vaisseau, ennemis, 5, ennemis_tirs, 5, &projectile);
    } else if (selection == 1) {
        // Charger une sauvegarde (non implémenté)
    } else {
        destroy_bitmap(buffer);
        destroy_bitmap(background);
        destroy_bitmap(vaisseau_image);
        destroy_bitmap(menu_background);
        destroy_bitmap(ennemi_image);
        return 0;
    }

    // Affichage du niveau 1 avant de commencer
    clear_to_color(buffer, makecol(0, 0, 0));
    textout_centre_ex(buffer, font, "Niveau 1", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, makecol(255, 255, 255), -1);
    blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    rest(2000); // Affiche "Niveau 1" pendant 2 secondes

    int game_time = 0;
    int game_over = 0;

    while (!key[KEY_ESC] && !game_over) {
        clear_to_color(buffer, makecol(0, 0, 0));
        draw_sprite(buffer, background, 0, 0);

        int directionX = 0, directionY = 0;
        if (key[KEY_UP]) directionY = -1;
        if (key[KEY_DOWN]) directionY = 1;
        if (key[KEY_LEFT]) directionX = -1;
        if (key[KEY_RIGHT]) directionX = 1;

        move_vaisseau(&vaisseau, directionX, directionY);

        if (key[KEY_SPACE] && !projectile.actif) {
            // Lancer le projectile du centre de Pacman
            projectile.x = vaisseau.x + 16;  // Centre de Pacman
            projectile.y = vaisseau.y + 16;  // Centre de Pacman
            projectile.actif = 1;
        }

        if (projectile.actif) {
            projectile.x += projectile.vitesse;
            if (projectile.x > SCREEN_WIDTH) {
                projectile.actif = 0;
            }
        }

        // Mouvements ennemis et tir
        for (int i = 0; i < 5; i++) {
            move_ennemi(&ennemis[i]);
            if (ennemis[i].vivant && !ennemis_tirs[i].tir.actif) {
                // Tir des ennemis (lancer un tir)
                ennemis_tirs[i].tir.x = ennemis[i].x + 16; // Centre de l'ennemi
                ennemis_tirs[i].tir.y = ennemis[i].y + 16;
                ennemis_tirs[i].tir.actif = 1;
            }
            move_ennemi_tir(&ennemis_tirs[i]);
        }

        check_collisions(&vaisseau, ennemis, 5, ennemis_tirs, 5, &projectile);

        draw_vaisseau(&vaisseau, buffer, vaisseau_image);
        for (int i = 0; i < 5; i++) {
            draw_ennemi(&ennemis[i], buffer, ennemi_image);
            draw_ennemi_tir(&ennemis_tirs[i], buffer);
        }
        draw_projectile(&projectile, buffer);

        // Affichage du score et du nombre de vies
        textprintf_ex(buffer, font, 10, 10, makecol(255, 255, 255), -1, "Score: %d", vaisseau.score_joueur);
        textprintf_ex(buffer, font, 10, 30, makecol(255, 255, 255), -1, "Vies: %d", vaisseau.nb_vie);

        // Affichage du chronomètre
        game_time++;
        int remaining_time = LEVEL_TIME_LIMIT - game_time / 30; // Convertir les frames en secondes
        textprintf_ex(buffer, font, SCREEN_WIDTH - 100, 10, makecol(255, 255, 255), -1, "Temps: %d", remaining_time);

        if (remaining_time <= 0) {
            game_over = 1; // Fin du niveau
        }

        blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        rest(20); // Augmenter la vitesse en réduisant la pause
    }

    destroy_bitmap(buffer);
    destroy_bitmap(background);
    destroy_bitmap(vaisseau_image);
    destroy_bitmap(menu_background);
    destroy_bitmap(ennemi_image);

    return 0;
}
END_OF_MAIN()
