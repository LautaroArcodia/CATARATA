#include "juego.h"
#include "juego_logica_clasico.h"
#include "juego_logica_dx.h"
#include "cadena.h"
#include "ingreso_nombre.h"
#include "GBT/gbt.h"
#include "layout.h"
#include <stdlib.h>

#define FACTOR_AUMENTO_VELOCIDAD       0.97
#define INTERVALO_REPEAT_MOVIMIENTO_MS 90.0
#define PIEZAS_POR_AUMENTO_VELOCIDAD   10
#define PROPORCION_LOCK_DELAY          0.5
#define CANTIDAD_NIVELES_VELOCIDAD     5

#define PUNTOS_POR_CHEAT 1000

static const double INTERVALO_INICIAL_MS[CANTIDAD_NIVELES_VELOCIDAD] =
{
    1200.0, 900.0, 700.0, 500.0, 300.0
};

static const int PUNTOS_POR_LINEAS[5] =
{
    0, 100, 300, 500, 800
};

#define TAM_BOLSA_MAX CANTIDAD_TIPOS_DX

static int bolsa[TAM_BOLSA_MAX];
static int tam_bolsa    = 0;
static int indice_bolsa = 0;

static void mezclar_bolsa(void)
{
    for (int i = 0; i < tam_bolsa; i++)
    {
        bolsa[i] = i;
    }

    /* Mezcla aleatoria de los índices */
    for (int i = 0; i < tam_bolsa; i++)
    {
        int indice_aleatorio    = rand() % tam_bolsa;

        int tipo_temp           = bolsa[i];
        bolsa[i]                = bolsa[indice_aleatorio];
        bolsa[indice_aleatorio] = tipo_temp;
    }

    indice_bolsa = 0;
}

static eTipoPieza obtener_pieza_de_bolsa(void)
{
    if (indice_bolsa >= tam_bolsa)
    {
        mezclar_bolsa();
    }

    return (eTipoPieza)bolsa[indice_bolsa++];
}

void reiniciar_temporizador_fijacion(tEstadoJuego *estado)
{
    double intervalo_segundos = (estado->intervalo_caida_ms * PROPORCION_LOCK_DELAY) / 1000.0;
    gbt_temporizador_destruir(estado->temporizador_fijacion);
    estado->temporizador_fijacion = gbt_temporizador_crear(intervalo_segundos);
}

static void reiniciar_temporizador_caida(tEstadoJuego *estado)
{
    double intervalo_segundos = estado->intervalo_caida_ms / 1000.0;
    gbt_temporizador_destruir(estado->temporizador_caida_automatica);
    estado->temporizador_caida_automatica = gbt_temporizador_crear(intervalo_segundos);
}

static void generar_pieza_nueva(tEstadoJuego *estado)
{
    movimiento_inicializar_pieza(&estado->pieza_activa, estado->tipo_pieza_siguiente);

    estado->estadisticas_piezas[estado->pieza_activa.tipo]++;

    estado->tipo_pieza_siguiente = obtener_pieza_de_bolsa();

    pieza_calcular_forma(estado->tipo_pieza_siguiente,
                         0,
                         estado->forma_pieza_siguiente);

    estado->pieza_en_suelo = 0;
}

ResultadoJuego fijar_pieza_y_continuar(tEstadoJuego *estado)
{
    uint8_t color_pieza = pieza_obtener_color_paleta(estado->pieza_activa.tipo);

    if (estado->modo == 1)
    {
        tablero_fijar_pieza_dx(&estado->tablero,
                               estado->pieza_activa.columna_tablero,
                               estado->pieza_activa.fila_tablero,
                               estado->pieza_activa.forma_actual,
                               color_pieza);
    }
    else
    {
        tablero_fijar_pieza(&estado->tablero,
                            estado->pieza_activa.columna_tablero,
                            estado->pieza_activa.fila_tablero,
                            estado->pieza_activa.forma_actual,
                            color_pieza);
    }

    int lineas_eliminadas = tablero_eliminar_filas_completas(&estado->tablero);
    estado->cantidad_lineas_eliminadas += lineas_eliminadas;
    estado->puntuacion += PUNTOS_POR_LINEAS[lineas_eliminadas];

    if (tablero_hay_derrota(&estado->tablero))
        return JUEGO_TERMINADO;

    estado->cantidad_piezas_fijadas++;

    if (estado->cantidad_piezas_fijadas % PIEZAS_POR_AUMENTO_VELOCIDAD == 0)
    {
        estado->intervalo_caida_ms *= FACTOR_AUMENTO_VELOCIDAD;
        reiniciar_temporizador_caida(estado);
    }

    generar_pieza_nueva(estado);

    /* Verificar que la nueva pieza tenga espacio para spawnear */
    int spawn_valido;

    if (estado->modo == 1)
    {
        spawn_valido = tablero_movimiento_valido_dx(
                       &estado->tablero,
                       estado->pieza_activa.columna_tablero,
                       estado->pieza_activa.fila_tablero,
                       estado->pieza_activa.forma_actual);
    }
    else
    {
        spawn_valido = tablero_movimiento_valido(
                       &estado->tablero,
                       estado->pieza_activa.columna_tablero,
                       estado->pieza_activa.fila_tablero,
                       estado->pieza_activa.forma_actual);
    }

    if (!spawn_valido)
        return JUEGO_TERMINADO;

    return JUEGO_CONTINUAR;
}

void juego_iniciar(tEstadoJuego *estado, int velocidad_nivel, const char *nombre_jugador, int modo, int param)
{
    estado->modo  = modo;
    estado->param = param;

    /* Inicializar tablero según el modo */
    switch (modo)
    {
        case 1:  tablero_inicializar(&estado->tablero, param);           break;
        default: tablero_inicializar(&estado->tablero, TABLERO_COLUMNAS); break;
    }

    /* Resetear contadores */
    estado->puntuacion                 = 0;
    estado->cantidad_lineas_eliminadas = 0;
    estado->cantidad_piezas_fijadas    = 0;
    estado->pieza_en_suelo             = 0;

    for (int i = 0; i < CANTIDAD_TIPOS_DX; i++)
    {
        estado->estadisticas_piezas[i] = 0;
    }

    /* Velocidad inicial */
    int indice_velocidad = velocidad_nivel - 1;

    if (indice_velocidad < 0)
    {
        indice_velocidad = 0;
    }

    if (indice_velocidad > CANTIDAD_NIVELES_VELOCIDAD - 1)
    {
        indice_velocidad = CANTIDAD_NIVELES_VELOCIDAD - 1;
    }

    estado->nivel_velocidad_inicial = velocidad_nivel;
    estado->intervalo_caida_ms      = INTERVALO_INICIAL_MS[indice_velocidad];

    /* Temporizadores */
    estado->temporizador_caida_automatica   = gbt_temporizador_crear(estado->intervalo_caida_ms / 1000.0);
    estado->temporizador_movimiento_lateral = gbt_temporizador_crear(INTERVALO_REPEAT_MOVIMIENTO_MS / 1000.0);
    estado->temporizador_fijacion           = gbt_temporizador_crear((estado->intervalo_caida_ms * PROPORCION_LOCK_DELAY) / 1000.0);

    /* Nombre del jugador */
    cadena_copiar(nombre_jugador, estado->nombre_jugador, NOMBRE_MAX_LONGITUD);

    /* Bolsa de piezas según el modo */
    switch (modo)
    {
        case 1:  tam_bolsa = CANTIDAD_TIPOS_DX; break;
        default: tam_bolsa = CANTIDAD_TIPOS;    break;
    }
    indice_bolsa = tam_bolsa;   /* fuerza mezcla en la primera extracción */

    /* Primera pieza */
    estado->tipo_pieza_siguiente = obtener_pieza_de_bolsa();

    pieza_calcular_forma(estado->tipo_pieza_siguiente,
                         0,
                         estado->forma_pieza_siguiente);

    generar_pieza_nueva(estado);
}

/*
 * Procesa el cheat de puntos (tecla C). Suma puntos al puntaje del jugador.
 */
void procesar_cheat_puntos(tEstadoJuego *estado)
{
    if (gbt_tecla_presionada(GBTK_c))
    {
        estado->puntuacion += PUNTOS_POR_CHEAT;
    }
}

/* ── Acceso al estado de la bolsa (usado por savegame) ──────────── */

void bolsa_obtener_estado(int *bolsa_out, int *tam_out, int *indice_out)
{
    for (int i = 0; i < tam_bolsa; i++)
    {
        bolsa_out[i] = bolsa[i];
    }

    *tam_out    = tam_bolsa;
    *indice_out = indice_bolsa;
}

void bolsa_restaurar_estado(const int *bolsa_in, int tam, int indice)
{
    tam_bolsa    = tam;
    indice_bolsa = indice;

    for (int i = 0; i < tam; i++)
    {
        bolsa[i] = bolsa_in[i];
    }
}

ResultadoJuego juego_actualizar(tEstadoJuego *estado)
{
    switch (estado->modo)
    {
        case 1:  return dx_actualizar(estado);
        default: return clasico_actualizar(estado);
    }
}

void juego_destruir(tEstadoJuego *estado)
{
    gbt_temporizador_destruir(estado->temporizador_caida_automatica);
    gbt_temporizador_destruir(estado->temporizador_movimiento_lateral);
    gbt_temporizador_destruir(estado->temporizador_fijacion);

    estado->temporizador_caida_automatica   = NULL;
    estado->temporizador_movimiento_lateral = NULL;
    estado->temporizador_fijacion           = NULL;
}
