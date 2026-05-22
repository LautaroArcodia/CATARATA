#ifndef LAYOUT_H
#define LAYOUT_H

/*
 * layout.h — Constantes y macros de layout
 *
 * Las constantes que NO dependen de la resolución (font, timing) siguen
 * siendo #define simples.
 *
 * Las que SÍ dependen de la resolución son macros que leen del struct
 * global g_layout (definido en resolucion.h). Así el resto del código
 * puede seguir escribiendo TABLERO_PIXEL_X, PANTALLA_ANCHO, etc., sin
 * saber que ahora son dinámicos.
 *
 * Antes de usar cualquiera de estas macros hay que llamar a
 * resolucion_inicializar() desde main.
 */

#include "resolucion.h"
#include "fuente.h"    /* GLIFO_ANCHO, GLIFO_ALTO, GLIFO_PASO */

/* ── Constantes fijas (no dependen de la resolución) ─────────────── */
#define MS_POR_FRAME          16
#define VELOCIDAD_INICIAL      3

/* ── Macros que leen del layout dinámico ─────────────────────────── */
#define PANTALLA_ANCHO             (g_layout.pantalla_ancho)
#define PANTALLA_ALTO              (g_layout.pantalla_alto)
#define ESCALA_INICIAL             (g_layout.escala_inicial)
#define TAMANO_BLOQUE_JUEGO        (g_layout.tamano_bloque_juego)
#define TABLERO_PIXEL_X            (g_layout.tablero_pixel_x)
#define TABLERO_PIXEL_Y            (g_layout.tablero_pixel_y)
#define HUD_X1                     (g_layout.hud_x1)
#define HUD_Y1                     (g_layout.hud_y1)
#define HUD_X2                     (g_layout.hud_x2)
#define HUD_Y2                     (g_layout.hud_y2)
#define TAMANO_BLOQUE_SIGUIENTE    (g_layout.tamano_bloque_siguiente)
#define GAMEOVER_X1                (g_layout.gameover_x1)
#define GAMEOVER_Y1                (g_layout.gameover_y1)
#define GAMEOVER_X2                (g_layout.gameover_x2)
#define GAMEOVER_Y2                (g_layout.gameover_y2)
#define GAMEOVER_ESCALA_TEXTO      (g_layout.gameover_escala_texto)
#define SALIR_PANEL_X1             (g_layout.salir_x1)
#define SALIR_PANEL_Y1             (g_layout.salir_y1)
#define SALIR_PANEL_X2             (g_layout.salir_x2)
#define SALIR_PANEL_Y2             (g_layout.salir_y2)
#define SALIR_ESCALA_TEXTO         (g_layout.salir_escala_texto)
#define STATS_X1                   (g_layout.stats_x1)
#define STATS_Y1                   (g_layout.stats_y1)
#define STATS_X2                   (g_layout.stats_x2)
#define STATS_Y2                   (g_layout.stats_y2)
#define LADRILLO_ANCHO_CELDA       (g_layout.ladrillo_ancho_celda)
#define LADRILLO_ALTO_CELDA        (g_layout.ladrillo_alto_celda)

#endif /* LAYOUT_H */
