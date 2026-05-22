#include "vector.h"

/*
 * vector.c — TDA Vector de capacidad estática
 */

/* =========================================================
 * HELPER INTERNO
 * ========================================================= */

static void copiar_bytes(void *dest, const void *src, int n)
{
    unsigned char       *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;
    int i;

    for (i = 0; i < n; i++)
    {
        d[i] = s[i];
    }
}

/* =========================================================
 * API PÚBLICA
 * ========================================================= */

void vector_inicializar(tVector *v, int tam_elem)
{
    v->cantidad = 0;
    v->tam_elem = tam_elem;
}

int vector_insertar_al_final(tVector *v, const void *elem)
{
    if (v->cantidad >= VECTOR_CAPACIDAD_MAX)
        return 0;

    copiar_bytes(v->datos[v->cantidad], elem, v->tam_elem);
    v->cantidad++;
    return 1;
}

void vector_obtener(const tVector *v, int indice, void *dest)
{
    copiar_bytes(dest, v->datos[indice], v->tam_elem);
}

void vector_asignar(tVector *v, int indice, const void *elem)
{
    copiar_bytes(v->datos[indice], elem, v->tam_elem);
}

int vector_cantidad(const tVector *v)
{
    return v->cantidad;
}

void vector_ordenar_burbujeo(tVector *v, tVectorComparador cmp)
{
    unsigned char temp[VECTOR_ELEM_TAM_MAX];
    int i, j;

    for (i = 0; i < v->cantidad - 1; i++)
    {
        for (j = 0; j < v->cantidad - 1 - i; j++)
        {
            if (cmp(v->datos[j], v->datos[j + 1]) > 0)
            {
                copiar_bytes(temp,            v->datos[j],     v->tam_elem);
                copiar_bytes(v->datos[j],     v->datos[j + 1], v->tam_elem);
                copiar_bytes(v->datos[j + 1], temp,            v->tam_elem);
            }
        }
    }
}
