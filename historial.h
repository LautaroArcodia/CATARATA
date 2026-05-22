#ifndef HISTORIAL_H
#define HISTORIAL_H

/*
 * historial.h — Pantalla de historial de mejores puntajes
 *
 * Muestra los 10 jugadores con mejor puntaje del modo actual.
 * El modo se recibe al iniciar y determina qué archivo se carga.
 * ESC o ENTER regresan al menú.
 */

typedef enum
{
    HISTORIAL_CONTINUAR = 0,
    HISTORIAL_VOLVER
} ResultadoHistorial;

typedef struct
{
    int modo;   /* 0 = CLASICO, 1 = DX */
} PantallaHistorial;

void              historial_iniciar   (PantallaHistorial *h, int modo);
ResultadoHistorial historial_actualizar(PantallaHistorial *h);
void              historial_dibujar   (const PantallaHistorial *h, int ancho, int alto);
void              historial_destruir  (PantallaHistorial *h);

#endif /* HISTORIAL_H */
