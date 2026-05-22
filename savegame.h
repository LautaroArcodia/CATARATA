#ifndef SAVEGAME_H
#define SAVEGAME_H

/*
 * savegame.h — Guardado y carga de partida
 *
 * Permite guardar el estado completo de una partida en curso
 * a un archivo binario ("tetris_savegame.dat") y restaurarlo
 * más tarde para continuar jugando.
 *
 * El archivo se sobreescribe cada vez que se guarda.
 * Se recomienda borrarlo cuando la partida termina (game over)
 * para evitar cargar un estado obsoleto.
 *
 * Funciona igual que config_archivo.c: abre el archivo,
 * escribe/lee los datos, cierra el archivo.
 */

#include "juego.h"

#define SAVEGAME_ARCHIVO "tetris_savegame.dat"

/* Guarda el estado completo de la partida al archivo. Retorna 1 si tuvo éxito. */
int savegame_guardar(const tEstadoJuego *estado);

/* Carga el estado desde el archivo y recrea los temporizadores. Retorna 1 si tuvo éxito. */
int savegame_cargar(tEstadoJuego *estado);

/* Retorna 1 si existe un archivo de partida guardada. */
int savegame_existe(void);

/* Borra el archivo de partida guardada (por ejemplo, tras game over). */
void savegame_borrar(void);

#endif /* SAVEGAME_H */
