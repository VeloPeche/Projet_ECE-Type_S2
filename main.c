#include <allegro.h>

// Chargement sécurisé d'une image
BITMAP *load_bitmap_check(char *nomImage) {
    BITMAP *bmp;
    bmp = load_bitmap(nomImage, NULL);
    if (!bmp) {
        allegro_message("pas pu trouver %s", nomImage);
        exit(EXIT_FAILURE);
    }
    return bmp;
}

// Structure pour les projectiles
typedef struct {
    int x;
    int y;
    int actif;
} Projectile;

// Structure pour le vaisseau
typedef struct {
    int x, y;
    int vitesse;
    int largeur, hauteur;
} Vaisseau;

// Fonction pour déplacer le vaisseau
void deplacement_vaisseau(Vaisseau *vaisseau, int dx, int dy) {
    vaisseau->x += dx;
    vaisseau->y += dy;

    // Limites d'écran
    if (vaisseau->x < 0) vaisseau->x = 0;
    if (vaisseau->x > SCREEN_W - vaisseau->largeur) vaisseau->x = SCREEN_W - vaisseau->largeur;
    if (vaisseau->y < 0) vaisseau->y = 0;
    if (vaisseau->y > SCREEN_H - vaisseau->hauteur) vaisseau->y = SCREEN_H - vaisseau->hauteur;
}

// Fonction pour déplacer les projectiles
void deplacement_projectile(Projectile *proj) {
    if (proj->actif) {
        proj->x += 5; // Vitesse du projectile
        if (proj->x > SCREEN_W) {
            proj->actif = 0; // Si le projectile dépasse l'écran, on le rend inactif
        }
    }
}

// Fonction pour afficher le vaisseau
void afficher_vaisseau(Vaisseau *vaisseau, BITMAP *page, BITMAP *image_vaisseau) {
    draw_sprite(page, image_vaisseau, vaisseau->x, vaisseau->y);
}

// Fonction pour afficher un projectile comme un petit carré
void afficher_projectile(Projectile *proj, BITMAP *page) {
    if (proj->actif) {
        rectfill(page, proj->x, proj->y, proj->x + 5, proj->y + 5, makecol(255, 0, 0)); // Petit carré rouge
    }
}

int main() {
    allegro_init();
    install_keyboard();
    set_color_depth(desktop_color_depth());
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0) != 0) {
        allegro_message("Problème avec le mode graphique");
        allegro_exit();
        exit(EXIT_FAILURE);
    }

    // Déclaration des objets
    BITMAP *page, *fond, *vaisseau_img;
    Vaisseau vaisseau = {100, 240, 4, 32, 32}; // Position et taille du vaisseau
    Projectile projectiles[10]; // Tableau de 10 projectiles
    int dx = 0, dy = 0;
    clock_t dernier_tir = 0;
    const int temps_tir = 500; // 500 ms pour tirer un projectile

    page = create_bitmap(SCREEN_W, SCREEN_H);
    fond = load_bitmap_check("Fond1600x600.bmp"); // Charger l'image du fond
    vaisseau_img = load_bitmap_check("Pacman4.bmp"); // Charger l'image du vaisseau

    // Initialisation des projectiles (inactifs au départ)
    for (int i = 0; i < 10; i++) {
        projectiles[i].actif = 0;
    }

    // Boucle principale
    while (!key[KEY_ESC]) {
        clear_bitmap(page);  // Effacer le buffer

        // Défilement du décor
        static int screenx = 0;
        screenx += 2; // Vitesse du défilement vers la gauche
        if (screenx > fond->w - SCREEN_W) {
            screenx = 0; // Revenir au début du décor
        }

        // Affichage du décor
        blit(fond, page, screenx, 0, 0, 0, SCREEN_W, SCREEN_H);

        // Déplacement du vaisseau avec les touches fléchées
        if (key[KEY_UP]) dy = -vaisseau.vitesse;
        if (key[KEY_DOWN]) dy = vaisseau.vitesse;
        if (key[KEY_LEFT]) dx = -vaisseau.vitesse;
        if (key[KEY_RIGHT]) dx = vaisseau.vitesse;
        deplacement_vaisseau(&vaisseau, dx, dy);

        // Vérifier si 500ms se sont écoulées avant de permettre un nouveau tir
        if (key[KEY_SPACE] && (clock() - dernier_tir >= temps_tir)) {
            // Chercher un projectile inactif
            for (int i = 0; i < 10; i++) {
                if (!projectiles[i].actif) {
                    projectiles[i].x = vaisseau.x + vaisseau.largeur;
                    projectiles[i].y = vaisseau.y + vaisseau.hauteur / 2;
                    projectiles[i].actif = 1; // Marquer ce projectile comme actif
                    dernier_tir = clock(); // Mettre à jour le dernier tir
                    break;
                }
            }
        }

        // Déplacement de tous les projectiles
        for (int i = 0; i < 10; i++) {
            deplacement_projectile(&projectiles[i]);
        }

        // Affichage du vaisseau et des projectiles
        afficher_vaisseau(&vaisseau, page, vaisseau_img);
        for (int i = 0; i < 10; i++) {
            afficher_projectile(&projectiles[i], page);
        }

        // Affichage du buffer à l'écran
        blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        rest(30); // Pause pour lisser le défilement
    }

    // Libération des ressources et fermeture
    destroy_bitmap(page);
    destroy_bitmap(fond);
    destroy_bitmap(vaisseau_img);
    allegro_exit();
    return 0;
}
END_OF_MAIN();




