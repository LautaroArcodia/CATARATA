#ifndef VECTOR_H
#define VECTOR_H

/*
 * vector.h — TDA Vector de capacidad estática
 *
 * Implementación propia sin memoria dinámica.
 * Almacena hasta VECTOR_CAPACIDAD_MAX elementos de tamaño fijo
 * en un buffer interno de bytes.
 *
 * Restricción: tam_elem <= VECTOR_ELEM_TAM_MAX bytes.
 * Para tRegistroJugador (~20 bytes), VECTOR_ELEM_TAM_MAX=32 es suficiente.
 */

#define VECTOR_CAPACIDAD_MAX  16   /* máximo de elementos */
#define VECTOR_ELEM_TAM_MAX   32   /* máximo de bytes por elemento */

/* Función de comparación: retorna negativo, 0 o positivo. */
typedef int (*tVectorComparador)(const void *a, const void *b);

typedef struct
{
    unsigned char datos[VECTOR_CAPACIDAD_MAX][VECTOR_ELEM_TAM_MAX];
    int           cantidad;
    int           tam_elem;
} tVector;

/* Inicializa el vector vacío con el tamaño de elemento indicado. */
void vector_inicializar(tVector *v, int tam_elem);

/* Inserta elem al final. Retorna 1 si OK, 0 si el vector está lleno. */
int  vector_insertar_al_final(tVector *v, const void *elem);

/* Copia el elemento en la posición indice hacia dest. */
void vector_obtener(const tVector *v, int indice, void *dest);

/* Sobreescribe el elemento en la posición indice con elem. */
void vector_asignar(tVector *v, int indice, const void *elem);

/* Retorna la cantidad actual de elementos. */
int  vector_cantidad(const tVector *v);

/* Ordena los elementos en su lugar usando burbujeo con el comparador dado. */
void vector_ordenar_burbujeo(tVector *v, tVectorComparador cmp);

#endif /* VECTOR_H */
