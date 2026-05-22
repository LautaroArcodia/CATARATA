/*
 * dibujo_menu.c — Funciones de dibujo compartidas por pantallas tipo menú
 *
 * Agrupa las funciones de UI usadas por menú, configuración y
 * cualquier pantalla con navegación por ítems.
 */

#include "dibujo_menu.h"
#include "dibujo.h"
#include "fuente.h"
#include "GBT/gbt.h"

/* =========================================================
 * TEXTO ESCALADO CON SOMBRA
 * ========================================================= */

int dm_texto_escalado(const char *s, int x, int y, uint8_t color, int escala)
{
    int x_inicio = x;

    while (*s)
    {
        const tGlifo *glifo = fuente_buscar_glifo(*s);

        if (glifo != NULL)
        {
            for (int fila = 0; fila < GLIFO_ALTO; fila++)
            {
                for (int col = 0; col < glifo->ancho; col++)
                {
                    if (glifo->b[fila][col])
                    {
                        rellenar_rectangulo(
                            x + col * escala + SOMBRA_DESPLAZAMIENTO,
                            y + fila * escala + SOMBRA_DESPLAZAMIENTO,
                            x + col * escala + SOMBRA_DESPLAZAMIENTO + escala - 1,
                            y + fila * escala + SOMBRA_DESPLAZAMIENTO + escala - 1,
                            COLOR_GRIS_SOMBRA);

                        rellenar_rectangulo(
                            x + col * escala,
                            y + fila * escala,
                            x + col * escala + escala - 1,
                            y + fila * escala + escala - 1,
                            color);
                    }
                }
            }
            x += glifo->ancho * escala + escala;
        }
        else
        {
            x += GLIFO_ANCHO * escala + escala;
        }

        s++;
    }

    return x - x_inicio - escala;
}

int dm_ancho_texto(const char *s, int escala)
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

/* =========================================================
 * FLECHAS DE NAVEGACIÓN
 * ========================================================= */

static void dibujar_flecha(int x, int y, uint8_t color, const unsigned char bitmap[7][4])
{
    for (int fila = 0; fila < 7; fila++)
    {
        for (int col = 0; col < 4; col++)
        {
            if (bitmap[fila][col])
            {
                gbt_dibujar_pixel((uint16_t)(x + col + SOMBRA_DESPLAZAMIENTO),
                                  (uint16_t)(y + fila + SOMBRA_DESPLAZAMIENTO),
                                  COLOR_GRIS_SOMBRA);

                gbt_dibujar_pixel((uint16_t)(x + col),
                                  (uint16_t)(y + fila),
                                  color);
            }
        }
    }
}

void dm_flecha_izquierda(int x, int y, uint8_t color)
{
    static const unsigned char bitmap[7][4] =
    {
        {0,0,0,1},
        {0,0,1,1},
        {0,1,1,1},
        {1,1,1,1},
        {0,1,1,1},
        {0,0,1,1},
        {0,0,0,1}
    };
    dibujar_flecha(x, y, color, bitmap);
}

void dm_flecha_derecha(int x, int y, uint8_t color)
{
    static const unsigned char bitmap[7][4] =
    {
        {1,0,0,0},
        {1,1,0,0},
        {1,1,1,0},
        {1,1,1,1},
        {1,1,1,0},
        {1,1,0,0},
        {1,0,0,0}
    };
    dibujar_flecha(x, y, color, bitmap);
}

/* =========================================================
 * SEPARADOR DECORATIVO
 * ========================================================= */

void dm_separador(int ancho_pantalla, int y)
{
    static const uint8_t colores[] =
    {
        COLOR_PIEZA_I, COLOR_PIEZA_J, COLOR_PIEZA_L, COLOR_PIEZA_O,
        COLOR_PIEZA_S, COLOR_PIEZA_T, COLOR_PIEZA_Z,
        COLOR_PIEZA_T, COLOR_PIEZA_S, COLOR_PIEZA_O,
        COLOR_PIEZA_L, COLOR_PIEZA_J, COLOR_PIEZA_I
    };

    const int tam_bloque  = 2;
    const int paso_bloque = tam_bloque + 1;   /* bloque + 1px de separación */
    const int cantidad    = sizeof(colores) / sizeof(colores[0]);

    int x_inicio = (ancho_pantalla - cantidad * paso_bloque) / 2;

    for (int i = 0; i < cantidad; i++)
    {
        int bloque_x = x_inicio + i * paso_bloque;

        rellenar_rectangulo(bloque_x + SOMBRA_DESPLAZAMIENTO,
                            y + SOMBRA_DESPLAZAMIENTO,
                            bloque_x + SOMBRA_DESPLAZAMIENTO + tam_bloque - 1,
                            y + SOMBRA_DESPLAZAMIENTO + tam_bloque - 1,
                            COLOR_GRIS_SOMBRA);

        rellenar_rectangulo(bloque_x,
                            y,
                            bloque_x + tam_bloque - 1,
                            y + tam_bloque - 1,
                            colores[i]);
    }
}

/* =========================================================
 * FILA DE OPCIÓN CON SELECTOR
 * ========================================================= */

void dm_fila_opcion(int centro_x, int y, const char *etiqueta, const char *valor, int seleccionado, int mostrar_flechas, int escala)
{
    const int ancho_flecha = 4 * escala;
    const int separacion   = 4 * escala;

    int ancho_etiqueta = dm_ancho_texto(etiqueta, escala);
    int ancho_separador      = dm_ancho_texto(" : ", escala);
    int ancho_valor    = dm_ancho_texto(valor, escala);
    int ancho_total    = ancho_etiqueta + ancho_separador + ancho_flecha + separacion + ancho_valor + separacion + ancho_flecha;

    int cursor_x = centro_x - ancho_total / 2;

    uint8_t color_etiqueta = seleccionado ? COLOR_PIEZA_I  : COLOR_GRIS_OSCURO;
    uint8_t color_flecha   = (seleccionado && mostrar_flechas) ? COLOR_PIEZA_O : COLOR_GRIS_OSCURO;
    uint8_t color_val      = seleccionado ? COLOR_PIEZA_O  : COLOR_GRIS_OSCURO;

    if (seleccionado)
    {
        dm_flecha_derecha(cursor_x - ancho_flecha - separacion * 2, y, COLOR_PIEZA_I);
    }

    cursor_x += dm_texto_escalado(etiqueta, cursor_x, y, color_etiqueta, escala);
    cursor_x += dm_ancho_texto(" ", escala) + escala;

    dm_texto_escalado(":", cursor_x, y, color_etiqueta, escala);

    cursor_x += dm_ancho_texto(": ", escala) + escala;

    dm_flecha_izquierda(cursor_x, y, color_flecha);

    cursor_x += ancho_flecha + separacion;

    dm_texto_escalado(valor, cursor_x, y, color_val, escala);

    cursor_x += ancho_valor + separacion;

    dm_flecha_derecha(cursor_x, y, color_flecha);
}

/* =========================================================
 * BOTÓN SIMPLE
 * ========================================================= */

void dm_boton(int centro_x, int y, const char *etiqueta, int seleccionado, int escala)
{
    const int ancho_flecha = 4 * escala;
    const int separacion   = 4 * escala;
    uint8_t color = seleccionado ? COLOR_PIEZA_I : COLOR_GRIS_OSCURO;
    int x = centro_x - dm_ancho_texto(etiqueta, escala) / 2;

    if (seleccionado)
        dm_flecha_derecha(x - ancho_flecha - separacion * 2, y, COLOR_PIEZA_I);

    dm_texto_escalado(etiqueta, x, y, color, escala);
}
