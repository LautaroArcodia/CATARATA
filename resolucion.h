#ifndef RESOLUCION_H
#define RESOLUCION_H

/*
 * resolucion.h — Resoluciones lógicas soportadas
 *
 * El juego soporta dos resoluciones lógicas (el tamaño del canvas):
 *
 *   CGA: 320 × 200 píxeles  (estilo retro clásico)
 *   VGA: 640 × 480 píxeles  (más espacio, bloques más grandes)
 *
 * Todos los valores de posición y tamaño que dependen de la resolución
 * viven en el struct tLayoutResolucion. La variable global g_layout
 * se inicializa al inicio del programa según la resolución elegida,
 * y las macros de layout.h leen de ella.
 *
 * Esto permite que el resto del código siga usando TABLERO_PIXEL_X,
 * PANTALLA_ANCHO, etc., sin saber que ahora son dinámicos.
 */

typedef enum
{
    RES_CGA = 0,   /* 320 × 200 — resolución por defecto */
    RES_VGA = 1    /* 640 × 480 */
} eResolucion;

typedef struct
{
    /* ── Pantalla ──────────────────────────────────────────── */
    int pantalla_ancho;
    int pantalla_alto;
    int escala_inicial;        /* escala de ventana por defecto    */

    /* ── Tablero de juego ──────────────────────────────────── */
    int tamano_bloque_juego;   /* px por celda                     */
    int tablero_pixel_x;       /* x del borde izquierdo del tablero*/
    int tablero_pixel_y;       /* y del borde superior del tablero */

    /* ── Panel HUD ─────────────────────────────────────────── */
    int hud_x1, hud_y1;
    int hud_x2, hud_y2;
    int tamano_bloque_siguiente; /* bloques en la previsualización */

    /* ── Panel Game Over ───────────────────────────────────── */
    int gameover_x1, gameover_y1;
    int gameover_x2, gameover_y2;
    int gameover_escala_texto;

    /* ── Panel de Estadísticas (stats) ─────────────────────── */
    int stats_x1, stats_y1;
    int stats_x2, stats_y2;

    /* ── Fondo decorativo (ladrillos) ──────────────────────── */
    int ladrillo_ancho_celda;
    int ladrillo_alto_celda;
} tLayoutResolucion;

/* Variable global que contiene el layout activo.
 * Se inicializa con resolucion_inicializar() antes de abrir la ventana. */
extern tLayoutResolucion g_layout;

/*
 * Carga en g_layout los valores correspondientes a la resolución indicada.
 * Debe llamarse antes de abrir la ventana y antes de dibujar cualquier cosa.
 */
void resolucion_inicializar(eResolucion res);

/* Convierte "CGA" o "VGA" (cualquier capitalización) a eResolucion.
 * Si la cadena no se reconoce, retorna RES_CGA. */
eResolucion resolucion_desde_cadena(const char *s);

#endif /* RESOLUCION_H */
