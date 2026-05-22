#ifndef MOVIMIENTO_H
#define MOVIMIENTO_H

/*
 * movimiento.h — Movimientos de la pieza activa
 *
 * Este módulo maneja todos los movimientos posibles de la pieza
 * que está cayendo: izquierda, derecha, abajo, rotaciones y caída
 * instantánea. Cada función valida el movimiento contra el tablero
 * antes de aplicarlo.
 *
 * Las funciones con sufijo _dx son para el modo DX: usan validación
 * circular (los minos que salen por un borde aparecen del otro lado).
 * En el modo DX, 'columna_tablero' puede quedar fuera del rango
 * [0, tablero->columnas) cuando la pieza envuelve horizontalmente.
 * El módulo de dibujo debe tener esto en cuenta al renderizar.
 */

#include "pieza.h"
#include "tablero.h"

/* Estado completo de la pieza que está cayendo en el tablero */
typedef struct {
    eTipoPieza  tipo;                 /* qué tipo de pieza es (I, O, T...) */
    int         rotaciones_aplicadas; /* cantidad de giros realizados (0-3) */
    int         columna_tablero;      /* columna del tablero donde está la col 0 de la matriz */
    int         fila_tablero;         /* fila del tablero donde está la fila 0 de la matriz */
    tFormaPieza forma_actual;         /* forma actual con las rotaciones aplicadas */
} tPiezaActiva;

/* ── Funciones clásicas (Modo Clásico) ──────────────────────────── */

/* Coloca la pieza en su posición y forma iniciales (spawn). */
void movimiento_inicializar_pieza(tPiezaActiva *pieza, eTipoPieza tipo);

int movimiento_mover_izquierda    (tPiezaActiva *pieza, const tTablero *tablero);
int movimiento_mover_derecha      (tPiezaActiva *pieza, const tTablero *tablero);
int movimiento_mover_abajo        (tPiezaActiva *pieza, const tTablero *tablero);
int movimiento_rotar_horario      (tPiezaActiva *pieza, const tTablero *tablero);
int movimiento_rotar_antihorario  (tPiezaActiva *pieza, const tTablero *tablero);
int movimiento_caida_instantanea  (tPiezaActiva *pieza, const tTablero *tablero);
int movimiento_puede_bajar        (const tPiezaActiva *pieza, const tTablero *tablero);

/* ── Funciones con tablero circular (Modo DX) ───────────────────── */

/*
 * movimiento_mover_izquierda_dx / movimiento_mover_derecha_dx
 *
 * Si el movimiento haría que la pieza salga del tablero, el valor de
 * columna_tablero se ajusta al lado opuesto del tablero para que cada
 * mino quede envuelto correctamente.
 */
int movimiento_mover_izquierda_dx  (tPiezaActiva *pieza, const tTablero *tablero);
int movimiento_mover_derecha_dx    (tPiezaActiva *pieza, const tTablero *tablero);

/* El movimiento vertical no varía en DX (no hay envoltura vertical). */
int movimiento_mover_abajo_dx      (tPiezaActiva *pieza, const tTablero *tablero);

/*
 * Rotaciones DX: usan validación circular.
 * Si la pieza rotada entra en colisión con celdas ocupadas (no con
 * los bordes), la rotación se rechaza.
 */
int movimiento_rotar_horario_dx    (tPiezaActiva *pieza, const tTablero *tablero);
int movimiento_rotar_antihorario_dx(tPiezaActiva *pieza, const tTablero *tablero);

int movimiento_caida_instantanea_dx(tPiezaActiva *pieza, const tTablero *tablero);
int movimiento_puede_bajar_dx      (const tPiezaActiva *pieza, const tTablero *tablero);

#endif /* MOVIMIENTO_H */
