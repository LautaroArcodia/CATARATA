#ifndef ESTADISTICAS_H
#define ESTADISTICAS_H

/*
 * estadisticas.h — Estadísticas de juego por jugador
 *
 * Guarda y carga un historial de partidas en un archivo binario
 * separado por modo de juego:
 *   CLASICO → tetris_stats_clasico.dat
 *   DX      → tetris_stats_dx.dat
 *
 * Cada vez que termina una partida, se actualiza el mejor puntaje
 * del jugador y su cantidad de partidas jugadas.
 *
 * Formato del archivo: binario, estructura tArchivoEstadisticas.
 */

#define ESTADISTICAS_MAX_JUGADORES  10
#define ESTADISTICAS_MAX_NOMBRE     11   /* 10 caracteres + '\0' */
#define ESTADISTICAS_ARCHIVO_CLASICO "tetris_stats_clasico.dat"
#define ESTADISTICAS_ARCHIVO_DX      "tetris_stats_dx.dat"

/* Registro de un jugador en el historial */
typedef struct {
    char nombre[ESTADISTICAS_MAX_NOMBRE];
    int  mejor_puntaje;
    int  partidas_jugadas;
} tRegistroJugador;

/*
 * Registra el resultado de una partida en el archivo del modo indicado.
 *
 * Si el jugador ya existe en el historial, actualiza su mejor puntaje
 * si 'puntaje' lo supera, e incrementa sus partidas jugadas.
 * Si no existe, lo agrega (hasta el máximo de jugadores permitido).
 */
void estadisticas_registrar(const char *nombre_jugador, int puntaje, int modo);

/*
 * Devuelve el mejor puntaje histórico del jugador indicado en el modo dado.
 * Retorna 0 si el jugador no está en el historial.
 */
int estadisticas_obtener_mejor_puntaje(const char *nombre_jugador, int modo);

/*
 * Llena el array con hasta 'max' jugadores del modo indicado,
 * ordenados por mejor_puntaje de mayor a menor.
 * Retorna la cantidad de jugadores encontrados.
 */
int estadisticas_obtener_ranking(tRegistroJugador *jugadores_ranking, int capacidad, int modo);

#endif /* ESTADISTICAS_H */
