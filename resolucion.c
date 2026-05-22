/*
 * resolucion.c — Tablas de layout para cada resolucion
 *
 * Contiene los valores concretos de posicion para CGA y VGA.
 * Si queres ajustar el layout, modifica los valores del struct.
 *
 * CGA (320x200): bloque de 8px.
 * VGA (640x480): bloque de 20px.
 */

#include "resolucion.h"
#include "cadena.h"

/* Variable global que contiene el layout activo */
tLayoutResolucion g_layout;

/* ── CGA: 320 x 200, bloque 8px ── */
static const tLayoutResolucion LAYOUT_CGA = {
    .pantalla_ancho          = 320,
    .pantalla_alto           = 200,
    .escala_inicial          = 2,

    .tamano_bloque_juego     = 8,
    .tablero_pixel_x         = 70,
    .tablero_pixel_y         = 20,

    .hud_x1                  = 204,
    .hud_y1                  = 17,
    .hud_x2                  = 314,
    .hud_y2                  = 182,
    .tamano_bloque_siguiente = 5,

    .gameover_x1             = 55,
    .gameover_y1             = 56,
    .gameover_x2             = 265,
    .gameover_y2             = 160,
    .gameover_escala_texto   = 3,

    .stats_x1                = 6,
    .stats_y1                = 17,
    .stats_x2                = 60,
    .stats_y2                = 182,

    .ladrillo_ancho_celda    = 11,
    .ladrillo_alto_celda     = 7
};

/* ── VGA: 640 x 480, bloque 20px ── */
static const tLayoutResolucion LAYOUT_VGA = {
    .pantalla_ancho          = 640,
    .pantalla_alto           = 480,
    .escala_inicial          = 2,

    .tamano_bloque_juego     = 18,
    .tablero_pixel_x         = 100,
    .tablero_pixel_y         = 74,

    .hud_x1                  = 410,
    .hud_y1                  = 74,
    .hud_x2                  = 630,
    .hud_y2                  = 406,
    .tamano_bloque_siguiente = 10,

    .gameover_x1             = 90,
    .gameover_y1             = 152,
    .gameover_x2             = 550,
    .gameover_y2             = 350,
    .gameover_escala_texto   = 5,

    .stats_x1                = 10,
    .stats_y1                = 74,
    .stats_x2                = 90,
    .stats_y2                = 406,

    .ladrillo_ancho_celda    = 22,
    .ladrillo_alto_celda     = 14
};

/* =========================================================
 * resolucion_inicializar — carga el layout en g_layout
 * ========================================================= */
void resolucion_inicializar(eResolucion res)
{
    g_layout = (res == RES_VGA) ? LAYOUT_VGA : LAYOUT_CGA;
}

/* =========================================================
 * resolucion_desde_cadena — convierte "CGA" o "VGA" a enum
 *
 * Acepta cualquier combinacion de mayusculas/minusculas
 * ("vga", "VGA", "Vga", etc.). Si no reconoce, retorna CGA.
 * ========================================================= */
eResolucion resolucion_desde_cadena(const char *s)
{
    if (!s)
        return RES_CGA;

    if (cadena_son_iguales_ignorando_mayusculas(s, "VGA", 3))
        return RES_VGA;

    return RES_CGA;
}
