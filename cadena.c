/* Operaciones basicas sobre cadenas de caracteres */

#include "cadena.h"

/* =========================================================
 * cadena_copiar — copia hasta 'max' caracteres de origen a destino
 * ========================================================= */
void cadena_copiar(const char *origen, char *destino, int max)
{
    const char *p_origen  = origen;
    char       *p_destino = destino;

    while (max > 0 && *p_origen != '\0')
    {
        *p_destino = *p_origen;
        p_origen++;
        p_destino++;
        max--;
    }

    *p_destino = '\0';
}

/* =========================================================
 * cadena_comparar — compara hasta 'max' caracteres
 *                   retorna 0 si son iguales, positivo/negativo si difieren.
 * ========================================================= */
int cadena_comparar(const char *a, const char *b, int max)
{
    const char *p1 = a;
    const char *p2 = b;

    while (max > 0)
    {
        if (*p1 != *p2)
            return (unsigned char)*p1 - (unsigned char)*p2;

        if (*p1 == '\0')
            return 0;

        p1++;
        p2++;
        max--;
    }

    return 0;
}

/* =========================================================
 * cadena_longitud — cuenta caracteres hasta el '\0'
 * ========================================================= */
int cadena_longitud(const char *s)
{
    const char *p = s;

    while (*p != '\0')
    {
        p++;
    }

    return (int)(p - s);
}

/* =========================================================
 * cadena_a_mayuscula — convierte un caracter a mayuscula
 * ========================================================= */
char cadena_a_mayuscula(char c)
{
    if (c >= 'a' && c <= 'z')
        return c - 32;

    return c;
}

/* =========================================================
 * cadena_son_iguales_ignorando_mayusculas
 *
 * Compara dos cadenas hasta 'max' caracteres, tratando
 * 'a'-'z' y 'A'-'Z' como iguales.
 * Retorna 1 si son iguales, 0 si no.
 * ========================================================= */
int cadena_son_iguales_ignorando_mayusculas(const char *a, const char *b, int max)
{
    const char *p1 = a;
    const char *p2 = b;

    while (max > 0)
    {
        if (cadena_a_mayuscula(*p1) != cadena_a_mayuscula(*p2))
            return 0;

        if (*p1 == '\0')
            return 1;

        p1++;
        p2++;
        max--;
    }

    return 1;
}
