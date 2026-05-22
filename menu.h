#ifndef MENU_H
#define MENU_H

/*
 * menu.h — Menú principal del juego
 *
 * Opciones del menú:
 *   MODO      → Clásico o DX
 *   HISTORIAL → muestra el top 10 del modo seleccionado
 *   OPCIONES  → navega a la pantalla de configuración (config.h)
 *
 * La velocidad inicial se lee desde la configuración guardada (config.h)
 * y no se muestra aquí para mantener el menú simple.
 */

#include "GBT/gbt.h"
#include <stdint.h>

typedef enum
{
    MENU_CONTINUAR = 0,
    MENU_JUGAR,
    MENU_CONTINUAR_PARTIDA,   /* ENTER en CONTINUAR → cargar savegame */
    MENU_HISTORIAL,       /* ENTER en HISTORIAL → abrir historial */
    MENU_CONFIGURACION,   /* ENTER en OPCIONES  → abrir config   */
    MENU_VOLVER
} ResultadoMenu;

typedef enum
{
    MODO_CLASICO = 0,
    MODO_DX      = 1
} eModo;

typedef enum
{
    ITEM_MODO            = 0,
    ITEM_CONTINUAR_JUEGO = 1,
    ITEM_HISTORIAL       = 2,
    ITEM_OPCIONES        = 3,
    ITEM_TOTAL           = 4
} eMenuItem;

typedef struct
{
    int                item_sel;
    int                modo;
    tGBT_Temporizador *temporizador_parpadeo;
    int                mostrar_flechas;
} PantallaMenu;

void          menu_iniciar   (PantallaMenu *m);
ResultadoMenu menu_actualizar(PantallaMenu *m);
void          menu_dibujar   (const PantallaMenu *m, int ancho, int alto);
void          menu_destruir  (PantallaMenu *m);

#endif /* MENU_H */
