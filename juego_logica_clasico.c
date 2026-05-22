#include "juego_logica_clasico.h"
#include "juego_logica_interna.h"
#include "movimiento.h"
#include "GBT/gbt.h"

/*
 * Modo Clásico
 *
 * Contiene la lógica de un frame del modo clásico.
 */

ResultadoJuego clasico_actualizar(tEstadoJuego *estado)
{
    gbt_procesar_entrada();

    if (gbt_tecla_presionada(GBTK_p))      return JUEGO_PAUSAR;
    if (gbt_tecla_presionada(GBTK_ESCAPE)) return JUEGO_PEDIR_GUARDAR;

    /* Cheat: tecla C suma 1000 puntos */
    procesar_cheat_puntos(estado);

    int movio_la_pieza = 0;

    /* Rotaciones */
    if (gbt_tecla_presionada(GBTK_z) || gbt_tecla_presionada(GBTK_ARRIBA))
    {
        if (movimiento_rotar_horario(&estado->pieza_activa, &estado->tablero))
        {
            movio_la_pieza = 1;
        }
    }

    if (gbt_tecla_presionada(GBTK_a))
    {
        if (movimiento_rotar_antihorario(&estado->pieza_activa, &estado->tablero))
        {
            movio_la_pieza = 1;
        }
    }

    /* Caída instantánea */
    if (gbt_tecla_presionada(GBTK_x))
    {
        int celdas = movimiento_caida_instantanea(&estado->pieza_activa, &estado->tablero);

        estado->puntuacion += celdas * PUNTOS_POR_CELDA_INSTANTANEA;

        return fijar_pieza_y_continuar(estado);
    }

    /* Movimiento lateral con auto-repetición */
    int hay_repeticion   = gbt_temporizador_consumir(estado->temporizador_movimiento_lateral);

    int quiere_izquierda = gbt_tecla_presionada(GBTK_IZQUIERDA) || (gbt_tecla_sostenida(GBTK_IZQUIERDA) && hay_repeticion);
    int quiere_derecha   = gbt_tecla_presionada(GBTK_DERECHA) || (gbt_tecla_sostenida(GBTK_DERECHA) && hay_repeticion);
    int quiere_abajo     = gbt_tecla_presionada(GBTK_ABAJO) || (gbt_tecla_sostenida(GBTK_ABAJO) && hay_repeticion);

    if (quiere_izquierda)
    {
        if (movimiento_mover_izquierda(&estado->pieza_activa, &estado->tablero))
        {
            movio_la_pieza = 1;
        }
    }

    if (quiere_derecha)
    {
        if (movimiento_mover_derecha(&estado->pieza_activa, &estado->tablero))
        {
            movio_la_pieza = 1;
        }
    }

    if (quiere_abajo)
    {
        if (movimiento_mover_abajo(&estado->pieza_activa, &estado->tablero))
        {
            estado->puntuacion += PUNTOS_POR_CELDA_MANUAL;
            movio_la_pieza = 1;
        }
    }

    /* Lock delay */
    int puede_bajar = movimiento_puede_bajar(&estado->pieza_activa, &estado->tablero);

    if (puede_bajar)
    {
        estado->pieza_en_suelo = 0;
    }
    else
    {
        if (!estado->pieza_en_suelo)
        {
            estado->pieza_en_suelo = 1;
            reiniciar_temporizador_fijacion(estado);
        }
        else if (movio_la_pieza)
        {
            reiniciar_temporizador_fijacion(estado);
        }
    }

    /* Caída automática */
    if (gbt_temporizador_consumir(estado->temporizador_caida_automatica))
    {
        if (puede_bajar)
        {
            movimiento_mover_abajo(&estado->pieza_activa, &estado->tablero);
        }
    }

    /* Fijación automática (lock delay) */
    if (estado->pieza_en_suelo && gbt_temporizador_consumir(estado->temporizador_fijacion))
    {
        return fijar_pieza_y_continuar(estado);
    }

    return JUEGO_CONTINUAR;
}
