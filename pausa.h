#ifndef PAUSA_H
#define PAUSA_H

/*
 * pausa.h — Pantalla de pausa
 *
 * Muestra una pantalla simple que indica que el juego está pausado.
 * El estado del juego se preserva en memoria mientras se está pausado.
 *
 * Teclas:
 *   P        → reanudar el juego
 *   ESCAPE   → volver al menú (abandona la partida)
 */

#include "GBT/gbt.h"

typedef enum
{
    PAUSA_CONTINUAR = 0,   /* el jugador sigue pausado         */
    PAUSA_REANUDAR,        /* volver al juego                  */
    PAUSA_GUARDAR,         /* guardar partida al archivo       */
    PAUSA_IR_AL_MENU       /* abandonar partida → menú         */
} ResultadoPausa;

/* Opciones del menú de pausa */
#define PAUSA_OPCION_REANUDAR  0
#define PAUSA_OPCION_GUARDAR   1
#define PAUSA_OPCION_MENU      2
#define PAUSA_OPCION_TOTAL     3

typedef struct
{
    tGBT_Temporizador *temporizador_parpadeo;
    int                mostrar_texto;
    int                opcion_seleccionada;   /* 0=REANUDAR, 1=GUARDAR, 2=MENU */
} PantallaPausa;

void           pausa_iniciar   (PantallaPausa *pantalla);
ResultadoPausa pausa_actualizar(PantallaPausa *pantalla);
void           pausa_dibujar   (const PantallaPausa *pantalla, int ancho, int alto);
void           pausa_destruir  (PantallaPausa *pantalla);

#endif /* PAUSA_H */
