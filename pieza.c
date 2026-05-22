#include "pieza.h"
#include "dibujo.h"   /* COLOR_PIEZA_* */

/* =========================================================
 * FORMAS BASE de las 7 piezas clásicas (sin rotación)
 * ========================================================= */

static const tFormaPieza FORMA_BASE_I = {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}
};

static const tFormaPieza FORMA_BASE_O = {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 1, 1, 0},
    {0, 0, 1, 1, 0},
    {0, 0, 0, 0, 0}
};

static const tFormaPieza FORMA_BASE_T = {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0}
};

static const tFormaPieza FORMA_BASE_S = {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0},
    {0, 0, 0, 0, 0}
};

static const tFormaPieza FORMA_BASE_Z = {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 1, 1, 0, 0},
    {0, 0, 1, 1, 0},
    {0, 0, 0, 0, 0}
};

static const tFormaPieza FORMA_BASE_L = {
    {0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 1, 0},
    {0, 0, 0, 0, 0}
};

static const tFormaPieza FORMA_BASE_J = {
    {0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 1, 1, 0, 0},
    {0, 0, 0, 0, 0}
};

/* =========================================================
 * FORMAS BASE de las 4 piezas exclusivas del modo DX
 *
 * TIPO_X   (x) — 1 solo mino, centrado
 * TIPO_C   (c) — 5 minos en forma de C
 * TIPO_P   (p) — 5 minos en forma de P
 * TIPO_CRUZ(*) — 5 minos en forma de cruz (+)
 * ========================================================= */

static const tFormaPieza FORMA_BASE_X = {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}
};

static const tFormaPieza FORMA_BASE_C = {
    {0, 0, 0, 0, 0},
    {0, 1, 1, 0, 0},
    {0, 1, 0, 0, 0},
    {0, 1, 1, 0, 0},
    {0, 0, 0, 0, 0}
};

static const tFormaPieza FORMA_BASE_P = {
    {0, 0, 0, 0, 0},
    {0, 1, 1, 0, 0},
    {0, 1, 1, 0, 0},
    {0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0}
};

static const tFormaPieza FORMA_BASE_CRUZ = {
    {0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 1, 1, 1, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0}
};

/* =========================================================
 * TABLAS DE DATOS por tipo (11 entradas: 7 clásicas + 4 DX)
 * ========================================================= */

static const tFormaPieza *TABLA_FORMAS_BASE[CANTIDAD_TIPOS_DX] = {
    &FORMA_BASE_I, &FORMA_BASE_O, &FORMA_BASE_T,
    &FORMA_BASE_S, &FORMA_BASE_Z, &FORMA_BASE_L, &FORMA_BASE_J,
    /* DX */
    &FORMA_BASE_X, &FORMA_BASE_C, &FORMA_BASE_P, &FORMA_BASE_CRUZ
};

/*
 * Colores de paleta.
 * Índices 1-7: piezas clásicas.
 * Índices 8-11 reutilizan el blanco (8) y tonos ya definidos en las
 * paletas de main.c. No se necesita agregar colores nuevos.
 *
 * TIPO_X    → 8  (blanco — un solo mino, fácil de identificar)
 * TIPO_C    → 2  (azul   — mismo que pieza J, aceptable visualmente)
 * TIPO_P    → 3  (naranja— mismo que pieza L, aceptable visualmente)
 * TIPO_CRUZ → 6  (violeta— mismo que pieza T, aceptable visualmente)
 */
static const uint8_t TABLA_COLORES_PALETA[CANTIDAD_TIPOS_DX] = {
    COLOR_PIEZA_I, COLOR_PIEZA_O, COLOR_PIEZA_T,
    COLOR_PIEZA_S, COLOR_PIEZA_Z, COLOR_PIEZA_L, COLOR_PIEZA_J,   /* clásicas */
    COLOR_BLANCO, COLOR_PIEZA_J, COLOR_PIEZA_L, COLOR_PIEZA_T     /* DX       */
};

/*
 * Posición inicial de spawn.
 * Las piezas DX usan las mismas columnas de inicio que las clásicas
 * centradas. La fila inicial varía: X y Cruz son simétricas (-2) y las
 * de 3 filas usan -3.
 */
static const int TABLA_COLUMNA_INICIAL[CANTIDAD_TIPOS_DX] = {
    3, 3, 3, 3, 3, 3, 3,   /* clásicas */
    4, 3, 3, 3              /* DX       */
};

static const int TABLA_FILA_INICIAL[CANTIDAD_TIPOS_DX] = {
    -2, -3, -3, -3, -3, -3, -3,   /* clásicas */
    -2, -3, -3, -3                 /* DX       */
};


/* =========================================================
 * COPIA DE FORMA
 * ========================================================= */

void pieza_copiar_forma(const tFormaPieza origen, tFormaPieza destino)
{
    for (int i = 0; i < DIMENSION_MATRIZ; i++)
    {
        for (int j = 0; j < DIMENSION_MATRIZ; j++)
        {
            destino[i][j] = origen[i][j];
        }
    }
}


/* =========================================================
 * OPERACIONES AUXILIARES SOBRE LA GRILLA 5×5
 * ========================================================= */

static void transponer(tFormaPieza mat)
{
    for (int i = 0; i < DIMENSION_MATRIZ; i++)
    {
        for (int j = i + 1; j < DIMENSION_MATRIZ; j++)
        {
            uint8_t temp = mat[i][j];
            mat[i][j]   = mat[j][i];
            mat[j][i]   = temp;
        }
    }
}

static void invertir_filas(tFormaPieza mat)
{
    for (int i = 0; i < DIMENSION_MATRIZ; i++)
    {
        for (int j = 0; j < DIMENSION_MATRIZ / 2; j++)
        {
            uint8_t temp                     = mat[i][j];
            mat[i][j]                        = mat[i][DIMENSION_MATRIZ - 1 - j];
            mat[i][DIMENSION_MATRIZ - 1 - j] = temp;
        }
    }
}

static void invertir_columnas(tFormaPieza mat)
{
    for (int i = 0; i < DIMENSION_MATRIZ / 2; i++)
    {
        for (int j = 0; j < DIMENSION_MATRIZ; j++)
        {
            uint8_t temp                     = mat[i][j];
            mat[i][j]                        = mat[DIMENSION_MATRIZ - 1 - i][j];
            mat[DIMENSION_MATRIZ - 1 - i][j] = temp;
        }
    }
}


/* =========================================================
 * ROTACIONES
 *
 * Horario     = transponer + invertir filas
 * Antihorario = transponer + invertir columnas
 * ========================================================= */

void pieza_rotar_horario(const tFormaPieza entrada, tFormaPieza salida)
{
    pieza_copiar_forma(entrada, salida);
    transponer(salida);
    invertir_filas(salida);
}

void pieza_rotar_antihorario(const tFormaPieza entrada, tFormaPieza salida)
{
    pieza_copiar_forma(entrada, salida);
    transponer(salida);
    invertir_columnas(salida);
}

/* =========================================================
 * API PÚBLICA
 * ========================================================= */

static void pieza_obtener_forma_base(eTipoPieza tipo, tFormaPieza destino)
{
    pieza_copiar_forma(*TABLA_FORMAS_BASE[tipo], destino);
}

void pieza_calcular_forma(eTipoPieza tipo, int num_rotaciones, tFormaPieza destino)
{
    pieza_obtener_forma_base(tipo, destino);

    for (int i = 0; i < num_rotaciones; i++)
    {
        pieza_rotar_horario(destino, destino);
    }
}

int     pieza_columna_inicial     (eTipoPieza tipo) { return TABLA_COLUMNA_INICIAL[tipo];  }
int     pieza_fila_inicial        (eTipoPieza tipo) { return TABLA_FILA_INICIAL[tipo];     }
uint8_t pieza_obtener_color_paleta(eTipoPieza tipo) { return TABLA_COLORES_PALETA[tipo];   }
