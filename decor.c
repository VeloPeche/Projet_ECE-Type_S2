//
// Created by adrie on 14/05/2025.
//
/**************************************************************************
DECOR SCROLLING:
            D�placement automatique de l'�cran vers la gauche.
            On modifie les coordonn�es d'affichage du d�cor
            pour simuler un d�filement automatique.

**************************************************************************/

#include <allegro.h>

// Chargement "s�curis�" d'une image :
BITMAP * load_bitmap_check(char *nomImage){
    BITMAP *bmp;
    bmp=load_bitmap(nomImage,NULL);
    if (!bmp)
    {
        allegro_message("pas pu trouver %s",nomImage);
        exit(EXIT_FAILURE);
    }
    return bmp;
}

int main()
{
    BITMAP *page;        // BITMAP buffer d'affichage
    BITMAP *decor;       // Image de fond
    int screenx, screeny; // Position de l'�cran dans le d�cor

    allegro_init();
    install_keyboard();

    set_color_depth(desktop_color_depth());
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED,640,480,0,0)!=0)
    {
        allegro_message("prb gfx mode");
        allegro_exit();
        exit(EXIT_FAILURE);
    }

    page = create_bitmap(SCREEN_W, SCREEN_H);
    clear_bitmap(page);

    decor = load_bitmap_check("grandfond_decor.bmp");

    screenx = 0;
    screeny = 0;

    // Boucle principale
    while (!key[KEY_ESC])
    {
        // Scroll automatique vers la gauche (donc screenx augmente)
        screenx += 2;  // Vitesse de défilement, ajustable

        // Revenir au début si on atteint la fin du décor
        if (screenx > decor->w - SCREEN_W)
            screenx = 0;

        // Affichage du décor
        blit(decor, page, screenx, screeny, 0, 0, SCREEN_W, SCREEN_H);

        // Éléments fixés au décor
        circlefill(page, 540 - screenx, 50 - screeny, 25, makecol(255,128,128));
        textprintf_centre_ex(page, font, 540 - screenx, 100 - screeny, makecol(255,255,255), -1, "AFFICHAGE FIXE AU DECOR");

        // Éléments fixés à l'écran
        circlefill(page, 100, 50, 25, makecol(128,255,128));
        textprintf_centre_ex(page, font, 100, 100, makecol(255,255,255), -1, "AFFICHAGE FIXE A L'ECRAN");
        textprintf_centre_ex(page, font, 320, 10, makecol(255,255,255), -1, "DEFILEMENT AUTOMATIQUE GAUCHE");

        // Affichage du buffer à l'écran
        blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        rest(30); // Pause pour lisser le défilement
    }

    return 0;
}
END_OF_MAIN();
