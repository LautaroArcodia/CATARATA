#ifndef PRESENTACION_H
#define PRESENTACION_H

#include "GBT/gbt.h"
#include <stdint.h>

/* =========================================================
 * RESULTADO — lo que devuelve presentacion_actualizar()
 * ========================================================= */
typedef enum
{
    PRESENTACION_CONTINUAR = 0,
    PRESENTACION_INICIAR   = 1,   /* ENTER presionado → ir al juego */
    PRESENTACION_SALIR     = 2    /* ESC presionado   → cerrar       */
} ResultadoPresentacion;

/* =========================================================
 * OBJETOS DEL FONDO
 * Tetrominos pequeños que rebotan detrás del título.
 * ========================================================= */
#define MAX_OBJETOS_FONDO 12

typedef struct
{
    float   x, y;    /* posición actual (píxeles lógicos)  */
    float   vx, vy;  /* velocidad       (px por frame)     */
    uint8_t tipo;    /* 0=I 1=J 2=L 3=O 4=S 5=T 6=Z       */
} tObjetoFondo;

/* =========================================================
 * ESTADO DE LA PANTALLA DE PRESENTACIÓN
 * ========================================================= */
typedef struct
{
    tGBT_Temporizador *temporizador_parpadeo;  /* controla el blink de "ENTER:JUGAR" */
    int                mostrar_enter;   /* 1 = visible, 0 = oculto            */
    tObjetoFondo       objetos[MAX_OBJETOS_FONDO];
} PantallaPresentacion;

/* =========================================================
 * API PÚBLICA
 * ========================================================= */
void presentacion_iniciar(PantallaPresentacion *p, int ancho, int alto);

ResultadoPresentacion presentacion_actualizar(PantallaPresentacion *p, int ancho, int alto);

void presentacion_dibujar(const PantallaPresentacion *p, int ancho, int alto);

void presentacion_destruir(PantallaPresentacion *p);

#endif /* PRESENTACION_H */
