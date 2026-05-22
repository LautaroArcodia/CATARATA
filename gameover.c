#include "gameover.h"
#include "dibujo.h"
#include "cadena.h"
#include "layout.h"
#include "GBT/gbt.h"
#include <stdio.h>

#define INTERVALO_PARPADEO  0.5
#define MARGEN_SEPARADOR    5   /* sangría horizontal de la línea separadora */
#define MARGEN_CONTENIDO    8   /* sangría horizontal del texto dentro del panel */
#define ESPACIADO_BASE      10   /* unidad mínima de separación vertical */

/* =========================================================
 * FUNCIONES INTERNAS DE DIBUJO
 * ========================================================= */

/* Oscurece la pantalla con un patrón de ajedrez semi-transparente. */
static void dibujar_overlay(void)
{
    for (int y = 0; y < PANTALLA_ALTO; y++)
    {
        for (int x = 0; x < PANTALLA_ANCHO; x++)
        {
            if ((x + y) % 2 == 0)
            {
                gbt_dibujar_pixel((uint16_t)x, (uint16_t)y, COLOR_NEGRO);
            }
        }
    }
}

/* Dibuja una línea horizontal de separación dentro del panel. */
static void dibujar_separador(int y)
{
    for (int x = GAMEOVER_X1 + MARGEN_SEPARADOR; x <= GAMEOVER_X2 - MARGEN_SEPARADOR; x++)
    {
        dibujar_pixel(x, y, COLOR_MORTERO);
    }
}

/*
 * Dibuja una fila de estadística:
 *   etiqueta alineada a la izquierda del panel,
 *   valor numérico alineado a la derecha.
 */
static void dibujar_fila_stat(const char *etiqueta, int valor, uint8_t color_valor, int y, int escala)
{
    char texto_valor[12];
    sprintf(texto_valor, "%d", valor);

    texto_escalado(etiqueta, GAMEOVER_X1 + MARGEN_CONTENIDO, y, COLOR_GRIS_OSCURO, escala);

    int ancho_valor = cadena_longitud(texto_valor) * GLIFO_PASO * escala;
    texto_escalado(texto_valor, GAMEOVER_X2 - MARGEN_CONTENIDO - ancho_valor, y, color_valor, escala);
}

/* =========================================================
 * API PÚBLICA
 * ========================================================= */

void gameover_iniciar(tPantallaGameOver *g)
{
    g->temporizador_parpadeo = gbt_temporizador_crear(INTERVALO_PARPADEO);
    g->mostrar_instrucciones = 1;
}

eResultadoGameOver gameover_actualizar(tPantallaGameOver *g)
{
    gbt_procesar_entrada();

    if (gbt_temporizador_consumir(g->temporizador_parpadeo))
        g->mostrar_instrucciones = !g->mostrar_instrucciones;

    if (gbt_tecla_presionada(GBTK_ENTER))  return GAMEOVER_IR_AL_MENU;
    if (gbt_tecla_presionada(GBTK_ESCAPE)) return GAMEOVER_IR_A_PRESENTACION;

    return GAMEOVER_CONTINUAR;
}

void gameover_destruir(tPantallaGameOver *g)
{
    gbt_temporizador_destruir(g->temporizador_parpadeo);
    g->temporizador_parpadeo = NULL;
}

void gameover_dibujar(int puntuacion, int mejor_puntaje, int mostrar_instrucciones)
{
    const int centro_x = (GAMEOVER_X1 + GAMEOVER_X2) / 2;
    const int escala   = PANTALLA_ANCHO / 320;

    const int y_titulo              = GAMEOVER_Y1 + MARGEN_CONTENIDO * escala;
    const int y_separador_superior  = y_titulo    + GLIFO_ALTO * GAMEOVER_ESCALA_TEXTO + ESPACIADO_BASE * escala;
    const int y_score               = y_separador_superior + ESPACIADO_BASE * escala;
    const int y_mejor               = y_score     + ESPACIADO_BASE * escala;
    const int y_separador_inferior  = y_mejor     + ESPACIADO_BASE * escala;
    const int y_controles           = y_separador_inferior + ESPACIADO_BASE * escala;
    const int y_esc                 = y_controles + ESPACIADO_BASE * escala;

    dibujar_overlay();

    rellenar_rectangulo(GAMEOVER_X1, GAMEOVER_Y1, GAMEOVER_X2, GAMEOVER_Y2, COLOR_FONDO_TABLERO);
    dibujar_marco_incrustado_en(GAMEOVER_X1, GAMEOVER_Y1, GAMEOVER_X2, GAMEOVER_Y2);

    texto_centrado_escalado("GAME OVER", centro_x, y_titulo, COLOR_ROJO, GAMEOVER_ESCALA_TEXTO);

    dibujar_separador(y_separador_superior);

    dibujar_fila_stat("SCORE", puntuacion,    COLOR_BLANCO,         y_score, escala);
    dibujar_fila_stat("MEJOR", mejor_puntaje, COLOR_LADRILLO_CLARO, y_mejor, escala);

    dibujar_separador(y_separador_inferior);

    if (mostrar_instrucciones)
    {
        texto_centrado_escalado("ENTER = MENU", centro_x, y_controles, COLOR_BLANCO, escala);
    }

    texto_centrado_escalado("ESC = VOLVER", centro_x, y_esc, COLOR_GRIS_OSCURO, escala);
}
