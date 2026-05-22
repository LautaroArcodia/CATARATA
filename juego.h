#ifndef JUEGO_H
#define JUEGO_H

/*
 * juego.h — Estado completo de una partida
 *
 * tEstadoJuego contiene todo lo necesario para representar una
 * partida en curso: tablero, pieza activa, puntaje, velocidad y
 * el nombre del jugador.
 *
 * El campo 'modo' indica qué modo está corriendo:
 *   0 = Clásico  — tablero 10×20, 7 piezas, bordes bloqueantes.
 *   1 = DX       — tablero de ancho variable, 11 piezas, tablero circular.
 *
 * El campo 'param' es un dato extra que puede usar cada modo:
 *   En DX: es el ancho del tablero (8 a 16 columnas).
 *   En Clásico: no se usa.
 */

#include "GBT/gbt.h"
#include "tablero.h"
#include "movimiento.h"
#include "confirmacion_salida.h"

#define PUNTOS_POR_CELDA_MANUAL       1
#define PUNTOS_POR_CELDA_INSTANTANEA  2

typedef enum
{
    JUEGO_CONTINUAR      = 0,
    JUEGO_TERMINADO      = 1,
    JUEGO_VOLVER         = 2,
    JUEGO_PAUSAR         = 3,
    JUEGO_PEDIR_GUARDAR  = 4
} ResultadoJuego;

typedef struct
{
    /* ── Modo activo ─────────────────────────────────── */
    int modo;    /* 0 = Clásico, 1 = DX */
    int param;   /* parámetro del modo (en DX: ancho del tablero) */

    /* ── Tablero y piezas ────────────────────────────── */
    tTablero     tablero;
    tPiezaActiva pieza_activa;
    eTipoPieza   tipo_pieza_siguiente;
    tFormaPieza  forma_pieza_siguiente;

    /* ── Temporizadores ──────────────────────────────── */
    tGBT_Temporizador *temporizador_caida_automatica;
    tGBT_Temporizador *temporizador_movimiento_lateral;
    tGBT_Temporizador *temporizador_fijacion;

    /* ── Puntaje y progreso ───────────────────────────  */
    int puntuacion;
    int cantidad_lineas_eliminadas;
    int cantidad_piezas_fijadas;
    int estadisticas_piezas[CANTIDAD_TIPOS_DX];

    /* ── Velocidad de caída ───────────────────────────  */
    int    nivel_velocidad_inicial;
    double intervalo_caida_ms;

    /* ── Lock delay ───────────────────────────────────  */
    int pieza_en_suelo;

    /* ── Jugador ─────────────────────────────────────── */
    char nombre_jugador[11];
} tEstadoJuego;

void           juego_iniciar(tEstadoJuego *estado, int velocidad_nivel, const char *nombre_jugador, int modo, int param);
ResultadoJuego juego_actualizar(tEstadoJuego *estado);
void           juego_dibujar(const tEstadoJuego *estado, int ancho, int alto);
void           juego_destruir(tEstadoJuego *estado);
void           juego_dibujar_pantalla_game_over(const tEstadoJuego *estado, int mostrar_instrucciones);
void           juego_dibujar_pantalla_confirmacion(const tEstadoJuego *estado, const tPantallaConfirmacion *c);

#endif /* JUEGO_H */
