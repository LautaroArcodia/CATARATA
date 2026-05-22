#ifndef INGRESO_NOMBRE_H
#define INGRESO_NOMBRE_H

/*
 * ingreso_nombre.h — Pantalla de ingreso de nombre del jugador
 *
 * Muestra un campo de texto gráfico donde el jugador puede escribir
 * su nombre usando el teclado. Solo acepta letras (A-Z). El nombre
 * se guarda en mayúsculas y tiene un largo máximo de 10 caracteres.
 *
 * Teclas:
 *   A–Z          → agregar letra al nombre
 *   RETROCESO    → borrar la última letra
 *   ENTER        → confirmar (solo si el nombre tiene al menos 1 letra)
 *   ESCAPE       → usar nombre por defecto "JUGADOR"
 */

#include "GBT/gbt.h"

#define NOMBRE_MAX_LONGITUD  10            /* máximo de letras en el nombre */
#define NOMBRE_POR_DEFECTO   "JUGADOR"    /* usado si el jugador presiona ESC */

typedef enum
{
    INGRESO_NOMBRE_CONTINUAR = 0,   /* el jugador todavía está escribiendo */
    INGRESO_NOMBRE_CONFIRMAR        /* ENTER presionado, nombre listo       */
} ResultadoIngresoNombre;

typedef struct
{
    char               texto[NOMBRE_MAX_LONGITUD + 1]; /* nombre actual + '\0' */
    int                longitud;                        /* cantidad de letras ingresadas */
    tGBT_Temporizador *timer_cursor;                   /* parpadeo del cursor           */
    int                mostrar_cursor;                  /* 1 = cursor visible            */
} PantallaIngresoNombre;

/* Inicializa la pantalla con el campo de texto vacío. */
void ingreso_nombre_iniciar(PantallaIngresoNombre *pantalla);

/* Procesa entradas y actualiza el estado del campo de texto. */
ResultadoIngresoNombre ingreso_nombre_actualizar(PantallaIngresoNombre *pantalla);

/* Dibuja la pantalla completa (fondo + panel + campo de texto). */
void ingreso_nombre_dibujar(const PantallaIngresoNombre *pantalla,
                             int ancho, int alto);

/* Libera recursos del temporizador. */
void ingreso_nombre_destruir(PantallaIngresoNombre *pantalla);

/*
 * Retorna un puntero al nombre ingresado (string terminado en '\0').
 * El puntero apunta al buffer interno de la estructura.
 */
const char *ingreso_nombre_obtener(const PantallaIngresoNombre *pantalla);

#endif /* INGRESO_NOMBRE_H */
