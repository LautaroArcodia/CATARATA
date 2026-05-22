#include "movimiento.h"

/* =========================================================
 * FUNCIONES MODO CLASICO
 * ========================================================= */

void movimiento_inicializar_pieza(tPiezaActiva *pieza, eTipoPieza tipo)
{
    pieza->tipo                 = tipo;
    pieza->rotaciones_aplicadas = 0;
    pieza->columna_tablero      = pieza_columna_inicial(tipo);
    pieza->fila_tablero         = pieza_fila_inicial(tipo);

    pieza_calcular_forma(tipo, 0, pieza->forma_actual);
}

int movimiento_mover_izquierda(tPiezaActiva *pieza, const tTablero *tablero)
{
    int nueva_columna = pieza->columna_tablero - 1;

    if (!tablero_movimiento_valido(tablero, nueva_columna, pieza->fila_tablero, pieza->forma_actual))
        return 0;

    pieza->columna_tablero = nueva_columna;
    return 1;
}

int movimiento_mover_derecha(tPiezaActiva *pieza, const tTablero *tablero)
{
    int nueva_columna = pieza->columna_tablero + 1;

    if (!tablero_movimiento_valido(tablero, nueva_columna, pieza->fila_tablero, pieza->forma_actual))
        return 0;

    pieza->columna_tablero = nueva_columna;
    return 1;
}

int movimiento_mover_abajo(tPiezaActiva *pieza, const tTablero *tablero)
{
    int nueva_fila = pieza->fila_tablero + 1;

    if (!tablero_movimiento_valido(tablero, pieza->columna_tablero, nueva_fila, pieza->forma_actual))
        return 0;

    pieza->fila_tablero = nueva_fila;
    return 1;
}

int movimiento_rotar_horario(tPiezaActiva *pieza, const tTablero *tablero)
{
    tFormaPieza forma_rotada;
    pieza_rotar_horario(pieza->forma_actual, forma_rotada);

    if (!tablero_movimiento_valido(tablero, pieza->columna_tablero, pieza->fila_tablero, forma_rotada))
        return 0;

    pieza_copiar_forma(forma_rotada, pieza->forma_actual);
    pieza->rotaciones_aplicadas = (pieza->rotaciones_aplicadas + 1) % 4;
    return 1;
}

int movimiento_rotar_antihorario(tPiezaActiva *pieza, const tTablero *tablero)
{
    tFormaPieza forma_rotada;
    pieza_rotar_antihorario(pieza->forma_actual, forma_rotada);

    if (!tablero_movimiento_valido(tablero, pieza->columna_tablero, pieza->fila_tablero, forma_rotada))
        return 0;

    pieza_copiar_forma(forma_rotada, pieza->forma_actual);
    pieza->rotaciones_aplicadas = (pieza->rotaciones_aplicadas + 3) % 4;
    return 1;
}

int movimiento_caida_instantanea(tPiezaActiva *pieza, const tTablero *tablero)
{
    int celdas_caidas = 0;

    while (tablero_movimiento_valido(tablero, pieza->columna_tablero, pieza->fila_tablero + 1, pieza->forma_actual))
    {
        pieza->fila_tablero++;
        celdas_caidas++;
    }

    return celdas_caidas;
}

int movimiento_puede_bajar(const tPiezaActiva *pieza, const tTablero *tablero)
{
    return tablero_movimiento_valido(tablero,
                                     pieza->columna_tablero,
                                     pieza->fila_tablero + 1,
                                     pieza->forma_actual);
}

/* =========================================================
 * FUNCIONES CON TABLERO CIRCULAR — MODO DX
 *
 * La diferencia clave respecto a las clásicas es el uso de
 * tablero_movimiento_valido_dx, que envuelve columnas en lugar
 * de rechazar el movimiento al llegar al borde.
 * ========================================================= */

int movimiento_mover_izquierda_dx(tPiezaActiva *pieza, const tTablero *tablero)
{
    int nueva_columna = pieza->columna_tablero - 1;

    if (!tablero_movimiento_valido_dx(tablero, nueva_columna, pieza->fila_tablero, pieza->forma_actual))
        return 0;

    pieza->columna_tablero = NORMALIZAR_COLUMNA(nueva_columna, tablero->columnas);
    return 1;
}

int movimiento_mover_derecha_dx(tPiezaActiva *pieza, const tTablero *tablero)
{
    int nueva_columna = pieza->columna_tablero + 1;

    if (!tablero_movimiento_valido_dx(tablero, nueva_columna, pieza->fila_tablero, pieza->forma_actual))
        return 0;

    pieza->columna_tablero = NORMALIZAR_COLUMNA(nueva_columna, tablero->columnas);
    return 1;
}

/*
 * El movimiento hacia abajo es idéntico al clásico: la envoltura
 * solo aplica horizontalmente, no verticalmente.
 */
int movimiento_mover_abajo_dx(tPiezaActiva *pieza, const tTablero *tablero)
{
    int nueva_fila = pieza->fila_tablero + 1;

    if (!tablero_movimiento_valido_dx(tablero, pieza->columna_tablero, nueva_fila, pieza->forma_actual))
        return 0;

    pieza->fila_tablero = nueva_fila;
    return 1;
}

int movimiento_rotar_horario_dx(tPiezaActiva *pieza,
                                 const tTablero *tablero)
{
    tFormaPieza forma_rotada;
    pieza_rotar_horario(pieza->forma_actual, forma_rotada);

    if (!tablero_movimiento_valido_dx(tablero, pieza->columna_tablero, pieza->fila_tablero, forma_rotada))
        return 0;

    pieza_copiar_forma(forma_rotada, pieza->forma_actual);
    pieza->rotaciones_aplicadas = (pieza->rotaciones_aplicadas + 1) % 4;
    return 1;
}

int movimiento_rotar_antihorario_dx(tPiezaActiva *pieza, const tTablero *tablero)
{
    tFormaPieza forma_rotada;
    pieza_rotar_antihorario(pieza->forma_actual, forma_rotada);

    if (!tablero_movimiento_valido_dx(tablero, pieza->columna_tablero, pieza->fila_tablero, forma_rotada))
        return 0;

    pieza_copiar_forma(forma_rotada, pieza->forma_actual);
    pieza->rotaciones_aplicadas = (pieza->rotaciones_aplicadas + 3) % 4;
    return 1;
}

int movimiento_caida_instantanea_dx(tPiezaActiva *pieza, const tTablero *tablero)
{
    int celdas_caidas = 0;

    while (tablero_movimiento_valido_dx(tablero, pieza->columna_tablero, pieza->fila_tablero + 1, pieza->forma_actual))
    {
        pieza->fila_tablero++;
        celdas_caidas++;
    }

    return celdas_caidas;
}

int movimiento_puede_bajar_dx(const tPiezaActiva *pieza, const tTablero *tablero)
{
    return tablero_movimiento_valido_dx(tablero,
                                        pieza->columna_tablero,
                                        pieza->fila_tablero + 1,
                                        pieza->forma_actual);
}
