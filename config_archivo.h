#ifndef CONFIG_ARCHIVO_H
#define CONFIG_ARCHIVO_H

/*
 * configuración persistente entre sesiones

    Guarda y carga las preferencias del jugador en el archivo "tetris_config.dat".
    La configuración se aplica automáticamente al iniciar el juego y se guarda al salir de la pantalla de opciones.
 *
 * Valores guardados:
 *   resolucion → 0 = CGA (320×200), 1 = VGA (640×480)
 *   velocidad  → 1 a 5 (nivel de dificultad inicial)
 *   paleta     → 0 = Clásica, 1 = Neon, 2 = Retro
 */

#define CONFIG_ARCHIVO "tetris_config.dat"

/* Valores por defecto cuando no existe el archivo de configuración */
#define CONFIG_RESOLUCION_DEFAULT  0   /* CGA */
#define CONFIG_VELOCIDAD_DEFAULT   1   /* nivel 1 */
#define CONFIG_PALETA_DEFAULT      0   /* Clásica */
#define CONFIG_ANCHO_DX_DEFAULT   10   /* ancho tablero modo DX (8-16) */

/* Estructura que se guarda en disco (formato binario) */
typedef struct
{
    int resolucion;
    int velocidad;
    int paleta;
    int ancho_dx;   /* ancho del tablero en modo DX: 8 a 16 columnas */
} tConfigGuardada;

tConfigGuardada config_archivo_cargar(void);

void config_archivo_guardar(int resolucion, int velocidad, int paleta, int ancho_dx);

#endif /* CONFIG_ARCHIVO_H */
