#include "confirmacion_salida.h"
#include "dibujo.h"
#include "dibujo_menu.h"
#include "layout.h"
#include "GBT/gbt.h"

#define INTERVALO_PARPADEO      0.5
#define MARGEN_CONTENIDO        8
#define MARGEN_SEPARADOR        5
#define ESPACIADO_BASE         10
#define ESPACIO_ENTRE_OPCIONES 14

#define COLOR_TITULO  COLOR_PIEZA_O    /* amarillo */
#define COLOR_AYUDA   COLOR_GRIS_OSCURO

/* Oscurece la pantalla con un patrón de ajedrez semi-transparente. */
static void dibujar_overlay(void)
{
    for (int y = 0; y < PANTALLA_ALTO; y++)
    {
        for (int x = 0; x < PANTALLA_ANCHO; x++)
        {
            if ((x + y) % 2 == 0)
                gbt_dibujar_pixel((uint16_t)x, (uint16_t)y, COLOR_NEGRO);
        }
    }
}

/* Dibuja una línea horizontal de separación dentro del panel. */
static void dibujar_separador_conf(int y)
{
    for (int x = SALIR_PANEL_X1 + MARGEN_SEPARADOR; x <= SALIR_PANEL_X2 - MARGEN_SEPARADOR; x++)
    {
        dibujar_pixel(x, y, COLOR_MORTERO);
    }
}

/* =========================================================
 * API PÚBLICA
 * ========================================================= */

void confirmacion_salida_iniciar(tPantallaConfirmacion *c)
{
    c->temporizador_parpadeo = gbt_temporizador_crear(INTERVALO_PARPADEO);
    c->mostrar_texto         = 1;
    c->opcion_seleccionada   = CONF_OPCION_GUARDAR_Y_SALIR;
}

eResultadoConfirmacion confirmacion_salida_actualizar(tPantallaConfirmacion *c)
{
    gbt_procesar_entrada();

    if (gbt_temporizador_consumir(c->temporizador_parpadeo))
        c->mostrar_texto = !c->mostrar_texto;

    if (gbt_tecla_presionada(GBTK_ESCAPE))
        return CONF_SALIDA_CANCELAR;

    if (gbt_tecla_presionada(GBTK_ARRIBA))
        c->opcion_seleccionada = (c->opcion_seleccionada - 1 + CONF_OPCION_TOTAL) % CONF_OPCION_TOTAL;

    if (gbt_tecla_presionada(GBTK_ABAJO))
        c->opcion_seleccionada = (c->opcion_seleccionada + 1) % CONF_OPCION_TOTAL;

    if (gbt_tecla_presionada(GBTK_ENTER))
    {
        if (c->opcion_seleccionada == CONF_OPCION_GUARDAR_Y_SALIR)
            return CONF_SALIDA_GUARDAR_Y_SALIR;

        if (c->opcion_seleccionada == CONF_OPCION_SALIR_SIN_GUARDAR)
            return CONF_SALIDA_SALIR_SIN_GUARDAR;

        if (c->opcion_seleccionada == CONF_OPCION_CANCELAR)
            return CONF_SALIDA_CANCELAR;
    }

    return CONF_SALIDA_CONTINUAR;
}

void confirmacion_salida_destruir(tPantallaConfirmacion *c)
{
    gbt_temporizador_destruir(c->temporizador_parpadeo);
    c->temporizador_parpadeo = NULL;
}

void confirmacion_salida_dibujar(const tPantallaConfirmacion *c)
{
    const int centro_x = (SALIR_PANEL_X1 + SALIR_PANEL_X2) / 2;
    const int escala   = PANTALLA_ANCHO / 320;

    const int y_titulo = SALIR_PANEL_Y1 + MARGEN_CONTENIDO * escala;
    const int y_sep1   = y_titulo + GLIFO_ALTO * SALIR_ESCALA_TEXTO + ESPACIADO_BASE * escala;
    const int y_op1    = y_sep1   + ESPACIADO_BASE * escala;
    const int y_op2    = y_op1    + ESPACIO_ENTRE_OPCIONES * escala;
    const int y_op3    = y_op2    + ESPACIO_ENTRE_OPCIONES * escala;
    const int y_sep2   = y_op3    + ESPACIADO_BASE * escala;
    const int y_ayuda  = y_sep2   + ESPACIADO_BASE * escala;

    dibujar_overlay();

    rellenar_rectangulo(SALIR_PANEL_X1,
                        SALIR_PANEL_Y1,
                        SALIR_PANEL_X2,
                        SALIR_PANEL_Y2,
                        COLOR_FONDO_TABLERO);

    dibujar_marco_incrustado_en(SALIR_PANEL_X1,
                                SALIR_PANEL_Y1,
                                SALIR_PANEL_X2,
                                SALIR_PANEL_Y2);

    texto_centrado_escalado("SALIR DEL JUEGO",
                            centro_x,
                            y_titulo,
                            COLOR_TITULO,
                            SALIR_ESCALA_TEXTO);

    dibujar_separador_conf(y_sep1);

    dm_boton(centro_x,
             y_op1,
             "GUARDAR Y SALIR",
             c->opcion_seleccionada == CONF_OPCION_GUARDAR_Y_SALIR,
             escala);

    dm_boton(centro_x,
             y_op2,
             "SALIR SIN GUARDAR",
             c->opcion_seleccionada == CONF_OPCION_SALIR_SIN_GUARDAR,
             escala);

    dm_boton(centro_x,
             y_op3,
             "CANCELAR",
             c->opcion_seleccionada == CONF_OPCION_CANCELAR,
             escala);

    dibujar_separador_conf(y_sep2);

    if (c->mostrar_texto)
    {
        const char *ayuda = "ENTER = CONFIRMAR";
        dm_texto_escalado(ayuda,
                          centro_x - dm_ancho_texto(ayuda, escala) / 2,
                          y_ayuda,
                          COLOR_AYUDA,
                          escala);
    }
}
