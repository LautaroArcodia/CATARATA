#ifndef JUEGO_LOGICA_INTERNA_H
#define JUEGO_LOGICA_INTERNA_H

/*
 * juego_logica_interna.h
 *
 * Funciones compartidas que usan los archivos de cada modo.
 * No es un header público: solo lo incluyen juego_logica_clasico.c
 * y juego_logica_dx.c.
 */

#include "juego.h"

/* Fija la pieza en el tablero, elimina líneas y genera la siguiente. */
ResultadoJuego fijar_pieza_y_continuar(tEstadoJuego *estado);

/* Reinicia el temporizador de fijación (lock delay). */
void reiniciar_temporizador_fijacion(tEstadoJuego *estado);

/*
 * Procesa el cheat de puntos (tecla C).
 * Si la tecla C fue presionada, suma 1000 puntos al puntaje.
 */
void procesar_cheat_puntos(tEstadoJuego *estado);

/* ── Acceso al estado de la bolsa de piezas (para savegame) ─── */

/* Copia el estado actual de la bolsa a los buffers proporcionados. */
void bolsa_obtener_estado(int *bolsa_out, int *tam_out, int *indice_out);

/* Restaura el estado de la bolsa desde los datos proporcionados. */
void bolsa_restaurar_estado(const int *bolsa_in, int tam, int indice);

#endif /* JUEGO_LOGICA_INTERNA_H */
