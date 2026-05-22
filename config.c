/*
 * config.c — Pantalla de configuracion persistente
 *
 * Permite cambiar resolucion, paleta y velocidad.
 * Al salir con ESC, los cambios se guardan en disco.
 */

#include "config.h"
#include "config_archivo.h"
#include "dibujo_menu.h"
#include "dibujo.h"
#include "layout.h"
#include "GBT/gbt.h"
#include <stdio.h>

/* =========================================================
 * CONSTANTES VISUALES
 * ========================================================= */
#define SEG_BLINK   0.4

#define COLOR_SELECCIONADO  COLOR_PIEZA_I
#define COLOR_VALOR         COLOR_PIEZA_O
#define COLOR_TITULO        COLOR_PIEZA_T
#define COLOR_NORMAL        COLOR_GRIS_OSCURO

static const char *NOMBRES_RES[2] = {"CGA", "VGA"};
static const char *NOMBRES_PAL[3] = {"CLASICA", "NEON", "RETRO"};
static const char *NOMBRES_VEL[5] = {"1", "2", "3", "4", "5"};

static void reiniciar_parpadeo(PantallaConfig *c)
{
    c->mostrar_flechas = 1;
    gbt_temporizador_pausar(c->timer_blink);
    gbt_temporizador_reanudar(c->timer_blink);
}

/* =========================================================
 * API PUBLICA
 * ========================================================= */

/*
    Inicializa la pantalla con los valores actuales de la configuración desde el archivo.
*/
void config_iniciar(PantallaConfig *c, eResolucion res_actual, int paleta_actual, int velocidad_actual, int ancho_dx_actual)
{
    c->item_seleccionado = ITEM_CONFIG_RESOLUCION;
    c->resolucion        = (int)res_actual;
    c->paleta            = paleta_actual;
    c->velocidad         = velocidad_actual - 1;   /* interno: 0-4 */
    c->ancho_dx          = ancho_dx_actual;        /* 8 a 16       */
    c->timer_blink       = gbt_temporizador_crear(SEG_BLINK);
    c->mostrar_flechas   = 1;
}

/*
    Procesa entradas, actualiza el estado y guarda si el jugador sale.
*/
ResultadoConfig config_actualizar(PantallaConfig *c)
{
    int direccion;

    gbt_procesar_entrada();

    if (gbt_temporizador_consumir(c->timer_blink))
    {
        c->mostrar_flechas = !c->mostrar_flechas;
    }

    if (gbt_tecla_presionada(GBTK_ARRIBA))
    {
        c->item_seleccionado = (c->item_seleccionado - 1 + ITEM_CONFIG_TOTAL) % ITEM_CONFIG_TOTAL;
        reiniciar_parpadeo(c);
    }

    if (gbt_tecla_presionada(GBTK_ABAJO))
    {
        c->item_seleccionado = (c->item_seleccionado + 1) % ITEM_CONFIG_TOTAL;
        reiniciar_parpadeo(c);
    }

    if (gbt_tecla_presionada(GBTK_IZQUIERDA) || gbt_tecla_presionada(GBTK_DERECHA))
    {
        direccion = gbt_tecla_presionada(GBTK_DERECHA) ? 1 : -1;

        switch (c->item_seleccionado)
        {
            case ITEM_CONFIG_RESOLUCION:
                c->resolucion = (c->resolucion + direccion + 2) % 2;
                break;

            case ITEM_CONFIG_PALETA:
                c->paleta = (c->paleta + direccion + CANTIDAD_PALETAS) % CANTIDAD_PALETAS;
                break;

            case ITEM_CONFIG_VELOCIDAD:
                c->velocidad = (c->velocidad + direccion + 5) % 5;
                break;

            case ITEM_CONFIG_ANCHO_DX:
                c->ancho_dx += direccion;

                if (c->ancho_dx < 8)
                {
                    c->ancho_dx = 16;
                }

                if (c->ancho_dx > 16)
                {
                    c->ancho_dx = 8;
                }
                break;
        }
        reiniciar_parpadeo(c);
    }

    if (gbt_tecla_presionada(GBTK_ESCAPE))
    {
        config_archivo_guardar(c->resolucion, c->velocidad + 1, c->paleta, c->ancho_dx);
        return CONFIG_VOLVER;
    }

    return CONFIG_CONTINUAR;
}

/*
    Dibuja la pantalla completa.
*/
void config_dibujar(const PantallaConfig *c, int ancho, int alto)
{
    int centro_x = ancho / 2;
    int escala = ancho / 320;

    if (escala < 1)
    {
        escala = 1;
    }

    int y_titulo     = alto / 2     - 60 * escala;
    int y_sep_top    = y_titulo     + 12 * escala;
    int y_resolucion = y_sep_top    + 16 * escala;
    int y_paleta     = y_resolucion + 22 * escala;
    int y_velocidad  = y_paleta     + 22 * escala;
    int y_ancho_dx   = y_velocidad  + 22 * escala;
    int y_sep_bot    = y_ancho_dx   + 16 * escala;
    int y_esc        = y_sep_bot    + 10 * escala;

    dibujar_pared_ladrillos_completa();

    /* Panel oscuro */
    int margen   = ancho / 8;
    int panel_x1 = margen;
    int panel_x2 = ancho - margen;
    int panel_y1 = y_titulo - 6 * escala;
    int panel_y2 = y_esc + GLIFO_ALTO * escala + 6 * escala;

    rellenar_rectangulo(panel_x1, panel_y1, panel_x2, panel_y2, COLOR_FONDO_TABLERO);
    dibujar_marco_incrustado_en(panel_x1, panel_y1, panel_x2, panel_y2);

    /* Titulo */
    const char *titulo = "CONFIG";

    dm_texto_escalado(titulo,
                      centro_x - dm_ancho_texto(titulo, escala) / 2,
                      y_titulo,
                      COLOR_TITULO,
                      escala);

    dm_separador(ancho, y_sep_top);

    /* Filas de configuracion */
    dm_fila_opcion(centro_x,
                    y_resolucion,
                    "RESOLUCION",
                    NOMBRES_RES[c->resolucion],
                    c->item_seleccionado == ITEM_CONFIG_RESOLUCION,
                    c->mostrar_flechas,
                    escala);

    dm_fila_opcion(centro_x,
                    y_paleta,
                    "PALETA",
                    NOMBRES_PAL[c->paleta],
                    c->item_seleccionado == ITEM_CONFIG_PALETA,
                    c->mostrar_flechas,
                    escala);

    dm_fila_opcion(centro_x,
                    y_velocidad,
                    "VELOCIDAD",
                    NOMBRES_VEL[c->velocidad],
                    c->item_seleccionado == ITEM_CONFIG_VELOCIDAD,
                    c->mostrar_flechas,
                    escala);

    /* Ancho tablero DX — se muestra el numero de columnas (8-16) */
    char texto_ancho[4];
    sprintf(texto_ancho, "%d", c->ancho_dx);

    dm_fila_opcion(centro_x,
                    y_ancho_dx,
                    "ANCHO DX",
                    texto_ancho,
                    c->item_seleccionado == ITEM_CONFIG_ANCHO_DX,
                    c->mostrar_flechas,
                    escala);

    dm_separador(ancho, y_sep_bot);

    /* Instruccion de pie */
    const char *pie = "ESC : GUARDAR Y VOLVER";

    dm_texto_escalado(pie,
                      centro_x - dm_ancho_texto(pie, escala) / 2,
                      y_esc,
                      COLOR_NORMAL,
                      escala);

    gbt_volcar_backbuffer();
}

/*
    Libera recursos.
*/
void config_destruir(PantallaConfig *c)
{
    gbt_temporizador_destruir(c->timer_blink);
    c->timer_blink = NULL;
}

eResolucion config_obtener_resolucion(const PantallaConfig *c)
{
    return (eResolucion)c->resolucion;
}

int config_obtener_paleta(const PantallaConfig *c)
{
    return c->paleta;
}

int config_obtener_velocidad(const PantallaConfig *c)
{
    return c->velocidad + 1;   /* devuelve 1-5 */
}

int config_obtener_ancho_dx(const PantallaConfig *c)
{
    return c->ancho_dx;   /* devuelve 8-16 */
}
