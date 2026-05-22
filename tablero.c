#include "tablero.h"

/* =========================================================
 * FUNCIONES AUXILIARES INTERNAS
 * ========================================================= */

/* Retorna 1 si todas las celdas de la fila están ocupadas. */
static int fila_esta_completa(const tTablero *tablero, int fila)
{
    for (int col = 0; col < tablero->columnas; col++)
    {
        if (tablero->celdas_ocupadas[col][fila] == 0)
            return 0;
    }
    return 1;
}

/* Deja todas las celdas de la fila en 0 (vacía). */
static void vaciar_fila(tTablero *tablero, int fila)
{
    for (int col = 0; col < tablero->columnas; col++)
    {
        tablero->celdas_ocupadas[col][fila] = 0;
    }
}

/*
 * Baja una posición todas las filas que están por encima de 'desde'.
 * La fila 'desde' queda sobreescrita con la de arriba.
 */
static void bajar_filas_sobre(tTablero *tablero, int desde)
{
    for (int fila = desde; fila > 0; fila--)
    {
        for (int col = 0; col < tablero->columnas; col++)
        {
            tablero->celdas_ocupadas[col][fila] = tablero->celdas_ocupadas[col][fila - 1];
        }
    }
}

/* =========================================================
 * MODO CLASICO
 * ========================================================= */

/* ── Validación clásica (bloqueante en los bordes) ────────────── */

int tablero_movimiento_valido(const tTablero *tablero, int columna_destino, int fila_destino, const tFormaPieza forma)
{
    for (int fila_forma = 0; fila_forma < DIMENSION_MATRIZ; fila_forma++)
    {
        for (int col_forma = 0; col_forma < DIMENSION_MATRIZ; col_forma++)
        {
            if (forma[fila_forma][col_forma] == 0)
                continue;

            int col_tablero  = columna_destino + col_forma;
            int fila_tablero = fila_destino    + fila_forma;

            if (col_tablero < 0 || col_tablero >= tablero->columnas || fila_tablero >= TABLERO_FILAS)
                return 0;

            if (fila_tablero < 0)
                continue;

            if (tablero->celdas_ocupadas[col_tablero][fila_tablero] != 0)
                return 0;
        }
    }
    return 1;
}

/* ── Fijación clásica ─────────────────────────────────────────── */

void tablero_fijar_pieza(tTablero *tablero, int columna_destino, int fila_destino, const tFormaPieza forma, uint8_t color_paleta)
{
    for (int fila_forma = 0; fila_forma < DIMENSION_MATRIZ; fila_forma++)
    {
        for (int col_forma = 0; col_forma < DIMENSION_MATRIZ; col_forma++)
        {
            if (forma[fila_forma][col_forma] == 0)
                continue;

            int col_tablero  = columna_destino + col_forma;
            int fila_tablero = fila_destino    + fila_forma;

            if (col_tablero >= 0 && col_tablero < tablero->columnas && fila_tablero >= 0 && fila_tablero < TABLERO_FILAS)
            {
                tablero->celdas_ocupadas[col_tablero][fila_tablero] = color_paleta;
            }
        }
    }
}

/* =========================================================
 * MODO DX
 * ========================================================= */

/* ── Validación circular para el modo DX ─────────────────────── */

int tablero_movimiento_valido_dx(const tTablero *tablero, int columna_destino, int fila_destino, const tFormaPieza forma)
{
    for (int fila_forma = 0; fila_forma < DIMENSION_MATRIZ; fila_forma++)
    {
        for (int col_forma = 0; col_forma < DIMENSION_MATRIZ; col_forma++)
        {
            if (forma[fila_forma][col_forma] == 0)
                continue;

            int fila_tablero = fila_destino + fila_forma;

            if (fila_tablero >= TABLERO_FILAS)
                return 0;   /* sí se bloquea en la base */

            if (fila_tablero < 0)
                continue;   /* zona de spawn, siempre libre */

            /* Las columnas se envuelven en lugar de bloquear. */
            int col_tablero = NORMALIZAR_COLUMNA(columna_destino + col_forma, tablero->columnas);

            if (tablero->celdas_ocupadas[col_tablero][fila_tablero] != 0)
                return 0;
        }
    }
    return 1;
}

/* ── Fijación circular para el modo DX ───────────────────────── */

void tablero_fijar_pieza_dx(tTablero *tablero, int columna_destino, int fila_destino, const tFormaPieza forma, uint8_t color_paleta)
{
    for (int fila_forma = 0; fila_forma < DIMENSION_MATRIZ; fila_forma++)
    {
        for (int col_forma = 0; col_forma < DIMENSION_MATRIZ; col_forma++)
        {
            if (forma[fila_forma][col_forma] == 0)
                continue;

            int fila_tablero = fila_destino + fila_forma;

            if (fila_tablero < 0 || fila_tablero >= TABLERO_FILAS)
                continue;

            int col_tablero = NORMALIZAR_COLUMNA(columna_destino + col_forma, tablero->columnas);

            tablero->celdas_ocupadas[col_tablero][fila_tablero] = color_paleta;
        }
    }
}

/* =========================================================
 * API PÚBLICA
 * ========================================================= */

void tablero_inicializar(tTablero *tablero, int columnas)
{
    if (columnas < TABLERO_COLUMNAS_MIN)
    {
        columnas = TABLERO_COLUMNAS_MIN;
    }

    if (columnas > TABLERO_COLUMNAS_MAX)
    {
        columnas = TABLERO_COLUMNAS_MAX;
    }

    tablero->columnas = columnas;
    tablero_reiniciar(tablero);
}

void tablero_reiniciar(tTablero *tablero)
{
    /* Limpia la totalidad del array para no dejar basura en columnas que podrían quedar fuera del ancho activo si éste cambia. */

    for (int col = 0; col < TABLERO_COLUMNAS_MAX; col++)
    {
        for (int fila = 0; fila < TABLERO_FILAS; fila++)
        {
            tablero->celdas_ocupadas[col][fila] = 0;
        }
    }
}

/* ── Eliminación de filas completas ──────────────────────────── */

/*
 * Recorre el tablero de abajo hacia arriba.
 * Cuando encuentra una fila completa:
 *   1. Baja todas las filas de arriba una posición.
 *   2. Vacía la fila superior (que quedó duplicada).
 *   3. No avanza el índice: la misma posición ahora tiene la fila que estaba arriba, hay que revisarla de nuevo.
 */
int tablero_eliminar_filas_completas(tTablero *tablero)
{
    int cantidad_eliminadas = 0;
    int fila_actual = TABLERO_FILAS - 1;

    while (fila_actual >= 0)
    {
        if (fila_esta_completa(tablero, fila_actual))
        {
            bajar_filas_sobre(tablero, fila_actual);
            vaciar_fila(tablero, 0);
            cantidad_eliminadas++;
        }
        else
        {
            fila_actual--;
        }
    }

    return cantidad_eliminadas;
}

int tablero_hay_derrota(const tTablero *tablero)
{
    for (int col = 0; col < tablero->columnas; col++)
    {
        if (tablero->celdas_ocupadas[col][0] != 0)
            return 1;
    }
    return 0;
}
