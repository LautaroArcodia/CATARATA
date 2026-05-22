#include "config_archivo.h"
#include <stdio.h>

/*
 *   Configuración persistente entre sesiones.
 */
/*
    Carga la configuración desde el archivo. Si el archivo no existe, devuelve los valores por defecto.
*/
tConfigGuardada config_archivo_cargar(void)
{
    tConfigGuardada config;

    config.resolucion = CONFIG_RESOLUCION_DEFAULT;
    config.velocidad  = CONFIG_VELOCIDAD_DEFAULT;
    config.paleta     = CONFIG_PALETA_DEFAULT;
    config.ancho_dx   = CONFIG_ANCHO_DX_DEFAULT;

    FILE *archivo = fopen(CONFIG_ARCHIVO, "rb");

    if (archivo == NULL)
        return config;   /* primera ejecución */

    fread(&config, sizeof(tConfigGuardada), 1, archivo);
    fclose(archivo);

    /* Validar que los valores estén en rangos aceptables */
    if (config.resolucion < 0 || config.resolucion > 1)
    {
        config.resolucion = CONFIG_RESOLUCION_DEFAULT;
    }

    if (config.velocidad < 1 || config.velocidad > 5)
    {
        config.velocidad = CONFIG_VELOCIDAD_DEFAULT;
    }

    if (config.paleta < 0 || config.paleta > 2)
    {
         config.paleta = CONFIG_PALETA_DEFAULT;
    }

    if (config.ancho_dx < 8 || config.ancho_dx > 16)
    {
         config.ancho_dx = CONFIG_ANCHO_DX_DEFAULT;
    }

    return config;
}

/*
    Guarda la configuración en el archivo. Crea el archivo si no existe, o lo sobreescribe si ya existe.
*/
void config_archivo_guardar(int resolucion, int velocidad, int paleta, int ancho_dx)
{
    tConfigGuardada config;
    config.resolucion = resolucion;
    config.velocidad  = velocidad;
    config.paleta     = paleta;
    config.ancho_dx   = ancho_dx;

    FILE *archivo = fopen(CONFIG_ARCHIVO, "wb");

    if (archivo == NULL)
        return;   /* no se pudo crear el archivo */

    fwrite(&config, sizeof(tConfigGuardada), 1, archivo);
    fclose(archivo);
}
