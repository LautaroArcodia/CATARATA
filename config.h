#ifndef CONFIG_H
#define CONFIG_H

/*
 * Pantalla de configuración persistente
 *
 * Permite al jugador cambiar tres opciones que se guardan en disco:
 *   RESOLUCION → CGA (320×200) o VGA (640×480)
 *   PALETA     → Clásica / Neon / Retro
 *   VELOCIDAD  → niveles 1 al 5 (velocidad inicial de caída)
 *
 * Al salir (ESC), los cambios se guardan automáticamente en
 * "tetris_config.dat" para que persistan entre sesiones.
 */

#include "GBT/gbt.h"
#include "resolucion.h"

#define CANTIDAD_PALETAS 3 /* Cantidad de paletas disponibles */

typedef enum
{
    CONFIG_CONTINUAR = 0,
    CONFIG_VOLVER
} ResultadoConfig;

/* Ítems del menú de configuración */
typedef enum
{
    ITEM_CONFIG_RESOLUCION = 0,
    ITEM_CONFIG_PALETA     = 1,
    ITEM_CONFIG_VELOCIDAD  = 2,
    ITEM_CONFIG_ANCHO_DX   = 3,   /* ancho tablero modo DX (8-16) */
    ITEM_CONFIG_TOTAL      = 4
} eItemConfig;

typedef struct
{
    int                item_seleccionado;
    int                resolucion;         /* 0=CGA, 1=VGA           */
    int                paleta;             /* 0=Clasica, 1=Neon, 2=Retro */
    int                velocidad;          /* 0-4 → se muestra como 1-5  */
    int                ancho_dx;           /* ancho tablero DX: 8 a 16   */
    tGBT_Temporizador *timer_blink;
    int                mostrar_flechas;
} PantallaConfig;


void config_iniciar(PantallaConfig *c, eResolucion res_actual, int paleta_actual, int velocidad_actual, int ancho_dx_actual);

ResultadoConfig config_actualizar(PantallaConfig *c);

void config_dibujar(const PantallaConfig *c, int ancho, int alto);

void config_destruir(PantallaConfig *c);

eResolucion config_obtener_resolucion(const PantallaConfig *c);
int         config_obtener_paleta    (const PantallaConfig *c);
int         config_obtener_velocidad (const PantallaConfig *c);   /* devuelve 1-5 */
int         config_obtener_ancho_dx  (const PantallaConfig *c);   /* devuelve 8-16 */

#endif /* CONFIG_H */
