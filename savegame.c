#include "savegame.h"
#include "juego_logica_interna.h"
#include "pieza.h"
#include "cadena.h"
#include "GBT/gbt.h"
#include <stdio.h>

/*
 * savegame.c — Guardado y carga de partida
 *
 * Guarda todo el estado necesario para continuar una partida:
 * tablero, pieza activa, puntaje, velocidad y bolsa de piezas.
 *
 * Los temporizadores NO se guardan (son punteros a recursos del sistema).
 * Al cargar, se recrean con los intervalos guardados.
 *
 * El formato es un struct plano seguido de los datos de la bolsa de
 * piezas. Todo se escribe con fwrite/fread igual que config_archivo.c.
 */

/* =========================================================
 * ESTRUCTURA DE DATOS GUARDADOS
 *
 * Contiene todos los campos de tEstadoJuego excepto los
 * temporizadores (que se recrean al cargar).
 * ========================================================= */
typedef struct
{
    /* Modo y parámetro */
    int modo;
    int param;

    /* Tablero completo (array estático, se guarda directo) */
    tTablero tablero;

    /* Pieza activa y siguiente */
    tPiezaActiva pieza_activa;
    eTipoPieza   tipo_pieza_siguiente;
    tFormaPieza  forma_pieza_siguiente;

    /* Contadores */
    int puntuacion;
    int cantidad_lineas_eliminadas;
    int cantidad_piezas_fijadas;
    int estadisticas_piezas[CANTIDAD_TIPOS_DX];

    /* Velocidad */
    int    nivel_velocidad_inicial;
    double intervalo_caida_ms;

    /* Lock delay */
    int pieza_en_suelo;

    /* Jugador */
    char nombre_jugador[11];

    /* Estado de la bolsa de piezas */
    int bolsa[CANTIDAD_TIPOS_DX];
    int tam_bolsa;
    int indice_bolsa;

} tDatosSavegame;


/* =========================================================
 * CONSTANTES DE TEMPORIZADORES (iguales a juego_logica.c)
 *
 * Se repiten aquí para poder recrear los temporizadores al
 * cargar sin exponer constantes internas de juego_logica.c.
 * ========================================================= */
#define INTERVALO_REPEAT_MOVIMIENTO_MS 90.0
#define PROPORCION_LOCK_DELAY          0.5


/* =========================================================
 * API PÚBLICA
 * ========================================================= */

int savegame_guardar(const tEstadoJuego *estado)
{
    FILE *archivo = fopen(SAVEGAME_ARCHIVO, "wb");

    if (archivo == NULL)
        return 0;

    tDatosSavegame datos;

    /* Copiar todos los campos del estado al struct de guardado */
    datos.modo                       = estado->modo;
    datos.param                      = estado->param;
    datos.tablero                    = estado->tablero;
    datos.pieza_activa               = estado->pieza_activa;
    datos.tipo_pieza_siguiente       = estado->tipo_pieza_siguiente;
    datos.puntuacion                 = estado->puntuacion;
    datos.cantidad_lineas_eliminadas = estado->cantidad_lineas_eliminadas;
    datos.cantidad_piezas_fijadas    = estado->cantidad_piezas_fijadas;
    datos.nivel_velocidad_inicial    = estado->nivel_velocidad_inicial;
    datos.intervalo_caida_ms         = estado->intervalo_caida_ms;
    datos.pieza_en_suelo             = estado->pieza_en_suelo;

    pieza_copiar_forma(estado->forma_pieza_siguiente, datos.forma_pieza_siguiente);
    cadena_copiar(estado->nombre_jugador, datos.nombre_jugador, 10);

    for (int i = 0; i < CANTIDAD_TIPOS_DX; i++)
    {
        datos.estadisticas_piezas[i] = estado->estadisticas_piezas[i];
    }

    /* Obtener el estado actual de la bolsa de piezas */
    bolsa_obtener_estado(datos.bolsa, &datos.tam_bolsa, &datos.indice_bolsa);

    /* Escribir todo de una vez */
    fwrite(&datos, sizeof(tDatosSavegame), 1, archivo);
    fclose(archivo);

    return 1;
}

int savegame_cargar(tEstadoJuego *estado)
{
    FILE *archivo = fopen(SAVEGAME_ARCHIVO, "rb");

    if (archivo == NULL)
        return 0;

    tDatosSavegame datos;

    if (fread(&datos, sizeof(tDatosSavegame), 1, archivo) != 1)
    {
        fclose(archivo);
        return 0;
    }

    fclose(archivo);

    /* Restaurar todos los campos del estado */
    estado->modo                       = datos.modo;
    estado->param                      = datos.param;
    estado->tablero                    = datos.tablero;
    estado->pieza_activa               = datos.pieza_activa;
    estado->tipo_pieza_siguiente       = datos.tipo_pieza_siguiente;
    estado->puntuacion                 = datos.puntuacion;
    estado->cantidad_lineas_eliminadas = datos.cantidad_lineas_eliminadas;
    estado->cantidad_piezas_fijadas    = datos.cantidad_piezas_fijadas;
    estado->nivel_velocidad_inicial    = datos.nivel_velocidad_inicial;
    estado->intervalo_caida_ms         = datos.intervalo_caida_ms;
    estado->pieza_en_suelo             = datos.pieza_en_suelo;

    pieza_copiar_forma(datos.forma_pieza_siguiente, estado->forma_pieza_siguiente);
    cadena_copiar(datos.nombre_jugador, estado->nombre_jugador, 10);

    for (int i = 0; i < CANTIDAD_TIPOS_DX; i++)
    {
        estado->estadisticas_piezas[i] = datos.estadisticas_piezas[i];
    }

    /* Restaurar la bolsa de piezas */
    bolsa_restaurar_estado(datos.bolsa, datos.tam_bolsa, datos.indice_bolsa);

    /*
     * Recrear los temporizadores (no se serializan porque son
     * punteros a recursos del sistema operativo).
     */
    estado->temporizador_caida_automatica =
        gbt_temporizador_crear(estado->intervalo_caida_ms / 1000.0);

    estado->temporizador_movimiento_lateral =
        gbt_temporizador_crear(INTERVALO_REPEAT_MOVIMIENTO_MS / 1000.0);

    estado->temporizador_fijacion =
        gbt_temporizador_crear((estado->intervalo_caida_ms * PROPORCION_LOCK_DELAY) / 1000.0);

    return 1;
}

int savegame_existe(void)
{
    FILE *archivo = fopen(SAVEGAME_ARCHIVO, "rb");

    if (archivo == NULL)
        return 0;

    fclose(archivo);
    return 1;
}

void savegame_borrar(void)
{
    remove(SAVEGAME_ARCHIVO);
}
