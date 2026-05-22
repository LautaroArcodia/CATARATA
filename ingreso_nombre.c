#include "ingreso_nombre.h"
#include "cadena.h"
#include "dibujo.h"
#include "layout.h"
#include "GBT/gbt.h"
#include "fuente.h"

#define INTERVALO_PARPADEO_CURSOR  0.5

#define COLOR_TITULO  COLOR_PIEZA_T
#define COLOR_TEXTO   COLOR_BLANCO
#define COLOR_AYUDA   COLOR_GRIS_OSCURO

#define ESPACIO_TITULO_SUBTITULO   4
#define ESPACIO_SUBTITULO_CAMPO   22
#define ESPACIO_CAMPO_AYUDA        6
#define MARGEN_SUPERIOR_PANEL      8
#define MARGEN_INFERIOR_PANEL     14

void ingreso_nombre_iniciar(PantallaIngresoNombre *pantalla)
{
    pantalla->longitud       = 0;
    pantalla->mostrar_cursor = 1;
    pantalla->timer_cursor   = gbt_temporizador_crear(INTERVALO_PARPADEO_CURSOR);

    /* Vaciar el buffer caracter a caracter */
    for (int i = 0; i <= NOMBRE_MAX_LONGITUD; i++)
    {
        pantalla->texto[i] = '\0';
    }
}

ResultadoIngresoNombre ingreso_nombre_actualizar(PantallaIngresoNombre *pantalla)
{
    gbt_procesar_entrada();

    if (gbt_temporizador_consumir(pantalla->timer_cursor))
    {
        pantalla->mostrar_cursor = !pantalla->mostrar_cursor;
    }

    eGBT_Tecla tecla = gbt_obtener_tecla_presionada();

    /* Letras A–Z: GBT las reporta en minúscula, las guardamos en mayúscula */
    if (tecla >= GBTK_a && tecla <= GBTK_z)
    {
        if (pantalla->longitud < NOMBRE_MAX_LONGITUD)
        {
            pantalla->texto[pantalla->longitud] = (char)(tecla - 'a' + 'A');
            pantalla->longitud++;
            pantalla->texto[pantalla->longitud] = '\0';
        }
    }

    /* RETROCESO: borrar la última letra */
    if (tecla == GBTK_RETROCESO && pantalla->longitud > 0)
    {
        pantalla->longitud--;
        pantalla->texto[pantalla->longitud] = '\0';
    }

    /* ENTER: confirmar (solo si hay al menos una letra) */
    if (tecla == GBTK_ENTER && pantalla->longitud > 0)
        return INGRESO_NOMBRE_CONFIRMAR;

    /* ESCAPE: usar nombre por defecto */
    if (tecla == GBTK_ESCAPE)
    {
        cadena_copiar(NOMBRE_POR_DEFECTO, pantalla->texto, NOMBRE_MAX_LONGITUD);
        pantalla->longitud = cadena_longitud(pantalla->texto);
        return INGRESO_NOMBRE_CONFIRMAR;
    }

    return INGRESO_NOMBRE_CONTINUAR;
}

void ingreso_nombre_dibujar(const PantallaIngresoNombre *pantalla, int ancho, int alto)
{
    int centro_x = ancho / 2;
    int escala   = ancho / 320;

    if (escala < 1) escala = 1;

    int y_titulo    = alto / 2 - 40 * escala;
    int y_subtitulo = y_titulo + (GLIFO_ALTO * ESCALA_FUENTE_GRANDE + ESPACIO_TITULO_SUBTITULO) * escala;
    int y_campo     = y_subtitulo + ESPACIO_SUBTITULO_CAMPO * escala;
    int y_ayuda     = y_campo + (GLIFO_ALTO * ESCALA_FUENTE_GRANDE + ESPACIO_CAMPO_AYUDA) * escala;

    int margen   = ancho / 8;
    int panel_x1 = margen;
    int panel_x2 = ancho - margen;
    int panel_y1 = y_titulo - MARGEN_SUPERIOR_PANEL * escala;
    int panel_y2 = y_ayuda + MARGEN_INFERIOR_PANEL * escala;

    dibujar_pared_ladrillos_completa();

    rellenar_rectangulo(panel_x1, panel_y1, panel_x2, panel_y2, COLOR_FONDO_TABLERO);
    dibujar_marco_incrustado_en(panel_x1, panel_y1, panel_x2, panel_y2);

    texto_grande_centrado("TETRIS", centro_x, y_titulo, COLOR_TITULO);
    texto_centrado_escalado("INGRESE SU NOMBRE:", centro_x, y_subtitulo, COLOR_AYUDA, escala);

    char texto_con_cursor[NOMBRE_MAX_LONGITUD + 2];

    cadena_copiar(pantalla->texto, texto_con_cursor, NOMBRE_MAX_LONGITUD);

    if (pantalla->mostrar_cursor && pantalla->longitud < NOMBRE_MAX_LONGITUD)
    {
        texto_con_cursor[pantalla->longitud]     = '_';
        texto_con_cursor[pantalla->longitud + 1] = '\0';
    }

    texto_grande_centrado(texto_con_cursor, centro_x, y_campo, COLOR_TEXTO);
    texto_centrado_escalado("ENTER:CONFIRMAR  ESC:OMITIR", centro_x, y_ayuda, COLOR_AYUDA, escala);

    gbt_volcar_backbuffer();
}

void ingreso_nombre_destruir(PantallaIngresoNombre *pantalla)
{
    gbt_temporizador_destruir(pantalla->timer_cursor);
    pantalla->timer_cursor = NULL;
}

const char *ingreso_nombre_obtener(const PantallaIngresoNombre *pantalla)
{
    return pantalla->texto;
}
