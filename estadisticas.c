#include "estadisticas.h"
#include "vector.h"
#include "cadena.h"
#include <stdio.h>

/*
 * estadisticas.c — Estadísticas de juego por jugador
 *
 * Usa tVector (TDA propio, sin memoria dinámica) para almacenar los
 * registros en memoria. El archivo binario guarda el tVector completo.
 * Si el archivo no existe o es incompatible, se empieza vacío.
 * Cada modo de juego usa un archivo distinto.
 */

/* =========================================================
 * FUNCIONES INTERNAS
 * ========================================================= */

static const char *nombre_archivo(int modo)
{
    return (modo == 1) ? ESTADISTICAS_ARCHIVO_DX : ESTADISTICAS_ARCHIVO_CLASICO;
}

static tVector cargar_estadisticas(int modo)
{
    tVector jugadores;
    vector_inicializar(&jugadores, (int)sizeof(tRegistroJugador));

    FILE *archivo = fopen(nombre_archivo(modo), "rb");

    if (archivo == NULL)
        return jugadores;

    fread(&jugadores, sizeof(tVector), 1, archivo);
    fclose(archivo);

    if (jugadores.tam_elem != (int)sizeof(tRegistroJugador) ||
        jugadores.cantidad < 0 || jugadores.cantidad > VECTOR_CAPACIDAD_MAX)
    {
        vector_inicializar(&jugadores, (int)sizeof(tRegistroJugador));
    }

    return jugadores;
}

static void guardar_estadisticas(const tVector *jugadores, int modo)
{
    FILE *archivo = fopen(nombre_archivo(modo), "wb");

    if (archivo == NULL)
        return;

    fwrite(jugadores, sizeof(tVector), 1, archivo);
    fclose(archivo);
}

/*
    Busca un jugador por nombre. Retorna su índice o -1 si no existe.
*/
static int buscar_jugador(const tVector *jugadores, const char *nombre)
{
    tRegistroJugador registro;
    int i;

    for (i = 0; i < vector_cantidad(jugadores); i++)
    {
        vector_obtener(jugadores, i, &registro);
        if (cadena_comparar(registro.nombre, nombre, ESTADISTICAS_MAX_NOMBRE) == 0)
            return i;
    }
    return -1;
}

/* Comparador descendente por mejor_puntaje para vector_ordenar_burbujeo. */
static int cmp_puntaje_desc(const void *a, const void *b)
{
    const tRegistroJugador *ja = (const tRegistroJugador *)a;
    const tRegistroJugador *jb = (const tRegistroJugador *)b;
    return jb->mejor_puntaje - ja->mejor_puntaje;
}

/* =========================================================
 * API PUBLICA
 * ========================================================= */

void estadisticas_registrar(const char *nombre_jugador, int puntaje, int modo)
{
    tVector          jugadores = cargar_estadisticas(modo);
    tRegistroJugador registro;
    int indice = buscar_jugador(&jugadores, nombre_jugador);

    if (indice >= 0)
    {
        vector_obtener(&jugadores, indice, &registro);

        if (puntaje > registro.mejor_puntaje)
            registro.mejor_puntaje = puntaje;

        registro.partidas_jugadas++;
        vector_asignar(&jugadores, indice, &registro);
    }
    else if (vector_cantidad(&jugadores) < ESTADISTICAS_MAX_JUGADORES)
    {
        cadena_copiar(nombre_jugador, registro.nombre, ESTADISTICAS_MAX_NOMBRE - 1);
        registro.mejor_puntaje    = puntaje;
        registro.partidas_jugadas = 1;
        vector_insertar_al_final(&jugadores, &registro);
    }
    else
    {
        /* Ranking lleno: reemplazar al jugador con el peor puntaje si el nuevo lo supera. */
        tRegistroJugador peor;
        int indice_peor  = 0;
        int i;

        vector_obtener(&jugadores, 0, &peor);
        int puntaje_peor = peor.mejor_puntaje;

        for (i = 1; i < vector_cantidad(&jugadores); i++)
        {
            vector_obtener(&jugadores, i, &registro);
            if (registro.mejor_puntaje < puntaje_peor)
            {
                puntaje_peor = registro.mejor_puntaje;
                indice_peor  = i;
            }
        }

        if (puntaje > puntaje_peor)
        {
            cadena_copiar(nombre_jugador, registro.nombre, ESTADISTICAS_MAX_NOMBRE - 1);
            registro.mejor_puntaje    = puntaje;
            registro.partidas_jugadas = 1;
            vector_asignar(&jugadores, indice_peor, &registro);
        }
    }

    guardar_estadisticas(&jugadores, modo);
}

int estadisticas_obtener_mejor_puntaje(const char *nombre_jugador, int modo)
{
    tVector          jugadores = cargar_estadisticas(modo);
    tRegistroJugador registro;
    int indice = buscar_jugador(&jugadores, nombre_jugador);

    if (indice < 0)
        return 0;

    vector_obtener(&jugadores, indice, &registro);
    return registro.mejor_puntaje;
}

int estadisticas_obtener_ranking(tRegistroJugador *jugadores_ranking, int capacidad, int modo)
{
    tVector jugadores = cargar_estadisticas(modo);
    int     cantidad  = vector_cantidad(&jugadores);
    int     i;

    if (cantidad > capacidad)
        cantidad = capacidad;

    vector_ordenar_burbujeo(&jugadores, cmp_puntaje_desc);

    for (i = 0; i < cantidad; i++)
        vector_obtener(&jugadores, i, &jugadores_ranking[i]);

    return cantidad;
}
