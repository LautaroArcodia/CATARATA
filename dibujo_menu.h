#ifndef DIBUJO_MENU_H
#define DIBUJO_MENU_H

/*
 * dibujo_menu.h — Funciones de dibujo compartidas por pantallas tipo menú
 *
 * Este módulo agrupa las primitivas de UI que usan las pantallas de
 * menú, configuración y cualquier pantalla con navegación por ítems:
 *   - Texto escalado con sombra
 *   - Flechas de navegación
 *   - Separadores decorativos
 *   - Filas de opción con selector (► LABEL : ◄ VALOR ►)
 *   - Botones simples con selector
 */

#include <stdint.h>

#define SOMBRA_DESPLAZAMIENTO 1 /* Desplazamiento de sombra usado por todos los elementos de menú */

/* Dibuja texto escalado con sombra. Retorna el ancho total dibujado. */
int dm_texto_escalado(const char *s, int x, int y, uint8_t color, int escala);

/* Calcula el ancho en píxeles de un texto escalado (sin dibujarlo). */
int dm_ancho_texto(const char *s, int escala);

/* Flechas de navegación con sombra. */
void dm_flecha_izquierda(int x, int y, uint8_t color);
void dm_flecha_derecha(int x, int y, uint8_t color);

/* Separador decorativo: fila de cuadraditos con colores de piezas. */
void dm_separador(int ancho_pantalla, int y);

/*
 * Fila de opción con selector:  ► LABEL : ◄ VALOR ►
 * Usada por el menú principal y la pantalla de configuración.
 */
void dm_fila_opcion(int centro_x, int y, const char *etiqueta, const char *valor, int seleccionado, int mostrar_flechas, int escala);

/* Botón sin valor: solo etiqueta con selector ► cuando está seleccionado. */
void dm_boton(int centro_x, int y, const char *etiqueta, int seleccionado, int escala);

#endif /* DIBUJO_MENU_H */
