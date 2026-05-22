/*
 * primitivas de dibujo
 *
 * Este modulo solo sabe dibujar pixeles, rectangulos, marcos, texto y fondo.
 *
 */

#include "dibujo.h"
#include "fuente.h"
#include "layout.h"
#include "GBT/gbt.h"

/* =========================================================
 * PIXEL Y RECTANGULO
 * ========================================================= */

/*
    Dibuja un pixel en (x, y). Ignora coordenadas fuera de pantalla.
*/
void dibujar_pixel(int x, int y, uint8_t color)
{
    if (x >= 0 && x < PANTALLA_ANCHO && y >= 0 && y < PANTALLA_ALTO)
    {
        gbt_dibujar_pixel((uint16_t)x, (uint16_t)y, color);
    }
}

/*
    Rellena el rectangulo entre (x1,y1) y (x2,y2) inclusive.
*/
void rellenar_rectangulo(int x1, int y1, int x2, int y2, uint8_t color)
{
    for (int y = y1; y <= y2; y++)
    {
        for (int x = x1; x <= x2; x++)
        {
            dibujar_pixel(x, y, color);
        }
    }
}

/* =========================================================
 * BORDE Y MARCO
 * ========================================================= */

/*
 * Dibuja los cuatro bordes de un rectangulo con dos colores:
 *   color_luz: borde superior e izquierdo
 *   color_sombra: borde inferior y derecho
 */
static void dibujar_borde_anillo(int x1, int y1, int x2, int y2, uint8_t color_luz, uint8_t color_sombra)
{
    /* Bordes horizontales */
    for (int x = x1; x <= x2; x++)
    {
        dibujar_pixel(x, y1, color_luz);
        dibujar_pixel(x, y2, color_sombra);
    }
    /* Bordes verticales (sin repetir esquinas) */
    for (int y = y1 + 1; y <= y2 - 1; y++)
    {
        dibujar_pixel(x1, y, color_luz);
        dibujar_pixel(x2, y, color_sombra);
    }
}

/*
    Dibuja un marco decorativo de tres anillos alrededor.
*/
void dibujar_marco_incrustado_en(int x1, int y1, int x2, int y2)
{
    static const int     MARCO_GROSOR = 3;
    static const uint8_t COLORES_LUZ[3] = { COLOR_MORTERO, COLOR_LADRILLO_CLARO, COLOR_GRIS_SOMBRA };
    static const uint8_t COLORES_SOMBRA[3] = { COLOR_MORTERO, COLOR_GRIS_SOMBRA, COLOR_LADRILLO_CLARO };

    for (int i = 0; i < MARCO_GROSOR; i++)
    {
        int offset = MARCO_GROSOR - i;

        dibujar_borde_anillo(x1 - offset,
                             y1 - offset,
                             x2 + offset,
                             y2 + offset,
                             COLORES_LUZ[i],
                             COLORES_SOMBRA[i]);
    }
}

/* =========================================================
 * TEXTO
 *
 * Cada caracter se dibuja recorriendo su glifo (5x7) con un puntero al bitmap.
 * Si escala > 1, cada pixel del glifo se convierte en un bloque de escala x escala.
 * ========================================================= */
/*
    Dibuja un glifo individual con sombra negra desplazada.
*/
static void dibujar_glifo_escalado(const tGlifo *glifo, int x, int y, uint8_t color, int escala, int sombra)
{
    for (int fila = 0; fila < GLIFO_ALTO; fila++)
    {
        for (int col = 0; col < glifo->ancho; col++)
        {
            if (!glifo->b[fila][col])
                continue;

            int bloque_x = x + col  * escala; // posición horizontal del bloque en pantalla
            int bloque_y = y + fila * escala; // posición vertical del bloque en pantalla

            /* Bloque de sombra (desplazado) */
            rellenar_rectangulo(bloque_x + sombra,
                                bloque_y + sombra,
                                bloque_x + sombra + escala - 1,
                                bloque_y + sombra + escala - 1,
                                COLOR_NEGRO);

            /* Bloque principal */
            rellenar_rectangulo(bloque_x,
                                bloque_y,
                                bloque_x + escala - 1,
                                bloque_y + escala - 1,
                                color);
        }
    }
}

/*
    Calcula el ancho total en píxeles de una cadena con el escala dado.
    Usa el ancho variable de cada glifo.
*/
static int calcular_ancho_texto(const char *s, int escala)
{
    int total = 0;
    const char *p = s;

    while (*p)
    {
        const tGlifo *g = fuente_buscar_glifo(*p);
        total += (g ? g->ancho : GLIFO_ANCHO) * escala + escala;
        p++;
    }
    return total > 0 ? total - escala : 0;
}

/*
    Dibuja texto escalado (escala=2 → cada pixel se dibuja 2x2).
*/
void texto_escalado(const char *s, int x, int y, uint8_t color, int escala)
{
    int sombra = escala > 1 ? 2 : 1;
    const char *p = s;

    while (*p)
    {
        const tGlifo *glifo = fuente_buscar_glifo(*p);

        if (glifo != NULL)
        {
            dibujar_glifo_escalado(glifo, x, y, color, escala, sombra);
            x += glifo->ancho * escala + escala;
        }
        else
        {
            x += GLIFO_ANCHO * escala + escala;
        }

        p++;
    }
}

/*
    Dibuja texto en (x, y) al tamano original (variable × 7 px por caracter).
*/
void texto(const char *s, int x, int y, uint8_t color)
{
    texto_escalado(s, x, y, color, 1);
}

/*
    Dibuja texto centrado horizontalmente sobre la coordenada centro_x.
*/
void texto_centrado(const char *s, int centro_x, int y, uint8_t color)
{
    texto(s, centro_x - calcular_ancho_texto(s, 1) / 2, y, color);
}

/*
    Dibuja texto escalado centrado horizontalmente sobre centro_x.
*/
void texto_centrado_escalado(const char *s, int centro_x, int y, uint8_t color, int escala)
{
    texto_escalado(s, centro_x - calcular_ancho_texto(s, escala) / 2, y, color, escala);
}

/*
    Dibuja texto grande centrado horizontalmente sobre centro_x.
*/
void texto_grande_centrado(const char *s, int centro_x, int y, uint8_t color)
{
    texto_centrado_escalado(s, centro_x, y, color, ESCALA_FUENTE_GRANDE);
}

/* =========================================================
 * FONDO DE LADRILLOS
 *
 * Cada ladrillo tiene borde claro arriba/izquierda y sombra abajo/derecha, con mortero entre ellos.
 * ========================================================= */

/*
    Pared de ladrillos que cubre toda la pantalla.
*/
void dibujar_pared_ladrillos_completa(void)
{
    const int ancho_ladrillo = LADRILLO_ANCHO_CELDA;
    const int alto_ladrillo = LADRILLO_ALTO_CELDA;

    for (int pixel_y = 0; pixel_y < PANTALLA_ALTO; pixel_y++)
    {
        int fila          = pixel_y / alto_ladrillo;
        int y_en_ladrillo = pixel_y % alto_ladrillo;
        int desplaz       = (fila % 2) ? ancho_ladrillo / 2 : 0;

        for (int bloque_x = 0; bloque_x < PANTALLA_ANCHO; bloque_x++)
        {
            int x_en_ladrillo = (bloque_x + desplaz) % ancho_ladrillo;
            uint8_t color;

            if (y_en_ladrillo == alto_ladrillo - 1 || x_en_ladrillo == ancho_ladrillo - 1)
            {
                color = COLOR_MORTERO;
            }
            else if (y_en_ladrillo == 0 || x_en_ladrillo == 0)
            {
                color = COLOR_LADRILLO_CLARO;
            }
            else
            {
                color = COLOR_LADRILLO_BASE;
            }

            gbt_dibujar_pixel((uint16_t)bloque_x, (uint16_t)pixel_y, color);
        }
    }
}
