/*
 * main.c — Tetris con GBT
 *
 * Integrantes del grupo:
 *
 *   Apellido: Arcodia, Lautaro Gabriel
 *   DNI: 41588362
 *   Usuario: LautaroArcodia
 *   Entrega: Sí
 *
 *   Apellido: Martinez, Pablo
 *   DNI: 33989156
 *   Usuario: pabale5833
 *   Entrega: Sí
 *
 *   Apellido: Zotelo, Julian
 *   DNI: 42536473
 *   Usuario: julianzotelo
 *   Entrega: No
 *
 */

#include "GBT/gbt.h"
#include "presentacion.h"
#include "menu.h"
#include "config.h"
#include "config_archivo.h"
#include "juego.h"
#include "gameover.h"
#include "estadisticas.h"
#include "ingreso_nombre.h"
#include "pausa.h"
#include "historial.h"
#include "layout.h"
#include "resolucion.h"
#include "args.h"
#include "savegame.h"

#include <stdlib.h>
#include <time.h>

/* =========================================================
 * PALETAS DE COLOR (15 colores cada una)
 *
 * Indices:
 *   0=negro  1=I(cian)  2=J(azul)  3=L(naranja)  4=O(amarillo)
 *   5=S(verde)  6=T(violeta)  7=Z(rojo)  8=blanco
 *   9=gris  10=sombra  11=fondo tablero
 *   12=ladrillo  13=ladrillo claro  14=mortero
 * ========================================================= */

static void aplicar_paleta_clasica(void)
{
    tGBT_ColorRGB paleta[] = {
        {  0,   0,   0},   /*  0 negro              */
        {  0, 240, 240},   /*  1 cian    — pieza I  */
        {  0,   0, 220},   /*  2 azul    — pieza J  */
        {240, 140,   0},   /*  3 naranja — pieza L  */
        {240, 240,   0},   /*  4 amarillo— pieza O  */
        {  0, 220,   0},   /*  5 verde   — pieza S  */
        {160,   0, 240},   /*  6 violeta — pieza T  */
        {220,   0,   0},   /*  7 rojo    — pieza Z  */
        {255, 255, 255},   /*  8 blanco             */
        { 60,  60,  60},   /*  9 gris oscuro        */
        { 40,  40,  40},   /* 10 gris sombra        */
        {  8,   8,  28},   /* 11 fondo tablero      */
        {110,  48,  16},   /* 12 ladrillo base      */
        {155,  78,  35},   /* 13 ladrillo claro     */
        { 22,  18,  14},   /* 14 mortero            */
    };
    gbt_aplicar_paleta(paleta, 15, GBT_FORMATO_888);
}

static void aplicar_paleta_neon(void)
{
    tGBT_ColorRGB paleta[] = {
        {  0,   0,   0},
        {  0, 255, 255},
        {  0, 100, 255},
        {255, 140,   0},
        {255, 255,   0},
        {  0, 255,   0},
        {220,   0, 255},
        {255,   0,  80},
        {255, 255, 255},
        { 80,  80,  80},
        { 40,  40,  40},
        {  0,   0,  30},
        { 40,   0,  80},
        { 70,   0, 130},
        { 15,   0,  35},
    };
    gbt_aplicar_paleta(paleta, 15, GBT_FORMATO_888);
}

static void aplicar_paleta_retro(void)
{
    tGBT_ColorRGB paleta[] = {
        {  0,   0,   0},
        {255, 200,   0},
        {180, 100,   0},
        {220,  80,   0},
        {255, 230,  60},
        {  0, 180,   0},
        { 80, 200,   0},
        {200,  40,   0},
        {255, 230, 180},
        { 80,  60,   0},
        { 40,  40,  40},
        { 10,   8,   0},
        { 60,  40,  10},
        { 90,  65,  20},
        { 25,  15,   3},
    };
    gbt_aplicar_paleta(paleta, 15, GBT_FORMATO_888);
}

static void aplicar_paleta(int indice_paleta)
{
    switch (indice_paleta)
    {
        case 1:  aplicar_paleta_neon();    break;
        case 2:  aplicar_paleta_retro();   break;
        default: aplicar_paleta_clasica(); break;
    }
}

/* =========================================================
 * VENTANA
 * ========================================================= */
static int abrir_ventana(int ancho, int alto, int escala)
{
    gbt_destruir_ventana();
    return gbt_crear_ventana("TETRIS",
                            (uint16_t)ancho,
                            (uint16_t)alto,
                            (uint8_t)escala);
}

/* =========================================================
 * ESTADOS DE LA MAQUINA
 * ========================================================= */
typedef enum
{
    ESTADO_PRESENTACION,
    ESTADO_MENU,
    ESTADO_CONFIGURACION,
    ESTADO_HISTORIAL,
    ESTADO_INGRESO_NOMBRE,
    ESTADO_CONFIRMAR_SALIDA,
    ESTADO_JUEGO,
    ESTADO_PAUSA,
    ESTADO_GAME_OVER,
    ESTADO_SALIR
} eEstadoMaquina;

/* =========================================================
 * MAIN
 * ========================================================= */
int main(int argc, char *argv[])
{
    srand((unsigned int)time(NULL));

    /* Parsear argumentos de linea de comandos */
    tArgumentos args = args_parsear(argc, argv);

    /* Cargar configuracion guardada desde disco */
    tConfigGuardada config_guardada = config_archivo_cargar();

    /* Los argumentos tienen prioridad sobre la configuracion guardada */
    eResolucion resolucion_actual = (args.resolucion != RES_CGA) ? args.resolucion : (eResolucion)config_guardada.resolucion;

    int paleta_actual     = config_guardada.paleta;
    int velocidad_inicial = config_guardada.velocidad;
    int ancho_dx_actual   = config_guardada.ancho_dx;

    resolucion_inicializar(resolucion_actual);

    int escala_ventana = (args.escala > 0) ? args.escala : ESCALA_INICIAL;

    /* Inicializar GBT y abrir ventana */
    if (gbt_iniciar() < 0)
        return 1;

    if (abrir_ventana(PANTALLA_ANCHO, PANTALLA_ALTO, escala_ventana) < 0)
    {
        gbt_cerrar();
        return 1;
    }

    aplicar_paleta(paleta_actual);

    /* Declarar pantallas */
    eEstadoMaquina        estado_actual = ESTADO_PRESENTACION;
    PantallaPresentacion  pantalla_presentacion;
    PantallaMenu          pantalla_menu;
    PantallaConfig        pantalla_config;
    PantallaHistorial     pantalla_historial;
    PantallaIngresoNombre pantalla_nombre;
    tEstadoJuego          estado_juego;
    tPantallaGameOver     pantalla_gameover;
    PantallaPausa         pantalla_pausa;
    tPantallaConfirmacion pantalla_confirmacion;

    presentacion_iniciar(&pantalla_presentacion, PANTALLA_ANCHO, PANTALLA_ALTO);

    /* Bucle principal */

    /* Variables de resultado para cada pantalla. */
    ResultadoPresentacion  resultado_presentacion;
    ResultadoMenu          resultado_menu;
    ResultadoConfig        resultado_config;
    ResultadoHistorial     resultado_historial;
    ResultadoIngresoNombre resultado_nombre;
    ResultadoJuego         resultado_juego;
    ResultadoPausa         resultado_pausa;
    eResultadoGameOver     resultado_gameover;
    eResultadoConfirmacion resultado_confirmacion;
    eResolucion            nueva_resolucion;
    int                    nueva_paleta;
    int                    nueva_velocidad;
    int                    nueva_ancho_dx;
    int                    modo_actual;
    const char            *nombre_ingresado;

    while (estado_actual != ESTADO_SALIR)
    {
        switch (estado_actual)
        {
            /* ─────────── PRESENTACION ─────────── */
            case ESTADO_PRESENTACION:

                resultado_presentacion = presentacion_actualizar(&pantalla_presentacion, PANTALLA_ANCHO, PANTALLA_ALTO);

                presentacion_dibujar(&pantalla_presentacion, PANTALLA_ANCHO, PANTALLA_ALTO);

                if (resultado_presentacion == PRESENTACION_INICIAR)
                {
                    presentacion_destruir(&pantalla_presentacion);
                    menu_iniciar(&pantalla_menu);
                    estado_actual = ESTADO_MENU;
                }
                else if (resultado_presentacion == PRESENTACION_SALIR)
                {
                    presentacion_destruir(&pantalla_presentacion);
                    estado_actual = ESTADO_SALIR;
                }
                break;

            /* ─────────── MENU ─────────── */
            case ESTADO_MENU:

                resultado_menu = menu_actualizar(&pantalla_menu);

                menu_dibujar(&pantalla_menu, PANTALLA_ANCHO, PANTALLA_ALTO);

                if (resultado_menu == MENU_JUGAR)
                {
                    modo_actual = pantalla_menu.modo;
                    menu_destruir(&pantalla_menu);
                    ingreso_nombre_iniciar(&pantalla_nombre);
                    estado_actual = ESTADO_INGRESO_NOMBRE;
                }
                else if (resultado_menu == MENU_CONTINUAR_PARTIDA)
                {
                    if (savegame_existe())
                    {
                        menu_destruir(&pantalla_menu);
                        savegame_cargar(&estado_juego);
                        estado_actual = ESTADO_JUEGO;
                    }
                }
                else if (resultado_menu == MENU_HISTORIAL)
                {
                    int modo = pantalla_menu.modo;
                    menu_destruir(&pantalla_menu);
                    historial_iniciar(&pantalla_historial, modo);
                    estado_actual = ESTADO_HISTORIAL;
                }
                else if (resultado_menu == MENU_CONFIGURACION)
                {
                    menu_destruir(&pantalla_menu);
                    config_iniciar(&pantalla_config, resolucion_actual, paleta_actual,
                                   velocidad_inicial, ancho_dx_actual);
                    estado_actual = ESTADO_CONFIGURACION;
                }
                else if (resultado_menu == MENU_VOLVER)
                {
                    menu_destruir(&pantalla_menu);
                    presentacion_iniciar(&pantalla_presentacion, PANTALLA_ANCHO, PANTALLA_ALTO);
                    estado_actual = ESTADO_PRESENTACION;
                }
                break;

            /* ─────────── HISTORIAL ─────────── */
            case ESTADO_HISTORIAL:

                resultado_historial = historial_actualizar(&pantalla_historial);

                historial_dibujar(&pantalla_historial, PANTALLA_ANCHO, PANTALLA_ALTO);

                if (resultado_historial == HISTORIAL_VOLVER)
                {
                    historial_destruir(&pantalla_historial);
                    menu_iniciar(&pantalla_menu);
                    estado_actual = ESTADO_MENU;
                }
                break;

            /* ─────────── CONFIGURACION ─────────── */
            case ESTADO_CONFIGURACION:
                resultado_config = config_actualizar(&pantalla_config);

                config_dibujar(&pantalla_config, PANTALLA_ANCHO, PANTALLA_ALTO);

                if (resultado_config == CONFIG_VOLVER)
                {
                    nueva_resolucion = config_obtener_resolucion(&pantalla_config);
                    nueva_paleta     = config_obtener_paleta(&pantalla_config);
                    nueva_velocidad  = config_obtener_velocidad(&pantalla_config);
                    nueva_ancho_dx   = config_obtener_ancho_dx(&pantalla_config);
                    config_destruir(&pantalla_config);

                    if (nueva_paleta != paleta_actual)
                    {
                        paleta_actual = nueva_paleta;
                        aplicar_paleta(paleta_actual);
                    }

                    velocidad_inicial = nueva_velocidad;
                    ancho_dx_actual   = nueva_ancho_dx;

                    if (nueva_resolucion != resolucion_actual)
                    {
                        resolucion_actual = nueva_resolucion;
                        resolucion_inicializar(resolucion_actual);

                        if (args.escala == 0)
                            escala_ventana = ESCALA_INICIAL;

                        if (abrir_ventana(PANTALLA_ANCHO, PANTALLA_ALTO, escala_ventana) < 0)
                        {
                            estado_actual = ESTADO_SALIR;
                            break;
                        }
                        aplicar_paleta(paleta_actual);
                    }

                    menu_iniciar(&pantalla_menu);
                    estado_actual = ESTADO_MENU;
                }
                break;

            /* ─────────── INGRESO DE NOMBRE ─────────── */
            case ESTADO_INGRESO_NOMBRE:

                resultado_nombre = ingreso_nombre_actualizar(&pantalla_nombre);

                ingreso_nombre_dibujar(&pantalla_nombre, PANTALLA_ANCHO, PANTALLA_ALTO);

                if (resultado_nombre == INGRESO_NOMBRE_CONFIRMAR)
                {
                    nombre_ingresado = ingreso_nombre_obtener(&pantalla_nombre);
                    juego_iniciar(&estado_juego, velocidad_inicial, nombre_ingresado, modo_actual, ancho_dx_actual);
                    ingreso_nombre_destruir(&pantalla_nombre);
                    estado_actual = ESTADO_JUEGO;
                }
                break;

            /* ─────────── JUEGO ─────────── */
            case ESTADO_JUEGO:

                resultado_juego = juego_actualizar(&estado_juego);

                juego_dibujar(&estado_juego, PANTALLA_ANCHO, PANTALLA_ALTO);

                if (resultado_juego == JUEGO_TERMINADO)
                {
                    estadisticas_registrar(estado_juego.nombre_jugador, estado_juego.puntuacion, estado_juego.modo);
                    savegame_borrar();
                    gameover_iniciar(&pantalla_gameover);
                    estado_actual = ESTADO_GAME_OVER;
                }
                else if (resultado_juego == JUEGO_PAUSAR)
                {
                    pausa_iniciar(&pantalla_pausa);
                    estado_actual = ESTADO_PAUSA;
                }
                else if (resultado_juego == JUEGO_PEDIR_GUARDAR)
                {
                    confirmacion_salida_iniciar(&pantalla_confirmacion);
                    estado_actual = ESTADO_CONFIRMAR_SALIDA;
                }
                else if (resultado_juego == JUEGO_VOLVER)
                {
                    juego_destruir(&estado_juego);
                    menu_iniciar(&pantalla_menu);
                    estado_actual = ESTADO_MENU;
                }
                break;

            /* ─────────── PAUSA ─────────── */
            case ESTADO_PAUSA:

                resultado_pausa = pausa_actualizar(&pantalla_pausa);

                pausa_dibujar(&pantalla_pausa, PANTALLA_ANCHO, PANTALLA_ALTO);

                if (resultado_pausa == PAUSA_REANUDAR)
                {
                    pausa_destruir(&pantalla_pausa);
                    estado_actual = ESTADO_JUEGO;
                }
                else if (resultado_pausa == PAUSA_GUARDAR)
                {
                    savegame_guardar(&estado_juego);
                }
                else if (resultado_pausa == PAUSA_IR_AL_MENU)
                {
                    pausa_destruir(&pantalla_pausa);
                    juego_destruir(&estado_juego);
                    menu_iniciar(&pantalla_menu);
                    estado_actual = ESTADO_MENU;
                }
                break;

            /* ─────────── GAME OVER ─────────── */
            case ESTADO_GAME_OVER:

                resultado_gameover = gameover_actualizar(&pantalla_gameover);

                juego_dibujar_pantalla_game_over(&estado_juego, pantalla_gameover.mostrar_instrucciones);

                if (resultado_gameover == GAMEOVER_IR_AL_MENU)
                {
                    gameover_destruir(&pantalla_gameover);
                    juego_destruir(&estado_juego);
                    menu_iniciar(&pantalla_menu);
                    estado_actual = ESTADO_MENU;
                }
                else if (resultado_gameover == GAMEOVER_IR_A_PRESENTACION)
                {
                    gameover_destruir(&pantalla_gameover);
                    juego_destruir(&estado_juego);
                    presentacion_iniciar(&pantalla_presentacion, PANTALLA_ANCHO, PANTALLA_ALTO);
                    estado_actual = ESTADO_PRESENTACION;
                }
                break;

            /* ─────────── CONFIRMAR SALIDA ─────────── */
            case ESTADO_CONFIRMAR_SALIDA:

                resultado_confirmacion = confirmacion_salida_actualizar(&pantalla_confirmacion);

                juego_dibujar_pantalla_confirmacion(&estado_juego, &pantalla_confirmacion);

                if (resultado_confirmacion == CONF_SALIDA_GUARDAR_Y_SALIR)
                {
                    savegame_guardar(&estado_juego);
                    confirmacion_salida_destruir(&pantalla_confirmacion);
                    juego_destruir(&estado_juego);
                    menu_iniciar(&pantalla_menu);
                    estado_actual = ESTADO_MENU;
                }
                else if (resultado_confirmacion == CONF_SALIDA_SALIR_SIN_GUARDAR)
                {
                    confirmacion_salida_destruir(&pantalla_confirmacion);
                    juego_destruir(&estado_juego);
                    menu_iniciar(&pantalla_menu);
                    estado_actual = ESTADO_MENU;
                }
                else if (resultado_confirmacion == CONF_SALIDA_CANCELAR)
                {
                    confirmacion_salida_destruir(&pantalla_confirmacion);
                    estado_actual = ESTADO_JUEGO;
                }
                break;

            default:
                break;
        }

        gbt_esperar(MS_POR_FRAME);
    }

    gbt_destruir_ventana();
    gbt_cerrar();
    return 0;
}
