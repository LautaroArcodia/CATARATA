#ifndef GAMEOVER_H
#define GAMEOVER_H

/*
 * gameover.h — Pantalla de Game Over
 *
 * Maneja el estado visual (parpadeo), la lectura de teclas y
 * el dibujado del panel central con puntaje actual y mejor puntaje.
 */

#include "GBT/gbt.h"

typedef enum
{
    GAMEOVER_CONTINUAR,
    GAMEOVER_IR_AL_MENU,
    GAMEOVER_IR_A_PRESENTACION
} eResultadoGameOver;

typedef struct
{
    tGBT_Temporizador *temporizador_parpadeo;
    int                mostrar_instrucciones;
} tPantallaGameOver;

void               gameover_iniciar   (tPantallaGameOver *g);
eResultadoGameOver gameover_actualizar(tPantallaGameOver *g);
void               gameover_destruir  (tPantallaGameOver *g);

/*
 * Dibuja el panel de Game Over sobre lo que ya esté en pantalla.
 */
void gameover_dibujar(int puntuacion, int mejor_puntaje, int mostrar_instrucciones);

#endif /* GAMEOVER_H */
