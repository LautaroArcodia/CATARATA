#ifndef DIBUJO_H
#define DIBUJO_H

/*
 * dibujo.h — Primitivas de dibujado compartidas
 *
 * Este modulo NO conoce nada del juego (tablero, piezas, estado).
 * Solo sabe dibujar pixeles, rectangulos, marcos, texto y fondo.
 *
 * FUENTES DISPONIBLES:
 *   Fuente normal  (5x7 px)  → texto(), texto_centrado(), texto_escalado()
 *   Fuente grande (10x14 px) → texto_grande_centrado()
 */

#include <stdint.h>

/* =========================================================
 * COLORES DE PALETA
 *
 * Indices de los 15 colores cargados en main.c con
 * gbt_aplicar_paleta(). No modificar sin actualizar main.c.
 * ========================================================= */

/* ── Colores de piezas ────────────────────────────────────── */
#define COLOR_PIEZA_I         1   /* cian     */
#define COLOR_PIEZA_J         2   /* azul     */
#define COLOR_PIEZA_L         3   /* naranja  */
#define COLOR_PIEZA_O         4   /* amarillo */
#define COLOR_PIEZA_S         5   /* verde    */
#define COLOR_PIEZA_T         6   /* violeta  */
#define COLOR_PIEZA_Z         7   /* rojo     */

/* ── Colores generales de UI ──────────────────────────────── */
#define COLOR_NEGRO           0
#define COLOR_ROJO            7
#define COLOR_BLANCO          8
#define COLOR_GRIS_OSCURO     9
#define COLOR_GRIS_SOMBRA    10
#define COLOR_FONDO_TABLERO  11
#define COLOR_LADRILLO_BASE  12
#define COLOR_LADRILLO_CLARO 13
#define COLOR_MORTERO        14

/* =========================================================
 * PRIMITIVAS — nivel mas bajo de dibujo
 * ========================================================= */

void dibujar_pixel(int x, int y, uint8_t color);

void rellenar_rectangulo(int x1, int y1, int x2, int y2, uint8_t color);

void dibujar_marco_incrustado_en(int x1, int y1, int x2, int y2);

/* =========================================================
 * TEXTO — FUENTE NORMAL (5x7 px)
 * ========================================================= */

void texto(const char *s, int x, int y, uint8_t color);

void texto_escalado(const char *s, int x, int y, uint8_t color, int escala);

void texto_centrado(const char *s, int centro_x, int y, uint8_t color);

void texto_centrado_escalado(const char *s, int centro_x, int y, uint8_t color, int escala);

/* =========================================================
 * TEXTO — FUENTE GRANDE (10x14 px)
 * ========================================================= */

void texto_grande_centrado(const char *s, int centro_x, int y, uint8_t color);

/* =========================================================
 * FONDOS
 * ========================================================= */

void dibujar_pared_ladrillos_completa(void);

#endif /* DIBUJO_H */
