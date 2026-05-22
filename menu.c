/*
 * menu.c — Menu principal del juego
 *
 * Opciones del menu:
 *   MODO      → Clasico o DX
 *   HISTORIAL → muestra el top 10 del modo seleccionado
 *   OPCIONES  → navega a la pantalla de configuracion
 */

#include "menu.h"
#include "dibujo_menu.h"
#include "layout.h"
#include "dibujo.h"
#include "GBT/gbt.h"

#define INTERVALO_PARPADEO_SEG  0.4
#define CANTIDAD_MODOS          2

#define COLOR_SELECCIONADO  COLOR_PIEZA_I
#define COLOR_NORMAL        COLOR_GRIS_OSCURO
#define COLOR_VALOR         COLOR_PIEZA_O
#define COLOR_TITULO        COLOR_PIEZA_T

/* Espaciados verticales del menú */
#define MARGEN_TITULO_PANEL         6
#define ESPACIO_TITULO_SEP          12
#define ESPACIO_ENTRE_OPCIONES      14
#define ESPACIO_SEP_INSTRUCCIONES   8
#define ESPACIO_ENTRE_INSTRUCCIONES 10

static const char *NOMBRES_MODO[CANTIDAD_MODOS] = { "CLASICO", "DX" };

/* Reinicia el parpadeo de flechas tras una accion del usuario. */
static void reiniciar_parpadeo(PantallaMenu *m)
{
    m->mostrar_flechas = 1;
    gbt_temporizador_pausar(m->temporizador_parpadeo);
    gbt_temporizador_reanudar(m->temporizador_parpadeo);
}

/* =========================================================
 * API PUBLICA
 * ========================================================= */

void menu_iniciar(PantallaMenu *m)
{
    m->item_sel              = ITEM_MODO;
    m->modo                  = MODO_CLASICO;
    m->temporizador_parpadeo = gbt_temporizador_crear(INTERVALO_PARPADEO_SEG);
    m->mostrar_flechas       = 1;
}

ResultadoMenu menu_actualizar(PantallaMenu *m)
{
    gbt_procesar_entrada();

    if (gbt_temporizador_consumir(m->temporizador_parpadeo))
    {
        m->mostrar_flechas = !m->mostrar_flechas;
    }

    if (gbt_tecla_presionada(GBTK_ARRIBA))
    {
        m->item_sel = (m->item_sel - 1 + ITEM_TOTAL) % ITEM_TOTAL;
        reiniciar_parpadeo(m);
    }

    if (gbt_tecla_presionada(GBTK_ABAJO))
    {
        m->item_sel = (m->item_sel + 1) % ITEM_TOTAL;
        reiniciar_parpadeo(m);
    }

    if (gbt_tecla_presionada(GBTK_IZQUIERDA) || gbt_tecla_presionada(GBTK_DERECHA))
    {
        if (m->item_sel == ITEM_MODO)
        {
            int direccion = gbt_tecla_presionada(GBTK_DERECHA) ? 1 : -1;
            m->modo = (m->modo + direccion + CANTIDAD_MODOS) % CANTIDAD_MODOS;
        }
        reiniciar_parpadeo(m);
    }

    if (gbt_tecla_presionada(GBTK_ENTER))
    {
        if (m->item_sel == ITEM_CONTINUAR_JUEGO) return MENU_CONTINUAR_PARTIDA;
        if (m->item_sel == ITEM_HISTORIAL)       return MENU_HISTORIAL;
        if (m->item_sel == ITEM_OPCIONES)        return MENU_CONFIGURACION;
        return MENU_JUGAR;
    }

    if (gbt_tecla_presionada(GBTK_ESCAPE))
        return MENU_VOLVER;

    return MENU_CONTINUAR;
}

void menu_dibujar(const PantallaMenu *m, int ancho, int alto)
{
    int centro_x = ancho / 2;
    int escala   = ancho / 320;
    if (escala < 1) escala = 1;

    /* Posiciones verticales calculadas desde y_titulo */
    int y_titulo              = alto / 2 - 60 * escala;
    int y_separacion_superior = y_titulo + ESPACIO_TITULO_SEP * escala;
    int y_modo                = y_separacion_superior + ESPACIO_ENTRE_OPCIONES * escala;
    int y_continuar           = y_modo + ESPACIO_ENTRE_OPCIONES * escala;
    int y_historial           = y_continuar + ESPACIO_ENTRE_OPCIONES * escala;
    int y_opciones            = y_historial + ESPACIO_ENTRE_OPCIONES * escala;
    int y_separacion_inferior = y_opciones + ESPACIO_ENTRE_OPCIONES * escala;
    int y_enter               = y_separacion_inferior + ESPACIO_SEP_INSTRUCCIONES * escala;
    int y_esc                 = y_enter + ESPACIO_ENTRE_INSTRUCCIONES * escala;

    dibujar_pared_ladrillos_completa();

    /* Panel oscuro */
    int margen   = ancho / 8;
    int panel_x1 = margen;
    int panel_x2 = ancho - margen;
    int panel_y1 = y_titulo - MARGEN_TITULO_PANEL * escala;
    int panel_y2 = y_esc + GLIFO_ALTO * escala + MARGEN_TITULO_PANEL * escala;

    rellenar_rectangulo(panel_x1, panel_y1, panel_x2, panel_y2, COLOR_FONDO_TABLERO);
    dibujar_marco_incrustado_en(panel_x1, panel_y1, panel_x2, panel_y2);

    /* Titulo */
    const char *titulo = "MENU";
    dm_texto_escalado(titulo,
                      centro_x - dm_ancho_texto(titulo, escala) / 2,
                      y_titulo,
                      COLOR_TITULO,
                      escala);

    dm_separador(ancho, y_separacion_superior);

    /* Opciones */
    dm_fila_opcion(centro_x,
                   y_modo,
                   "MODO",
                   NOMBRES_MODO[m->modo],
                   m->item_sel == ITEM_MODO,
                   m->mostrar_flechas,
                   escala);

    dm_boton(centro_x,
             y_continuar,
             "CONTINUAR PARTIDA",
             m->item_sel == ITEM_CONTINUAR_JUEGO,
             escala);

    dm_boton(centro_x,
             y_historial,
             "HISTORIAL",
             m->item_sel == ITEM_HISTORIAL,
             escala);

    dm_boton(centro_x,
             y_opciones,
             "OPCIONES",
             m->item_sel == ITEM_OPCIONES,
             escala);

    dm_separador(ancho, y_separacion_inferior);

    /* Instrucciones de pie */
    const char *texto_accion;
    if (m->item_sel == ITEM_OPCIONES)
    {
        texto_accion = "ENTER : ABRIR";
    }
    else if (m->item_sel == ITEM_HISTORIAL)
    {
        texto_accion = "ENTER : VER";
    }
    else if (m->item_sel == ITEM_CONTINUAR_JUEGO)
    {
        texto_accion = "ENTER : CARGAR";
    }
    else
    {
        texto_accion = "ENTER : JUGAR";
    }

    dm_texto_escalado(texto_accion,
                      centro_x - dm_ancho_texto(texto_accion, escala) / 2,
                      y_enter,
                      COLOR_GRIS_OSCURO,
                      escala);

    const char *texto_esc = "ESC : VOLVER";
    dm_texto_escalado(texto_esc,
                      centro_x - dm_ancho_texto(texto_esc, escala) / 2,
                      y_esc,
                      COLOR_GRIS_OSCURO,
                      escala);

    gbt_volcar_backbuffer();
}

void menu_destruir(PantallaMenu *m)
{
    gbt_temporizador_destruir(m->temporizador_parpadeo);
    m->temporizador_parpadeo = NULL;
}
