#ifndef TABLERO_H
#define TABLERO_H

#include "pieza.h"
#include <stdint.h>

/* =========================================================
 * DIMENSIONES DEL TABLERO
 * ========================================================= */
#define TABLERO_COLUMNAS_MAX  16   /* máximo en modo DX                  */
#define TABLERO_COLUMNAS      10   /* ancho clásico (compatibilidad)     */
#define TABLERO_COLUMNAS_MIN   8   /* mínimo configurable en modo DX     */
#define TABLERO_FILAS         20

/*
 * Normaliza una columna al rango [0, columnas) para el tablero circular. Si la columna se sale por un lado, vuelve por el otro.
 */
#define NORMALIZAR_COLUMNA(col, columnas)  (((col) % (columnas) + (columnas)) % (columnas))

/* =========================================================
 * TABLERO DE JUEGO
 *
 * celdas_ocupadas[columna][fila]
 *   0   = celda libre
 *   1-7 = color de paleta de la pieza almacenada en esa celda
 *
 * El campo 'columnas' indica el ancho activo:
 *   - Modo Clásico : siempre TABLERO_COLUMNAS (10)
 *   - Modo DX      : entre TABLERO_COLUMNAS_MIN y TABLERO_COLUMNAS_MAX
 * ========================================================= */
typedef struct
{
    uint8_t celdas_ocupadas[TABLERO_COLUMNAS_MAX][TABLERO_FILAS];
    int     columnas;   /* ancho activo del tablero */
} tTablero;

/* =========================================================
 * API
 * ========================================================= */

/*
 * Establece el ancho activo y deja todas las celdas libres.
 * Usar en lugar de tablero_reiniciar cuando cambia el ancho.
 */
void tablero_inicializar(tTablero *tablero, int columnas);

/* Deja todas las celdas libres sin cambiar el ancho. */
void tablero_reiniciar(tTablero *tablero);

/*
 * Verifica si la forma dada puede colocarse en (columna_destino,
 * fila_destino) sin solapar celdas ocupadas ni salirse del área
 * del tablero (usa tablero->columnas como límite horizontal).
 * Retorna 1 si el movimiento es válido, 0 si colisiona.
 * Las filas negativas (por encima del tablero) son válidas.
 */
int tablero_movimiento_valido(const tTablero *tablero,
                              int columna_destino, int fila_destino,
                              const tFormaPieza forma);

/*
 * Versión circular para el modo DX.
 *
 * Los minos que excedan el límite horizontal (izquierda o derecha)
 * aparecen del otro lado del tablero. Solo se rechaza el movimiento
 * si un mino colisiona con una celda ya ocupada en la posición
 * envuelta, o si excede el límite vertical.
 * Las filas negativas (spawning) siguen siendo válidas.
 */
int tablero_movimiento_valido_dx(const tTablero *tablero,
                                 int columna_destino, int fila_destino,
                                 const tFormaPieza forma);

/*
 * Almacena la forma en el tablero usando 'color_paleta' como valor
 * de celda. Solo escribe celdas dentro del área visible.
 */
void tablero_fijar_pieza(tTablero *tablero,
                         int columna_destino, int fila_destino,
                         const tFormaPieza forma, uint8_t color_paleta);

/*
 * Versión circular para el modo DX.
 * Igual que tablero_fijar_pieza pero envuelve columnas fuera de rango.
 */
void tablero_fijar_pieza_dx(tTablero *tablero,
                             int columna_destino, int fila_destino,
                             const tFormaPieza forma, uint8_t color_paleta);

/*
 * Elimina todas las filas completamente llenas, baja las superiores
 * y retorna la cantidad de filas eliminadas (0 a 4).
 */
int tablero_eliminar_filas_completas(tTablero *tablero);

/*
 * Retorna 1 si alguna celda de la primera fila visible (fila 0) está
 * ocupada, lo que indica que el juego terminó.
 */
int tablero_hay_derrota(const tTablero *tablero);

#endif /* TABLERO_H */
