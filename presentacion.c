/*
 * presentacion.c — Pantalla de presentacion (titulo TETRIS animado)
 *
 * Muestra el titulo "TETRIS" con letras de colores de piezas,
 * tetrominos rebotando en el fondo, y un texto parpadeante "JUGAR".
 *
 * Simplificaciones respecto al original:
 *   - Los bloques escalados se dibujan con rellenar_rectangulo()
 *     en vez de loops anidados de sx/sy.
 *   - Las dos pasadas (sombra + color) se hacen en un solo recorrido.
 */

#include "presentacion.h"
#include "fuente.h"
#include "layout.h"
#include "dibujo.h"
#include <stdlib.h>

/* =========================================================
 * INDICES DE PALETA
 * ========================================================= */
#define COLOR_AMARILLO  COLOR_PIEZA_O
#define COLOR_SOMBRA    COLOR_GRIS_SOMBRA

/* =========================================================
 * PARAMETROS VISUALES
 * ========================================================= */
#define INTERVALO_PARPADEO_SEG     0.5
#define SOMBRA_TITULO              4
#define SOMBRA_BG                  1
#define SOMBRA_TEXTO               1
#define TAMANO_BLOQUE_FONDO        8    /* px de cada bloque del fondo          */
#define PASO_BLOQUE_FONDO         10    /* TAMANO_BLOQUE_FONDO + px separacion */

/* Glifo del titulo */
#define GLIFO_TITULO_ALTO          7    /* filas del glifo grande               */
#define GLIFO_TITULO_ANCHO         5    /* columnas del glifo grande            */
#define LETRAS_TITULO              6    /* cantidad de letras en "TETRIS"       */
#define ESPACIADO_ENTRE_LETRAS     2    /* px entre letras del titulo           */
#define ESPACIADO_ENTRE_PIXELES    1    /* px entre caracteres del texto normal */

/* Separador de colores */
#define TAMANO_BLOQUE_SEPARADOR    3
#define PASO_BLOQUE_SEPARADOR      (TAMANO_BLOQUE_SEPARADOR + 1)

/* Escala de la pantalla */
#define DIVISOR_ESCALA             80   /* ancho / DIVISOR_ESCALA = escala base        */
#define ESCALA_MINIMA               2
#define ESCALA_MULTIPLICADOR_TITULO 1   /* cuanto mas grande que el resto es el titulo */

/* Espaciados verticales del panel (en unidades de escala) */
#define MARGEN_PANEL               2   /* margen interior del panel                   */
#define ESPACIO_TITULO_SEP         5   /* entre el titulo y el separador superior     */
#define ESPACIO_SEP_JUGAR          3   /* entre el separador superior y "JUGAR"       */
#define ESPACIO_JUGAR_SEP          5   /* entre "JUGAR" y el separador inferior       */

/* =========================================================
 * FUENTE GRANDE: TITULO "TETRIS" CON COLORES POR PIEZA
 *
 * Cada celda contiene 0 (vacio) o un indice de color de paleta.
 * ========================================================= */
typedef uint8_t tGlifoTitulo[GLIFO_TITULO_ALTO][GLIFO_TITULO_ANCHO];

static const tGlifoTitulo G_T = {
    {COLOR_PIEZA_I, COLOR_PIEZA_I, COLOR_PIEZA_I, COLOR_PIEZA_I, COLOR_PIEZA_L},
    {0,             0,             COLOR_PIEZA_I, 0,             0            },
    {0,             0,             COLOR_PIEZA_I, 0,             0            },
    {0,             0,             COLOR_PIEZA_I, 0,             0            },
    {0,             0,             COLOR_PIEZA_L, 0,             0            },
    {0,             0,             COLOR_PIEZA_L, 0,             0            },
    {0,             0,             COLOR_PIEZA_L, 0,             0            },
};

static const tGlifoTitulo G_E = {
    {COLOR_PIEZA_Z, COLOR_PIEZA_I, COLOR_PIEZA_I, COLOR_PIEZA_I, COLOR_PIEZA_I},
    {COLOR_PIEZA_Z, 0,             0,             0,             0            },
    {COLOR_PIEZA_Z, 0,             0,             0,             0            },
    {COLOR_PIEZA_Z, COLOR_PIEZA_T, COLOR_PIEZA_T, COLOR_PIEZA_T, 0            },
    {COLOR_PIEZA_S, 0,             0,             0,             0            },
    {COLOR_PIEZA_S, 0,             0,             0,             0            },
    {COLOR_PIEZA_S, COLOR_PIEZA_L, COLOR_PIEZA_L, COLOR_PIEZA_L, COLOR_PIEZA_L},
};

static const tGlifoTitulo G_R = {
    {COLOR_PIEZA_J, COLOR_PIEZA_J, COLOR_PIEZA_J, COLOR_PIEZA_J, 0            },
    {COLOR_PIEZA_J, 0,             0,             0,             COLOR_PIEZA_O},
    {COLOR_PIEZA_I, 0,             0,             0,             COLOR_PIEZA_O},
    {COLOR_PIEZA_I, COLOR_PIEZA_I, COLOR_PIEZA_I, COLOR_PIEZA_O, 0            },
    {COLOR_PIEZA_I, 0,             COLOR_PIEZA_T, 0,             0            },
    {COLOR_PIEZA_Z, 0,             0,             COLOR_PIEZA_T, 0            },
    {COLOR_PIEZA_Z, 0,             0,             0,             COLOR_PIEZA_T},
};

static const tGlifoTitulo G_I = {
    {COLOR_PIEZA_O, COLOR_PIEZA_O, COLOR_PIEZA_O, COLOR_PIEZA_O, COLOR_PIEZA_O},
    {0,             0,             COLOR_PIEZA_O, 0,             0            },
    {0,             0,             COLOR_PIEZA_T, 0,             0            },
    {0,             0,             COLOR_PIEZA_T, 0,             0            },
    {0,             0,             COLOR_PIEZA_T, 0,             0            },
    {0,             0,             COLOR_PIEZA_T, 0,             0            },
    {COLOR_PIEZA_Z, COLOR_PIEZA_Z, COLOR_PIEZA_Z, COLOR_PIEZA_Z, COLOR_PIEZA_Z},
};

static const tGlifoTitulo G_S = {
    {0,             COLOR_PIEZA_J, COLOR_PIEZA_J, COLOR_PIEZA_J, COLOR_PIEZA_J},
    {COLOR_PIEZA_J, 0,             0,             0,             0            },
    {COLOR_PIEZA_O, 0,             0,             0,             0            },
    {0,             COLOR_PIEZA_O, COLOR_PIEZA_O, COLOR_PIEZA_O, 0            },
    {0,             0,             0,             0,             COLOR_PIEZA_O},
    {0,             0,             0,             0,             COLOR_PIEZA_I},
    {COLOR_PIEZA_I, COLOR_PIEZA_I, COLOR_PIEZA_I, COLOR_PIEZA_I, 0            },
};

/* =========================================================
 * TETROMINOS DEL FONDO
 * ========================================================= */
#define CELDAS_POR_PIEZA  4
#define CANTIDAD_TIPOS_BG 7

typedef struct
{
    int col;
    int fila;
} tOffset;

static const tOffset FORMAS[CANTIDAD_TIPOS_BG][CELDAS_POR_PIEZA] = {
    /* I */ {{0,0},{1,0},{2,0},{3,0}},
    /* J */ {{0,0},{1,0},{2,0},{0,1}},
    /* L */ {{0,0},{1,0},{2,0},{2,1}},
    /* O */ {{0,0},{1,0},{0,1},{1,1}},
    /* S */ {{1,0},{2,0},{0,1},{1,1}},
    /* T */ {{0,0},{1,0},{2,0},{1,1}},
    /* Z */ {{0,0},{1,0},{1,1},{2,1}},
};

static const uint8_t COLOR_TIPO[CANTIDAD_TIPOS_BG] = {
    COLOR_PIEZA_I, COLOR_PIEZA_J, COLOR_PIEZA_L, COLOR_PIEZA_O,
    COLOR_PIEZA_S, COLOR_PIEZA_T, COLOR_PIEZA_Z
};

/* =========================================================
 * FUNCIONES DE DIBUJADO INTERNAS
 * ========================================================= */

/*
 * Dibuja un bloque cuadrado con sombra.
 */
static void dibujar_bloque_con_sombra(int x, int y, int tamano, uint8_t color, int desplazamiento_sombra)
{
    /* sombra */
    rellenar_rectangulo(x + desplazamiento_sombra,
                        y + desplazamiento_sombra,
                        x + desplazamiento_sombra + tamano - 1,
                        y + desplazamiento_sombra + tamano - 1,
                        COLOR_SOMBRA);
    /* color */
    rellenar_rectangulo(x,
                        y,
                        x + tamano - 1,
                        y + tamano - 1,
                        color);
}

/*
 * Dibuja un glifo del titulo escalado.
 * Cada celda con valor > 0 se dibuja como un bloque de escala x escala
 * usando el color que tiene la celda (indice de paleta).
 */
static void dibujar_glifo_titulo(const tGlifoTitulo glifo, int x, int y, int escala)
{
    for (int fila = 0; fila < GLIFO_TITULO_ALTO; fila++)
    {
        for (int col = 0; col < GLIFO_TITULO_ANCHO; col++)
        {
            if (glifo[fila][col])
            {
                dibujar_bloque_con_sombra(x + col * escala,
                                           y + fila * escala,
                                           escala,
                                           glifo[fila][col],
                                           SOMBRA_TITULO);
            }
        }
    }
}

/* Centra y dibuja palabra completa. */
static void dibujar_titulo(int centro_x, int y, int escala)
{
    const tGlifoTitulo *letras[LETRAS_TITULO] = { &G_T, &G_E, &G_T, &G_R, &G_I, &G_S };

    int paso_letra         = GLIFO_TITULO_ANCHO * escala + ESPACIADO_ENTRE_LETRAS;
    int ancho_total_titulo = LETRAS_TITULO * paso_letra - ESPACIADO_ENTRE_LETRAS;
    int x_inicio_titulo    = centro_x - ancho_total_titulo / 2;

    for (int i = 0; i < LETRAS_TITULO; i++)
    {
        dibujar_glifo_titulo(*letras[i],
                             x_inicio_titulo + i * paso_letra,
                             y,
                             escala);
    }
}

/* Fila de cuadraditos de colores (arcoiris de piezas). */
static void dibujar_separador(int ancho, int y)
{
    static const uint8_t COLORES_SEPARADOR[] = {
        COLOR_PIEZA_I, COLOR_PIEZA_J, COLOR_PIEZA_L, COLOR_PIEZA_O,
        COLOR_PIEZA_S, COLOR_PIEZA_T, COLOR_PIEZA_Z,
        COLOR_PIEZA_T, COLOR_PIEZA_S, COLOR_PIEZA_O,
        COLOR_PIEZA_L, COLOR_PIEZA_J, COLOR_PIEZA_I
    };

    const int cantidad_bloques = (int)(sizeof COLORES_SEPARADOR / sizeof COLORES_SEPARADOR[0]);

    int x_inicio = (ancho - cantidad_bloques * PASO_BLOQUE_SEPARADOR) / 2;

    for (int i = 0; i < cantidad_bloques; i++)
    {
        dibujar_bloque_con_sombra(x_inicio + i * PASO_BLOQUE_SEPARADOR,
                                  y,
                                  TAMANO_BLOQUE_SEPARADOR,
                                  COLORES_SEPARADOR[i],
                                  SOMBRA_TITULO);
    }
}

/* Ancho en pixeles de una cadena con escala dada (fuente 5x7). */
static int texto_ancho(const char *s, int escala)
{
    const char *p = s;

    while (*p)
    {
        p++;
    }

    int longitud = (int)(p - s);

    return longitud * (GLIFO_TITULO_ANCHO * escala + ESPACIADO_ENTRE_PIXELES) - 1;
}

/*
 * Dibuja texto monocromatico con sombra (escala 1 = fuente pequena).
 * Cada pixel encendido del glifo se dibuja como un bloque de escala x escala.
 */
static void dibujar_texto(const char *s, int x, int y, int escala, uint8_t color)
{
    int paso_caracter = GLIFO_TITULO_ANCHO * escala + ESPACIADO_ENTRE_PIXELES;
    const char *caracter = s;

    while (*caracter)
    {
        const tGlifo *glifo = fuente_buscar_glifo(*caracter);

        if (glifo)
        {
            for (int fila = 0; fila < GLIFO_TITULO_ALTO; fila++)
            {
                for (int col = 0; col < GLIFO_TITULO_ANCHO; col++)
                {
                    if (glifo->b[fila][col])
                    {
                        dibujar_bloque_con_sombra(x + col  * escala,
                                                   y + fila * escala,
                                                   escala,
                                                   color,
                                                   SOMBRA_TEXTO);
                    }
                }
            }
        }

        x += paso_caracter;
        caracter++;
    }
}

/* Dibuja un tetromino pequeno del fondo con sombra. */
static void dibujar_objeto_fondo(const tObjetoFondo *objeto_fondo)
{
    const tOffset *forma        = FORMAS[objeto_fondo->tipo];
    uint8_t        color        = COLOR_TIPO[objeto_fondo->tipo];
    int            pixel_x_base = (int)objeto_fondo->x;
    int            pixel_y_base = (int)objeto_fondo->y;

    for (int celda = 0; celda < CELDAS_POR_PIEZA; celda++)
    {
        int pixel_x = pixel_x_base + forma[celda].col  * PASO_BLOQUE_FONDO;
        int pixel_y = pixel_y_base + forma[celda].fila * PASO_BLOQUE_FONDO;

        dibujar_bloque_con_sombra(pixel_x,
                                  pixel_y,
                                  TAMANO_BLOQUE_FONDO,
                                  color,
                                  SOMBRA_BG);
    }
}

/* Ancho en pixeles de una pieza del fondo segun su tipo. */
static int ancho_objeto_fondo(uint8_t tipo)
{
    const tOffset *forma   = FORMAS[tipo];
    int            max_col = 0;

    for (int celda = 0; celda < CELDAS_POR_PIEZA; celda++)
    {
        if (forma[celda].col > max_col)
        {
             max_col = forma[celda].col;
        }
    }

    return max_col * PASO_BLOQUE_FONDO + TAMANO_BLOQUE_FONDO;
}

/* Alto en pixeles de una pieza del fondo segun su tipo. */
static int alto_objeto_fondo(uint8_t tipo)
{
    const tOffset *forma    = FORMAS[tipo];
    int            max_fila = 0;

    for (int celda = 0; celda < CELDAS_POR_PIEZA; celda++)
    {
        if (forma[celda].fila > max_fila)
        {
            max_fila = forma[celda].fila;
        }
    }

    return max_fila * PASO_BLOQUE_FONDO + TAMANO_BLOQUE_FONDO;
}

/* =========================================================
 * API PUBLICA
 * ========================================================= */

void presentacion_iniciar(PantallaPresentacion *pantalla, int ancho, int alto)
{
    pantalla->temporizador_parpadeo = gbt_temporizador_crear(INTERVALO_PARPADEO_SEG);
    pantalla->mostrar_enter         = 1;

    for (int i = 0; i < MAX_OBJETOS_FONDO; i++)
    {
        tObjetoFondo *objeto_fondo = &pantalla->objetos[i];
        objeto_fondo->tipo         = (uint8_t)(i % CANTIDAD_TIPOS_BG);

        int ancho_objeto = ancho_objeto_fondo(objeto_fondo->tipo);
        int alto_objeto  = alto_objeto_fondo(objeto_fondo->tipo);

        objeto_fondo->x = (float)(rand() % (ancho - ancho_objeto));
        objeto_fondo->y = (float)(rand() % (alto  - alto_objeto));

        float velocidad  = (float)((rand() % 2) + 1);

        objeto_fondo->vx = (rand() & 1) ?  velocidad : -velocidad;
        objeto_fondo->vy = (rand() & 1) ?  velocidad : -velocidad;
    }
}

ResultadoPresentacion presentacion_actualizar(PantallaPresentacion *pantalla, int ancho, int alto)
{
    gbt_procesar_entrada();

    if (gbt_temporizador_consumir(pantalla->temporizador_parpadeo))
    {
        pantalla->mostrar_enter = !pantalla->mostrar_enter;
    }

    /* Mover y rebotar cada tetromino del fondo */
    for (int i = 0; i < MAX_OBJETOS_FONDO; i++)
    {
        tObjetoFondo *objeto_fondo = &pantalla->objetos[i];

        int ancho_objeto = ancho_objeto_fondo(objeto_fondo->tipo);
        int alto_objeto  = alto_objeto_fondo(objeto_fondo->tipo);

        objeto_fondo->x += objeto_fondo->vx;
        objeto_fondo->y += objeto_fondo->vy;

        if (objeto_fondo->x < 0.0f)
        {
            objeto_fondo->x  = 0.0f;
            objeto_fondo->vx = -objeto_fondo->vx;
        }
        else if (objeto_fondo->x + ancho_objeto > ancho)
        {
            objeto_fondo->x  = (float)(ancho - ancho_objeto);
            objeto_fondo->vx = -objeto_fondo->vx;
        }

        if (objeto_fondo->y < 0.0f)
        {
            objeto_fondo->y  = 0.0f;
            objeto_fondo->vy = -objeto_fondo->vy;
        }
        else if (objeto_fondo->y + alto_objeto > alto)
        {
            objeto_fondo->y  = (float)(alto - alto_objeto);
            objeto_fondo->vy = -objeto_fondo->vy;
        }
    }

    if (gbt_tecla_presionada(GBTK_ENTER))  return PRESENTACION_INICIAR;
    if (gbt_tecla_presionada(GBTK_ESCAPE)) return PRESENTACION_SALIR;

    return PRESENTACION_CONTINUAR;
}

void presentacion_dibujar(const PantallaPresentacion *pantalla, int ancho, int alto)
{
    int centro_x = ancho / 2;

    int escala = ancho / DIVISOR_ESCALA;

    if (escala < ESCALA_MINIMA)
    {
        escala = ESCALA_MINIMA;
    }

    int panel_ancho       = ancho - 2 * (ancho / 8);
    int espacio_letras    = (LETRAS_TITULO - 1) * ESPACIADO_ENTRE_LETRAS;
    int max_escala_titulo = (panel_ancho - espacio_letras - 2 * MARGEN_PANEL * escala) / (LETRAS_TITULO * GLIFO_TITULO_ANCHO);
    int escala_titulo     = escala * ESCALA_MULTIPLICADOR_TITULO;

    if (escala_titulo > max_escala_titulo)
    {
        escala_titulo = max_escala_titulo;
    }

    if (escala_titulo < escala)
    {
        escala_titulo = escala;
    }
    /*
     * Altura total del contenido expresada en unidades de escala
     * para que sea proporcional en CGA y VGA.
     */
    int altura_titulo  = GLIFO_TITULO_ALTO * escala_titulo;
    int altura_jugar   = GLIFO_TITULO_ALTO * escala;
    int altura_total   = altura_titulo + ESPACIO_TITULO_SEP * escala + altura_jugar + ESPACIO_JUGAR_SEP  * escala;

    /* El bloque completo queda centrado verticalmente en la pantalla */
    int y_titulo              = (alto - altura_total) / 2;
    int y_separacion_superior = y_titulo + altura_titulo + ESPACIO_TITULO_SEP * escala;
    int y_jugar               = y_separacion_superior + ESPACIO_SEP_JUGAR * escala;
    int y_separacion_inferior = y_jugar + ESPACIO_JUGAR_SEP * escala;

    /* Fondo de ladrillos */
    dibujar_pared_ladrillos_completa();

    /* Tetrominos animados */
    for (int i = 0; i < MAX_OBJETOS_FONDO; i++)
    {
        dibujar_objeto_fondo(&pantalla->objetos[i]);
    }

    /* Panel oscuro */
    int margen   = ancho / 8;
    int panel_x1 = margen;
    int panel_x2 = ancho - margen;
    int panel_y1 = y_titulo - MARGEN_PANEL * escala;
    int panel_y2 = y_separacion_inferior + MARGEN_PANEL * escala;

    rellenar_rectangulo(panel_x1, panel_y1, panel_x2, panel_y2, COLOR_FONDO_TABLERO);

    dibujar_marco_incrustado_en(panel_x1, panel_y1, panel_x2, panel_y2);

    /* Separador superior */
    dibujar_separador(ancho, y_separacion_superior);

    /* Titulo TETRIS */
    dibujar_titulo(centro_x, y_titulo, escala_titulo);

    /* "JUGAR" parpadeante */
    if (pantalla->mostrar_enter)
    {
        dibujar_texto("JUGAR",
                       centro_x - texto_ancho("JUGAR", 1) / 2,
                       y_jugar,
                       1,
                       COLOR_AMARILLO);
    }

    /* Separador inferior */
    dibujar_separador(ancho, y_separacion_inferior);

    /* Volcar a pantalla */
    gbt_volcar_backbuffer();
}

void presentacion_destruir(PantallaPresentacion *pantalla)
{
    gbt_temporizador_destruir(pantalla->temporizador_parpadeo);
    pantalla->temporizador_parpadeo = NULL;
}
