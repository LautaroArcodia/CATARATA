/*
 * Pantalla de historial de mejores puntajes
 *
 * Muestra los 10 jugadores con mejor puntaje del modo seleccionado en el menú.
 */

#include "historial.h"
#include "estadisticas.h"
#include "cadena.h"
#include "dibujo.h"
#include "layout.h"
#include "GBT/gbt.h"
#include <stdio.h>

#define COLOR_TITULO   COLOR_PIEZA_T
#define COLOR_MODO     COLOR_PIEZA_O
#define COLOR_NOMBRE   COLOR_BLANCO
#define COLOR_PUNTAJE  COLOR_PIEZA_I
#define COLOR_POSICION COLOR_GRIS_OSCURO
#define COLOR_VACIO    COLOR_GRIS_OSCURO

#define MARGEN_CONTENIDO       8  /* sangría horizontal del contenido dentro del panel */
#define MARGEN_SEPARADOR       3  /* sangría horizontal de la línea separadora */
#define MARGEN_SUPERIOR_PANEL  8
#define MARGEN_INFERIOR_PANEL  8

#define ESPACIO_TITULO_MODO    10
#define ESPACIO_MODO_SEP       10
#define ESPACIO_SEP_ENTRADAS   4
#define ESPACIO_SEP_ESC        5

static const char *NOMBRES_MODO[2] = {"CLASICO", "DX"};

/* =========================================================
 * FUNCIONES INTERNAS DE DIBUJO
 * ========================================================= */

/* Dibuja una línea horizontal de separación dentro del panel. */
static void dibujar_separador(int x1, int x2, int y)
{
    for (int x = x1; x <= x2; x++)
    {
        dibujar_pixel(x, y, COLOR_MORTERO);
    }
}

/*
 * Dibuja una fila del ranking:
 *   posicion y nombre alineados a la izquierda,
 *   puntaje alineado a la derecha.
 */
static void dibujar_fila_ranking(int x_izq, int x_der, int y, int posicion, const tRegistroJugador *jugador, int escala)
{
    char texto_posicion[8];
    char texto_puntaje[12];

    sprintf(texto_posicion, "%d.", posicion);

    int ancho_posicion = cadena_longitud(texto_posicion) * GLIFO_PASO * escala;

    texto_escalado(texto_posicion, x_izq, y, COLOR_POSICION, escala);

    if (jugador != NULL)
    {
        texto_escalado(jugador->nombre, x_izq + ancho_posicion + 2 * escala, y, COLOR_NOMBRE, escala);

        sprintf(texto_puntaje, "%d", jugador->mejor_puntaje);

        int ancho_puntaje = cadena_longitud(texto_puntaje) * GLIFO_PASO * escala;
        texto_escalado(texto_puntaje, x_der - ancho_puntaje, y, COLOR_PUNTAJE, escala);
    }
    else
    {
        texto_escalado("---", x_izq + ancho_posicion + 2 * escala, y, COLOR_VACIO, escala);
    }
}

/* =========================================================
 * API PÚBLICA
 * ========================================================= */

void historial_iniciar(PantallaHistorial *h, int modo)
{
    h->modo = modo;
}

ResultadoHistorial historial_actualizar(PantallaHistorial *h)
{
    gbt_procesar_entrada();

    if (gbt_tecla_presionada(GBTK_ESCAPE) || gbt_tecla_presionada(GBTK_ENTER))
        return HISTORIAL_VOLVER;

    return HISTORIAL_CONTINUAR;
}

void historial_dibujar(const PantallaHistorial *h, int ancho, int alto)
{
    int escala   = ancho / 320;
    if (escala < 1) escala = 1;

    int centro_x = ancho / 2;
    int margen   = ancho / 8;
    int panel_x1 = margen;
    int panel_x2 = ancho - margen;

    /* Cargar ranking jugadores del modo actual */
    tRegistroJugador ranking[ESTADISTICAS_MAX_JUGADORES];
    int cantidad = estadisticas_obtener_ranking(ranking, ESTADISTICAS_MAX_JUGADORES, h->modo);

    /* Altura del panel calculada desde constantes nombradas */
    int espacio_fila = (GLIFO_ALTO + 2) * escala;

    int panel_altura = (MARGEN_SUPERIOR_PANEL
                      + ESPACIO_TITULO_MODO
                      + ESPACIO_MODO_SEP
                      + ESPACIO_SEP_ENTRADAS
                      + ESTADISTICAS_MAX_JUGADORES * (GLIFO_ALTO + 2)
                      + ESPACIO_SEP_ESC
                      + GLIFO_ALTO
                      + MARGEN_INFERIOR_PANEL) * escala;

    /* Posiciones verticales calculadas desde panel_y1 */
    int panel_y1              = (alto - panel_altura) / 2;
    int panel_y2              = panel_y1 + panel_altura;
    int y_titulo              = panel_y1 + MARGEN_SUPERIOR_PANEL * escala;
    int y_modo                = y_titulo + ESPACIO_TITULO_MODO * escala;
    int y_separacion_superior = y_modo + ESPACIO_MODO_SEP * escala;
    int y_entradas            = y_separacion_superior + ESPACIO_SEP_ENTRADAS * escala;
    int y_separacion_inferior = y_entradas + ESTADISTICAS_MAX_JUGADORES * espacio_fila;
    int y_esc                 = y_separacion_inferior + ESPACIO_SEP_ESC * escala;

    int x_izq = panel_x1 + MARGEN_CONTENIDO * escala;
    int x_der = panel_x2 - MARGEN_CONTENIDO * escala;

    dibujar_pared_ladrillos_completa();

    rellenar_rectangulo(panel_x1, panel_y1, panel_x2, panel_y2, COLOR_FONDO_TABLERO);
    dibujar_marco_incrustado_en(panel_x1, panel_y1, panel_x2, panel_y2);

    texto_centrado_escalado("HISTORIAL", centro_x, y_titulo, COLOR_TITULO, escala);
    texto_centrado_escalado(NOMBRES_MODO[h->modo], centro_x, y_modo, COLOR_MODO, escala);

    dibujar_separador(panel_x1 + MARGEN_SEPARADOR, panel_x2 - MARGEN_SEPARADOR, y_separacion_superior);

    for (int i = 0; i < ESTADISTICAS_MAX_JUGADORES; i++)
    {
        int y_fila = y_entradas + i * espacio_fila;

        dibujar_fila_ranking(x_izq, x_der, y_fila,
                             i + 1,
                             (i < cantidad) ? &ranking[i] : NULL,
                             escala);
    }

    dibujar_separador(panel_x1 + MARGEN_SEPARADOR, panel_x2 - MARGEN_SEPARADOR, y_separacion_inferior);

    texto_centrado_escalado("ESC : VOLVER", centro_x, y_esc, COLOR_GRIS_OSCURO, escala);

    gbt_volcar_backbuffer();
}

void historial_destruir(PantallaHistorial *h)
{
    (void)h;
}
