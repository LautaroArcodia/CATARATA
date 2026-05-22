/*
 * juego_dibujo.c — Helpers compartidos y despachador de modos
 *
 * Misma estructura que juego_logica.c:
 *
 *   1. Helpers de dibujo usados por ambos modos (bloques, tablero, HUD, estadísticas).
 *
 *   2. renderizar_base(): ensambla los elementos comunes.
 *
 *   3. Dispatcher: juego_dibujar y juego_dibujar_pantalla_game_over
 *                  eligen qué modo renderizar con un switch.
 *
 * ─── Para agregar un nuevo modo ──────────────────────────────────
 *   1. Crear juego_dibujo_NUEVO.h y juego_dibujo_NUEVO.c con la
 *      función nuevo_dibujar().
 *   2. Agregar #include "juego_dibujo_NUEVO.h" arriba.
 *   3. Agregar un case en juego_dibujar() y
 *      juego_dibujar_pantalla_game_over() abajo.
 * ─────────────────────────────────────────────────────────────────
 */

#include "juego_dibujo_interna.h"
#include "juego_dibujo_clasico.h"
#include "juego_dibujo_dx.h"
#include "cadena.h"
#include "dibujo.h"
#include "gameover.h"
#include "estadisticas.h"
#include "layout.h"
#include "GBT/gbt.h"
#include <stdio.h>

/* Márgenes internos de los paneles */
#define MARGEN_INTERNO_PANEL    5   /* sangría horizontal respecto al borde del panel */
#define MARGEN_SEPARADOR_PANEL  4   /* sangría horizontal de la línea separadora      */

/* Posición y ancho del contenido del HUD */
#define HUD_CONTENIDO_X     (HUD_X1 + MARGEN_INTERNO_PANEL)
#define HUD_CONTENIDO_ANCHO (HUD_X2 - HUD_X1 - MARGEN_INTERNO_PANEL * 2)

/* Espaciados verticales del HUD */
#define ESPACIO_TRAS_NOMBRE         3   /* entre nombre del jugador y separador           */
#define ESPACIO_TRAS_SEPARADOR_HUD  4   /* entre separador y el elemento siguiente        */
#define ESPACIO_TRAS_ETIQUETA       3   /* entre etiqueta "SIGUIENTE" y la caja           */
#define ESPACIO_TRAS_CAJA_PREVIEW   5   /* entre la caja de previsualización y el puntaje */
#define ESPACIO_TRAS_DATO          12   /* entre filas de datos (score, lines, ms)        */
#define ESPACIO_ENTRE_CONTROLES     9   /* entre líneas de controles                      */

/* Espaciados verticales del panel de estadísticas */
#define MARGEN_INTERNO_STATS        4
#define ESPACIO_TITULO_STATS       10
#define ESPACIO_SEPARADOR_STATS     6
#define ESPACIO_EXTRA_MINI_PIEZA    2   /* offset vertical de la mini-pieza respecto al texto */
#define SEPARACION_TEXTO_CONTADOR   4   /* espacio entre mini-pieza y su contador             */

/* =========================================================
 * HELPERS DE BLOQUES
 * ========================================================= */

/*
 * Dibuja un bloque con efecto 3D:
 *   relleno de color, borde superior/izquierdo blanco (luz) y borde inferior/derecho gris (sombra).
 */
void dibujar_bloque_3d(int x, int y, uint8_t color)
{
    const int tamano = TAMANO_BLOQUE_JUEGO;

    /* Relleno interior (1px de margen para los bordes) */
    rellenar_rectangulo(x + 1,
                        y + 1,
                        x + tamano - 2,
                        y + tamano - 2,
                        color);

    /* Bordes de luz (arriba e izquierda) y sombra (abajo y derecha) */
    for (int i = 0; i < tamano; i++)
    {
        dibujar_pixel(x + i,          y,              COLOR_BLANCO);
        dibujar_pixel(x,              y + i,          COLOR_BLANCO);
        dibujar_pixel(x + i,          y + tamano - 1, COLOR_GRIS_SOMBRA);
        dibujar_pixel(x + tamano - 1, y + i,          COLOR_GRIS_SOMBRA);
    }
}

/*
 * Dibuja un bloque plano (sin efecto 3D) con un punto blanco
 * en la esquina superior izquierda como brillo mínimo.
 * Usado para las mini-piezas del panel de estadísticas.
 */
void dibujar_bloque_simple(int x, int y, int tamano, uint8_t color)
{
    rellenar_rectangulo(x,
                        y,
                        x + tamano - 1,
                        y + tamano - 1,
                        color);

    dibujar_pixel(x, y, COLOR_BLANCO);
}

/* =========================================================
 * TABLERO — fondo y bloques fijados
 * ========================================================= */

/*
 * Dibuja el fondo del tablero con un patrón de cuadrícula
 * ajedrezada (negro/gris oscuro) y un marco decorativo.
 */
void dibujar_area_tablero_vacia(const tTablero *tablero)
{
    const int tamano_celda      = TAMANO_BLOQUE_JUEGO;
    const int cantidad_columnas = tablero->columnas;

    for (int fila = 0; fila < TABLERO_FILAS; fila++)
    {
        for (int col = 0; col < cantidad_columnas; col++)
        {
            uint8_t color = ((col + fila) % 2 == 0) ? COLOR_NEGRO : COLOR_FONDO_TABLERO;
            int pixel_x = TABLERO_PIXEL_X + col  * tamano_celda;
            int pixel_y = TABLERO_PIXEL_Y + fila * tamano_celda;

            rellenar_rectangulo(pixel_x,
                                pixel_y,
                                pixel_x + tamano_celda - 1,
                                pixel_y + tamano_celda - 1,
                                color);
        }
    }

    dibujar_marco_incrustado_en(TABLERO_PIXEL_X,
                                 TABLERO_PIXEL_Y,
                                 TABLERO_PIXEL_X + TABLERO_ANCHO_PIXELES(cantidad_columnas) - 1,
                                 TABLERO_PIXEL_Y + TABLERO_ALTO_PIXELES - 1);
}

/*
 * Dibuja todos los bloques que ya están fijados en el tablero (piezas que ya cayeron).
 * Cada celda con valor > 0 se dibuja como un bloque 3D con el color almacenado.
 */
void dibujar_bloques_fijados(const tTablero *tablero)
{
    for (int col = 0; col < tablero->columnas; col++)
    {
        for (int fila = 0; fila < TABLERO_FILAS; fila++)
        {
            uint8_t color = tablero->celdas_ocupadas[col][fila];

            if (color == 0) continue;

            dibujar_bloque_3d(TABLERO_PIXEL_X + col  * TAMANO_BLOQUE_JUEGO,
                              TABLERO_PIXEL_Y + fila * TAMANO_BLOQUE_JUEGO,
                              color);
        }
    }
}

/* =========================================================
 * PANEL HUD — pieza siguiente, puntaje, controles
 * ========================================================= */

/* Dibuja una línea horizontal de separación dentro de un panel. */
static void dibujar_separador(int x1, int x2, int y)
{
    for (int pixel_x = x1; pixel_x <= x2; pixel_x++)
    {
        dibujar_pixel(pixel_x, y, COLOR_MORTERO);
    }
}

/*
 * Dibuja una fila con etiqueta a la izquierda y valor numérico alineado a la derecha dentro del ancho disponible.
 */
static void dibujar_fila_dato(const char *etiqueta, int valor, int inicio_x, int y, int ancho_disponible, int escala)
{
    char texto_valor[16];
    sprintf(texto_valor, "%d", valor);

    texto_escalado(etiqueta, inicio_x, y, COLOR_GRIS_OSCURO, escala);

    int ancho_valor = cadena_longitud(texto_valor) * GLIFO_PASO * escala;
    texto_escalado(texto_valor, inicio_x + ancho_disponible - ancho_valor, y, COLOR_BLANCO, escala);
}

/*
 * Dibuja el panel lateral derecho
 */
void dibujar_panel_hud(const tEstadoJuego *estado)
{
    const int escala          = TAMANO_BLOQUE_JUEGO / 8;
    const int contenido_x     = HUD_CONTENIDO_X;
    const int contenido_ancho = HUD_CONTENIDO_ANCHO;
    const int centro_x        = contenido_x + contenido_ancho / 2;

    const int tamano_bloque_preview = TAMANO_BLOQUE_SIGUIENTE;
    const int lado_caja_preview     = DIMENSION_MATRIZ * tamano_bloque_preview;
    const int caja_preview_x        = centro_x - lado_caja_preview / 2;

    /* Posiciones verticales calculadas desde HUD_Y1 */
    const int y_nombre        = HUD_Y1 + MARGEN_INTERNO_PANEL * escala;
    const int y_sep_nombre    = y_nombre + (GLIFO_ALTO + ESPACIO_TRAS_NOMBRE) * escala;
    const int y_etiqueta_sig  = y_sep_nombre + ESPACIO_TRAS_SEPARADOR_HUD * escala;
    const int y_caja_preview  = y_etiqueta_sig + (GLIFO_ALTO + ESPACIO_TRAS_ETIQUETA) * escala;
    const int y_sep_score     = y_caja_preview + lado_caja_preview + ESPACIO_TRAS_CAJA_PREVIEW * escala;
    const int y_score         = y_sep_score + ESPACIO_TRAS_SEPARADOR_HUD * escala;
    const int y_sep_lines     = y_score + ESPACIO_TRAS_DATO * escala;
    const int y_lines         = y_sep_lines + ESPACIO_TRAS_SEPARADOR_HUD * escala;
    const int y_sep_ms        = y_lines + ESPACIO_TRAS_DATO * escala;
    const int y_ms            = y_sep_ms + ESPACIO_TRAS_SEPARADOR_HUD * escala;
    const int y_sep_controles = y_ms + ESPACIO_TRAS_DATO * escala;
    const int y_ctrl_1        = y_sep_controles + ESPACIO_TRAS_SEPARADOR_HUD * escala;
    const int y_ctrl_2        = y_ctrl_1 + ESPACIO_ENTRE_CONTROLES * escala;
    const int y_ctrl_3        = y_ctrl_2 + ESPACIO_ENTRE_CONTROLES * escala;
    const int y_ctrl_4        = y_ctrl_3 + ESPACIO_ENTRE_CONTROLES * escala;

    /* Fondo y marco del panel */
    rellenar_rectangulo(HUD_X1, HUD_Y1, HUD_X2, HUD_Y2, COLOR_FONDO_TABLERO);
    dibujar_marco_incrustado_en(HUD_X1, HUD_Y1, HUD_X2, HUD_Y2);

    /* ── Nombre del jugador ───────────────────────────── */
    texto_centrado_escalado(estado->nombre_jugador, centro_x, y_nombre, COLOR_LADRILLO_CLARO, escala);

    /* ── Previsualización de la pieza siguiente ───────── */
    dibujar_separador(contenido_x, contenido_x + contenido_ancho, y_sep_nombre);
    texto_centrado_escalado("SIGUIENTE", centro_x, y_etiqueta_sig, COLOR_LADRILLO_CLARO, escala);

    /* Fondo negro de la caja de previsualización */
    rellenar_rectangulo(caja_preview_x,
                        y_caja_preview,
                        caja_preview_x + lado_caja_preview - 1,
                        y_caja_preview + lado_caja_preview - 1,
                        COLOR_NEGRO);

    /* Borde exterior */
    for (int i = -1; i <= lado_caja_preview; i++)
    {
        dibujar_pixel(caja_preview_x + i,                 y_caja_preview - 1,                 COLOR_MORTERO);
        dibujar_pixel(caja_preview_x + i,                 y_caja_preview + lado_caja_preview, COLOR_MORTERO);
        dibujar_pixel(caja_preview_x - 1,                 y_caja_preview + i,                 COLOR_MORTERO);
        dibujar_pixel(caja_preview_x + lado_caja_preview, y_caja_preview + i,                 COLOR_MORTERO);
    }

    /* Borde interior (sombra arriba/izq, luz abajo/der) */
    for (int i = 0; i < lado_caja_preview; i++)
    {
        dibujar_pixel(caja_preview_x + i,                     y_caja_preview,                         COLOR_GRIS_SOMBRA);
        dibujar_pixel(caja_preview_x,                         y_caja_preview + i,                     COLOR_GRIS_SOMBRA);
        dibujar_pixel(caja_preview_x + i,                     y_caja_preview + lado_caja_preview - 1, COLOR_LADRILLO_CLARO);
        dibujar_pixel(caja_preview_x + lado_caja_preview - 1, y_caja_preview + i,                     COLOR_LADRILLO_CLARO);
    }

    /* Dibujar la pieza siguiente dentro de la caja */
    uint8_t color_siguiente = pieza_obtener_color_paleta(estado->tipo_pieza_siguiente);

    for (int fila = 0; fila < DIMENSION_MATRIZ; fila++)
    {
        for (int col = 0; col < DIMENSION_MATRIZ; col++)
        {
            if (estado->forma_pieza_siguiente[fila][col])
            {
                int bloque_x = caja_preview_x + col  * tamano_bloque_preview;
                int bloque_y = y_caja_preview  + fila * tamano_bloque_preview;

                rellenar_rectangulo(bloque_x, bloque_y,
                                    bloque_x + tamano_bloque_preview - 2,
                                    bloque_y + tamano_bloque_preview - 2,
                                    color_siguiente);

                dibujar_pixel(bloque_x, bloque_y, COLOR_BLANCO);
            }
        }
    }

    /* ── Puntaje ──────────────────────────────────────── */
    dibujar_separador(contenido_x, contenido_x + contenido_ancho, y_sep_score);
    dibujar_fila_dato("SCORE", estado->puntuacion, contenido_x, y_score, contenido_ancho, escala);

    /* ── Líneas eliminadas ────────────────────────────── */
    dibujar_separador(contenido_x, contenido_x + contenido_ancho, y_sep_lines);
    dibujar_fila_dato("LINES", estado->cantidad_lineas_eliminadas, contenido_x, y_lines, contenido_ancho, escala);

    /* ── Velocidad actual (ms entre caídas) ───────────── */
    dibujar_separador(contenido_x, contenido_x + contenido_ancho, y_sep_ms);
    dibujar_fila_dato("MS", (int)estado->intervalo_caida_ms, contenido_x, y_ms, contenido_ancho, escala);

    /* ── Controles ────────────────────────────────────── */
    dibujar_separador(contenido_x, contenido_x + contenido_ancho, y_sep_controles);
    texto_escalado("A=ROTAR IZQUIERDA", contenido_x, y_ctrl_1, COLOR_LADRILLO_BASE, escala);
    texto_escalado("Z=ROTAR DERECHA",   contenido_x, y_ctrl_2, COLOR_LADRILLO_BASE, escala);
    texto_escalado("X=CAER",            contenido_x, y_ctrl_3, COLOR_LADRILLO_BASE, escala);
    texto_escalado("P=PAUSA",           contenido_x, y_ctrl_4, COLOR_LADRILLO_BASE, escala);
}

/* =========================================================
 * PANEL DE ESTADÍSTICAS POR PIEZA
 * ========================================================= */

/*
 * Dibuja una pieza en miniatura (sin efecto 3D) para mostrar
 * en el panel de estadísticas junto a su contador.
 */
static void dibujar_mini_pieza(eTipoPieza tipo, int x, int y, int tamano_bloque)
{
    tFormaPieza forma;
    pieza_calcular_forma(tipo, 0, forma);
    uint8_t color = pieza_obtener_color_paleta(tipo);

    for (int fila = 0; fila < DIMENSION_MATRIZ; fila++)
    {
        for (int col = 0; col < DIMENSION_MATRIZ; col++)
        {
            if (forma[fila][col])
            {
                dibujar_bloque_simple(x + col * tamano_bloque,
                                      y + fila * tamano_bloque,
                                      tamano_bloque,
                                      color);
            }
        }
    }
}

/*
 * Dibuja el panel lateral izquierdo con la cantidad de cada
 * tipo de pieza que apareció durante la partida.
 * Muestra una mini-pieza junto a su contador para cada uno
 * de los 7 tipos clásicos.
 */
void dibujar_panel_estadisticas(const tEstadoJuego *estado)
{
    const int escala      = TAMANO_BLOQUE_JUEGO / 8;
    const int panel_x1    = STATS_X1;
    const int panel_y1    = STATS_Y1;
    const int panel_x2    = STATS_X2;
    const int panel_y2    = STATS_Y2;
    const int contenido_x = panel_x1 + MARGEN_INTERNO_STATS * escala;

    /* Cantidad de tipos segun el modo activo */
    const int cantidad_tipos = (estado->modo == 1) ? CANTIDAD_TIPOS_DX : CANTIDAD_TIPOS;

    /* Overhead fijo: margen superior + titulo + separador + margen inferior */
    const int overhead        = (MARGEN_INTERNO_STATS
                               + ESPACIO_TITULO_STATS
                               + ESPACIO_SEPARADOR_STATS
                               + MARGEN_INTERNO_STATS) * escala;

    /* alto_fila se divide en el espacio disponible para que todas
       las piezas quepan sin superponer el tablero, independientemente
       de cuantos tipos haya en el modo activo */
    const int alto_disponible   = (panel_y2 - panel_y1) - overhead;
    const int alto_fila         = alto_disponible / cantidad_tipos;

    /* tamano_mini_pieza ocupa 2/3 del alto_fila para dejar margen vertical */
    const int tamano_mini_pieza = (alto_fila * 2 / 3) / DIMENSION_MATRIZ;
    const int ancho_mini_pieza  = tamano_mini_pieza * DIMENSION_MATRIZ;
    const int separacion_texto  = ancho_mini_pieza + SEPARACION_TEXTO_CONTADOR * escala;

    const int y_titulo    = panel_y1 + MARGEN_INTERNO_STATS * escala;
    const int y_separador = y_titulo + ESPACIO_TITULO_STATS * escala;
    const int y_entradas  = y_separador + ESPACIO_SEPARADOR_STATS * escala;

    /* Fondo y marco */
    rellenar_rectangulo(panel_x1, panel_y1, panel_x2, panel_y2, COLOR_FONDO_TABLERO);
    dibujar_marco_incrustado_en(panel_x1, panel_y1, panel_x2, panel_y2);

    texto_escalado("STATS", contenido_x, y_titulo, COLOR_LADRILLO_CLARO, escala);

    dibujar_separador(panel_x1 + MARGEN_SEPARADOR_PANEL * escala,
                      panel_x2 - MARGEN_SEPARADOR_PANEL * escala,
                      y_separador);

    /* Lista de piezas con sus contadores */
    for (int indice_tipo = 0; indice_tipo < cantidad_tipos; indice_tipo++)
    {
        int y_fila = y_entradas + indice_tipo * alto_fila;

        dibujar_mini_pieza((eTipoPieza)indice_tipo,
                            contenido_x,
                            y_fila,
                            tamano_mini_pieza);

        char texto_valor[16];
        sprintf(texto_valor, "%d", estado->estadisticas_piezas[indice_tipo]);
        texto_escalado(texto_valor, contenido_x + separacion_texto, y_fila, COLOR_BLANCO, escala);
    }
}


/* =========================================================
 * BASE COMPARTIDA
 * ========================================================= */

/*
 * Renderiza los elementos comunes a ambos modos:
 *      pared de fondo, HUD, cuadrícula del tablero, bloques fijados y panel de estadísticas.
 */
void renderizar_base(const tEstadoJuego *estado)
{
    dibujar_pared_ladrillos_completa();
    dibujar_panel_hud(estado);
    dibujar_area_tablero_vacia(&estado->tablero);
    dibujar_bloques_fijados(&estado->tablero);
    dibujar_panel_estadisticas(estado);
}

/* =========================================================
 * API PÚBLICA
 * =========================================================
 */

void juego_dibujar(const tEstadoJuego *estado, int ancho, int alto)
{
    (void)ancho;
    (void)alto;

    switch (estado->modo)
    {
        case 1:  dx_dibujar(estado);      break;
        default: clasico_dibujar(estado); break;
    }

    gbt_volcar_backbuffer();
}

void juego_dibujar_pantalla_game_over(const tEstadoJuego *estado, int mostrar_instrucciones)
{
    switch (estado->modo)
    {
        case 1:  dx_dibujar(estado);      break;
        default: clasico_dibujar(estado); break;
    }

    int mejor_puntaje = estadisticas_obtener_mejor_puntaje(estado->nombre_jugador, estado->modo);

    gameover_dibujar(estado->puntuacion, mejor_puntaje, mostrar_instrucciones);

    gbt_volcar_backbuffer();
}
