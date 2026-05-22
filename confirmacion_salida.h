#ifndef CONFIRMACION_SALIDA_H
#define CONFIRMACION_SALIDA_H

/*
 * confirmacion_salida.h — Pantalla de confirmación al salir con ESC
 *
 * Se muestra cuando el jugador pulsa ESC durante la partida.
 * Overlay estilo game over (oscurecido + panel con marco) con
 * opciones navegables estilo pausa (flechas + ENTER).
 *
 * Teclas:
 *   ARRIBA / ABAJO  → navegar entre opciones
 *   ENTER           → confirmar la opción seleccionada
 *   ESC             → cancelar y volver al juego
 */

#include "GBT/gbt.h"

typedef enum
{
    CONF_SALIDA_CONTINUAR = 0,
    CONF_SALIDA_GUARDAR_Y_SALIR,
    CONF_SALIDA_SALIR_SIN_GUARDAR,
    CONF_SALIDA_CANCELAR
} eResultadoConfirmacion;

/* Índices de opciones */
#define CONF_OPCION_GUARDAR_Y_SALIR    0
#define CONF_OPCION_SALIR_SIN_GUARDAR  1
#define CONF_OPCION_CANCELAR           2
#define CONF_OPCION_TOTAL              3

typedef struct
{
    tGBT_Temporizador *temporizador_parpadeo;
    int                mostrar_texto;
    int                opcion_seleccionada;
} tPantallaConfirmacion;

void                   confirmacion_salida_iniciar   (tPantallaConfirmacion *c);
eResultadoConfirmacion confirmacion_salida_actualizar(tPantallaConfirmacion *c);
void                   confirmacion_salida_destruir  (tPantallaConfirmacion *c);
void                   confirmacion_salida_dibujar   (const tPantallaConfirmacion *c);

#endif /* CONFIRMACION_SALIDA_H */
