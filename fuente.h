#ifndef FUENTE_H
#define FUENTE_H

/*
 * fuente.h — Fuentes bitmap para el juego
 *
 * Fuente de ancho variable: cada carácter define su propio campo 'ancho'
 * (1-5 px). El bitmap siempre ocupa b[7][5], pero el render solo itera
 * hasta 'ancho' columnas y avanza el cursor por 'ancho + 1' px.
 *
 * Fuente normal  (variable × 7 px): HUD, menús, texto pequeño.
 * Fuente grande (doble escala)    : títulos, puntajes, game over.
 */

/* Ancho máximo del bitmap y alto fijo de todos los glifos */
#define GLIFO_ANCHO  5
#define GLIFO_ALTO   7
#define GLIFO_PASO   (GLIFO_ANCHO + 1)   /* paso fijo de referencia (= 6 px); usar glifo->ancho para render real */

/* Factor de escala de la fuente grande */
#define ESCALA_FUENTE_GRANDE  2

/*
 * Descriptor de un glifo.
 *   c     — carácter ASCII representado
 *   ancho — columnas activas del bitmap (1-5); determina el avance horizontal
 *   b     — mapa de píxeles [fila][columna], solo cols 0..ancho-1 son relevantes
 */
typedef struct
{
    char          c;
    unsigned char ancho;
    unsigned char b[7][5];
} tGlifo;

/*
 * Busca el glifo correspondiente al carácter 'c'.
 * Retorna un puntero al tGlifo, o NULL si el carácter no está definido.
 */
const tGlifo *fuente_buscar_glifo(char c);

#endif /* FUENTE_H */
