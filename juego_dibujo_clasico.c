#include "juego_dibujo_clasico.h"
#include "juego_dibujo_interna.h"
#include "GBT/gbt.h"

/*
 * juego_dibujo_clasico.c — Renderizado del Modo Clásico
 *
 * Dibuja la pieza cayendo y la ghost piece con las reglas clásicas:
 * los minos que salen del área del tablero se descartan (nunca ocurre
 * en el modo clásico porque los bordes bloquean el movimiento).
 */

/* ── Pieza cayendo ───────────────────────────────────────────── */

static void dibujar_pieza_cayendo(const tPiezaActiva *pieza)
{
    uint8_t color = pieza_obtener_color_paleta(pieza->tipo);

    for (int fila_forma = 0; fila_forma < DIMENSION_MATRIZ; fila_forma++)
    {
        for (int col_forma = 0; col_forma < DIMENSION_MATRIZ; col_forma++)
        {
            if (!pieza->forma_actual[fila_forma][col_forma])
                continue;

            int fila_tablero = pieza->fila_tablero + fila_forma;
            int col_tablero  = pieza->columna_tablero + col_forma;

            if (fila_tablero < 0 || fila_tablero >= TABLERO_FILAS)
                continue;

            if (col_tablero  < 0 || col_tablero  >= TABLERO_COLUMNAS)
                continue;

            dibujar_bloque_3d(TABLERO_PIXEL_X + col_tablero  * TAMANO_BLOQUE_JUEGO,
                              TABLERO_PIXEL_Y + fila_tablero * TAMANO_BLOQUE_JUEGO,
                              color);
        }
    }
}

/* ── Ghost piece ─────────────────────────────────────────────── */

static int calcular_fila_ghost(const tPiezaActiva *pieza, const tTablero *tablero)
{
    int fila = pieza->fila_tablero;

    while (tablero_movimiento_valido(tablero, pieza->columna_tablero, fila + 1, pieza->forma_actual))
    {
        fila++;
    }
    return fila;
}

static void dibujar_ghost(const tPiezaActiva *pieza, const tTablero *tablero)
{
    int fila_destino = calcular_fila_ghost(pieza, tablero);

    if (fila_destino == pieza->fila_tablero) return;

    uint8_t   color         = pieza_obtener_color_paleta(pieza->tipo);
    const int tamano_bloque = TAMANO_BLOQUE_JUEGO;

    for (int fila_forma = 0; fila_forma < DIMENSION_MATRIZ; fila_forma++)
    {
        for (int col_forma = 0; col_forma < DIMENSION_MATRIZ; col_forma++)
        {
            if (!pieza->forma_actual[fila_forma][col_forma])
                continue;

            int fila_tablero = fila_destino + fila_forma;
            int col_tablero  = pieza->columna_tablero + col_forma;

            if (fila_tablero < 0 || fila_tablero >= TABLERO_FILAS)
                continue;

            if (col_tablero  < 0 || col_tablero  >= TABLERO_COLUMNAS)
                continue;

            if (tablero->celdas_ocupadas[col_tablero][fila_tablero])
                continue;

            int pixel_x = TABLERO_PIXEL_X + col_tablero  * tamano_bloque;
            int pixel_y = TABLERO_PIXEL_Y + fila_tablero * tamano_bloque;

            for (int i = 0; i < tamano_bloque; i++)
            {
                dibujar_pixel(pixel_x + i,
                              pixel_y,
                              color);

                dibujar_pixel(pixel_x + i,
                              pixel_y + tamano_bloque - 1,
                              color);

                dibujar_pixel(pixel_x,
                              pixel_y + i,
                              color);

                dibujar_pixel(pixel_x + tamano_bloque - 1,
                              pixel_y + i,
                              color);
            }
        }
    }
}

/* ── API del modo ────────────────────────────────────────────── */

void clasico_dibujar(const tEstadoJuego *estado)
{
    renderizar_base(estado);
    dibujar_ghost(&estado->pieza_activa, &estado->tablero);
    dibujar_pieza_cayendo(&estado->pieza_activa);
}
