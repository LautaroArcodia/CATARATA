/*
 * Parseo de argumentos de linea de comandos
 *
 * Recorre argv buscando pares "-r VALOR" y "-e VALOR".
 * Los argumentos pueden ir en cualquier orden.
 * Los argumentos desconocidos se ignoran.
 */

#include "args.h"
#include "cadena.h"
#include <stdlib.h>

tArgumentos args_parsear(int argc, char *argv[])
{
    tArgumentos args;
    args.resolucion   = RES_CGA;    /* por defecto */
    args.escala       = 0;          /* 0 = usar el default de la resolucion */

    for (int i = 1; i < argc; i++)
    {
        /* -r CGA|VGA — resolucion logica */
        if (cadena_comparar(argv[i], "-r", 3) == 0 && i + 1 < argc)
        {
            i++;
            args.resolucion = resolucion_desde_cadena(argv[i]);
        }
        /* -e N — factor de escala de ventana */
        else if (cadena_comparar(argv[i], "-e", 3) == 0 && i + 1 < argc)
        {
            i++;
            int escala = atoi(argv[i]);

            if (escala >= 1 && escala <= 4)
            {
                args.escala = escala;
            }
        }
    }

    return args;
}
