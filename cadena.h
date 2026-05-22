#ifndef CADENA_H
#define CADENA_H

void cadena_copiar(const char *origen, char *destino, int max);

int cadena_comparar(const char *a, const char *b, int max);

int cadena_longitud(const char *s);

char cadena_a_mayuscula(char c);

int cadena_son_iguales_ignorando_mayusculas(const char *a, const char *b, int max);

#endif /* CADENA_H */
