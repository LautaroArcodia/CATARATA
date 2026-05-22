#ifndef PIEZA_H
#define PIEZA_H

#include <stdint.h>

/* =========================================================
 * CONSTANTES
 * ========================================================= */
#define DIMENSION_MATRIZ       5
#define CANTIDAD_TIPOS         7    /* piezas clásicas (I J L O S T Z)   */
#define CANTIDAD_TIPOS_DX     11    /* clásicas + piezas exclusivas DX    */

/* =========================================================
 * TIPOS DE PIEZAS
 *
 * Los primeros 7 son los tetrominos clásicos del Tetris.
 * Los 4 restantes son exclusivos del modo DX.
 * ========================================================= */
typedef enum
{
    TIPO_I    = 0,
    TIPO_O    = 1,
    TIPO_T    = 2,
    TIPO_S    = 3,
    TIPO_Z    = 4,
    TIPO_L    = 5,
    TIPO_J    = 6,
    /* ── Piezas exclusivas del modo DX ─────────────── */
    TIPO_X    = 7,   /* x : 1 solo mino                  */
    TIPO_C    = 8,   /* c : 5 minos en forma de C         */
    TIPO_P    = 9,   /* p : 5 minos en forma de P         */
    TIPO_CRUZ = 10   /* * : 5 minos en forma de cruz (+)  */
} eTipoPieza;

/* =========================================================
 * FORMA DE PIEZA
 * Grilla de DIMENSION_MATRIZ × DIMENSION_MATRIZ.
 * forma[fila][columna] : 0 = vacío / 1 = bloque
 * ========================================================= */
typedef uint8_t tFormaPieza[DIMENSION_MATRIZ][DIMENSION_MATRIZ];

/* =========================================================
 * API
 * ========================================================= */

/* Copia celda a celda 'origen' en 'destino'. */
void pieza_copiar_forma(const tFormaPieza origen, tFormaPieza destino);

/* Rota 'entrada' 90° en sentido HORARIO y guarda en 'salida'. */
void pieza_rotar_horario(const tFormaPieza entrada, tFormaPieza salida);

/* Rota 'entrada' 90° en sentido ANTIHORARIO y guarda en 'salida'. */
void pieza_rotar_antihorario(const tFormaPieza entrada, tFormaPieza salida);

/* Aplica num_rotaciones rotaciones horarias sobre la forma base del tipo. */
void pieza_calcular_forma(eTipoPieza tipo, int num_rotaciones,
                          tFormaPieza destino);

int     pieza_columna_inicial     (eTipoPieza tipo);
int     pieza_fila_inicial        (eTipoPieza tipo);
uint8_t pieza_obtener_color_paleta(eTipoPieza tipo);

#endif /* PIEZA_H */
