#ifndef ARGS_H
#define ARGS_H

/*
 * args.h — Argumentos de línea de comandos
 *
 * El juego acepta los siguientes argumentos opcionales:
 *
 *   -r CGA|VGA   Resolución lógica (por defecto: CGA)
 *   -e N         Factor de escala de ventana, N entre 1 y 4 (por defecto: 2)
 *
 * Ejemplos:
 *   tetris                    → CGA a escala 2 (ventana 640×400)
 *   tetris -r VGA             → VGA a escala 1 (ventana 640×480)
 *   tetris -r CGA -e 3        → CGA a escala 3 (ventana 960×600)
 *   tetris -e 1 -r VGA        → el orden de los argumentos no importa
 */

#include "resolucion.h"

typedef struct
{
    eResolucion resolucion;   /* resolución lógica elegida */
    int         escala;       /* factor de escala (1-4), o 0 si no se especificó (usar default) */
} tArgumentos;

tArgumentos args_parsear(int argc, char *argv[]);

#endif /* ARGS_H */
