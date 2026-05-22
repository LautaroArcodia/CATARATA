#include "pausa.h"
#include "dibujo.h"
#include "dibujo_menu.h"
#include "layout.h"
#include "GBT/gbt.h"

/*
 * Pantalla de pausa
 *
 * Muestra un panel centrado con "PAUSADO" y las instrucciones.
 * El fondo de la pantalla es el fondo de ladrillos (el estado del
 * tablero no se dibuja debajo para mantener la implementación simple).
 */

 #define INTERVALO_PARPADEO_SEG  0.5

/* Colores */
#define COLOR_TITULO  COLOR_PIEZA_O     /* amarillo */
#define COLOR_AYUDA   COLOR_GRIS_OSCURO /* gris     */

/* Espaciados verticales */
#define MARGEN_SUPERIOR_PANEL      8
#define MARGEN_INFERIOR_PANEL      10
#define ESPACIO_TITULO_OPCIONES    18
#define ESPACIO_ENTRE_OPCIONES     14

void pausa_iniciar(PantallaPausa *pantalla)
{
    pantalla->temporizador_parpadeo = gbt_temporizador_crear(INTERVALO_PARPADEO_SEG);
    pantalla->mostrar_texto         = 1;
    pantalla->opcion_seleccionada   = PAUSA_OPCION_REANUDAR;
}

ResultadoPausa pausa_actualizar(PantallaPausa *pantalla)
{
    gbt_procesar_entrada();

    if (gbt_temporizador_consumir(pantalla->temporizador_parpadeo))
    {
        pantalla->mostrar_texto = !pantalla->mostrar_texto;
    }

    /* P sigue como atajo directo para reanudar */
    if (gbt_tecla_presionada(GBTK_p))
        return PAUSA_REANUDAR;

    /* Navegación con flechas */
    if (gbt_tecla_presionada(GBTK_ARRIBA))
    {
        pantalla->opcion_seleccionada =
            (pantalla->opcion_seleccionada - 1 + PAUSA_OPCION_TOTAL) % PAUSA_OPCION_TOTAL;
    }

    if (gbt_tecla_presionada(GBTK_ABAJO))
    {
        pantalla->opcion_seleccionada =
            (pantalla->opcion_seleccionada + 1) % PAUSA_OPCION_TOTAL;
    }

    /* Confirmar con ENTER */
    if (gbt_tecla_presionada(GBTK_ENTER))
    {
        if (pantalla->opcion_seleccionada == PAUSA_OPCION_REANUDAR)
            return PAUSA_REANUDAR;

        if (pantalla->opcion_seleccionada == PAUSA_OPCION_GUARDAR)
            return PAUSA_GUARDAR;

        if (pantalla->opcion_seleccionada == PAUSA_OPCION_MENU)
            return PAUSA_IR_AL_MENU;
    }

    return PAUSA_CONTINUAR;
}

void pausa_dibujar(const PantallaPausa *pantalla, int ancho, int alto)
{
    int centro_x = ancho / 2;
    int escala   = ancho / 320;
    if (escala < 1) escala = 1;

    /* Posiciones verticales */
    int y_titulo   = alto / 2 - 34 * escala;
    int y_reanudar = y_titulo + ESPACIO_TITULO_OPCIONES * escala;
    int y_guardar  = y_reanudar + ESPACIO_ENTRE_OPCIONES * escala;
    int y_menu     = y_guardar + ESPACIO_ENTRE_OPCIONES * escala;
    int y_ayuda    = y_menu + ESPACIO_ENTRE_OPCIONES * escala;

    dibujar_pared_ladrillos_completa();

    /* Panel oscuro */
    int margen   = ancho / 6;
    int panel_x1 = margen;
    int panel_x2 = ancho - margen;
    int panel_y1 = y_titulo - MARGEN_SUPERIOR_PANEL * escala;
    int panel_y2 = y_ayuda + GLIFO_ALTO * escala + MARGEN_INFERIOR_PANEL * escala;

    rellenar_rectangulo(panel_x1, panel_y1, panel_x2, panel_y2, COLOR_FONDO_TABLERO);
    dibujar_marco_incrustado_en(panel_x1, panel_y1, panel_x2, panel_y2);

    /* Título */
    texto_centrado_escalado("PAUSADO", centro_x, y_titulo, COLOR_TITULO, 2 * escala);

    /* Opciones navegables */
    dm_boton(centro_x, y_reanudar, "REANUDAR",
             pantalla->opcion_seleccionada == PAUSA_OPCION_REANUDAR, escala);

    dm_boton(centro_x, y_guardar, "GUARDAR PARTIDA",
             pantalla->opcion_seleccionada == PAUSA_OPCION_GUARDAR, escala);

    dm_boton(centro_x, y_menu, "MENU",
             pantalla->opcion_seleccionada == PAUSA_OPCION_MENU, escala);

    /* Instrucción de ayuda */
    if (pantalla->mostrar_texto)
    {
        const char *ayuda = "P : REANUDAR RAPIDO";
        dm_texto_escalado(ayuda,
                          centro_x - dm_ancho_texto(ayuda, escala) / 2,
                          y_ayuda,
                          COLOR_AYUDA,
                          escala);
    }

    gbt_volcar_backbuffer();
}

void pausa_destruir(PantallaPausa *pantalla)
{
    gbt_temporizador_destruir(pantalla->temporizador_parpadeo);
    pantalla->temporizador_parpadeo = NULL;
}
