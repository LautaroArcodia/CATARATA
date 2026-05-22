#ifndef JUEGO_DIBUJO_INTERNA_H
#define JUEGO_DIBUJO_INTERNA_H

/*
 * juego_dibujo_interna.h
 *
 * Helpers de dibujo compartidos entre los archivos de cada modo.
 * No es un header público: solo lo incluyen juego_dibujo_clasico.c
 * y juego_dibujo_dx.c.
 */

#include "juego.h"
#include "layout.h"   /* TABLERO_PIXEL_X/Y, TAMANO_BLOQUE_JUEGO, HUD_*, etc. */
#include "dibujo.h"   /* dibujar_pixel, rellenar_rectangulo, texto_*, etc.   */
#include <stdint.h>

/* ── Constantes ──────────────────────────────────────────────── */
#define TABLERO_ALTO_PIXELES        (TABLERO_FILAS * TAMANO_BLOQUE_JUEGO)
#define TABLERO_ANCHO_PIXELES(cols) ((cols) * TAMANO_BLOQUE_JUEGO)

/* Envuelve una columna al rango [0, cols) — tablero circular DX */
#define COL_WRAP(col, cols)  (((col) % (cols) + (cols)) % (cols))

/* ── Bloques ─────────────────────────────────────────────────── */
void dibujar_bloque_3d    (int x, int y, uint8_t color);
void dibujar_bloque_simple(int x, int y, int tam, uint8_t color);

/* ── Tablero ─────────────────────────────────────────────────── */
void dibujar_area_tablero_vacia(const tTablero *tablero);
void dibujar_bloques_fijados   (const tTablero *tablero);

/* ── Paneles ─────────────────────────────────────────────────── */
void dibujar_panel_hud         (const tEstadoJuego *estado);
void dibujar_panel_estadisticas(const tEstadoJuego *estado);

/*
 * Renderiza los elementos comunes a ambos modos:
 * pared de fondo, HUD, cuadrícula del tablero, bloques fijados y
 * panel de estadísticas. No hace flip del backbuffer.
 */
void renderizar_base(const tEstadoJuego *estado);

#endif /* JUEGO_DIBUJO_INTERNA_H */
