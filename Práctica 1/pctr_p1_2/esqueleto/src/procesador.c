#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constantes
#define MAX_LONGITUD_LINEA 1024
#define MAX_LONGITUD_PATRON 100

// Funcion principal
int main(int argc, char *argv[])
{
    char linea[MAX_LONGITUD_LINEA];   // Linea leida del archivo
    char patron[MAX_LONGITUD_PATRON]; // Patron a buscar
    int numero_linea = 0;             // Numero de linea

    if (argc != 3) // Comprobar si se pasan los argumentos necesarios para el programa
    {
        fprintf(stderr, "ERROR. Uso: %s <ruta_archivo> <patron>\n", argv[0]); // Mensaje de error
        return EXIT_FAILURE;                                                  // Salir con error
    }

    char *ruta_archivo = argv[1];              // Ruta del archivo
    if (strlen(argv[2]) >= MAX_LONGITUD_LINEA) // Comprobar si el patron es demasiado largo
    {
        fprintf(stderr, "ERROR. El patron es demasiado largo.\n"); // Mensaje de error
        return EXIT_FAILURE;                                       // Salir con error
    }
    strncpy(patron, argv[2], MAX_LONGITUD_PATRON); // Copiar el patron

    FILE *puntero_archivo = fopen(ruta_archivo, "r"); // Abrir el archivo en modo lectura
    if (puntero_archivo == NULL)                      // Comprobar si se ha podido abrir el archivo
    {
        fprintf(stderr, "Error: no se pudo abrir el archivo %s\n", ruta_archivo); // Mensaje de error
        return EXIT_FAILURE;           // Salir con error
    }

    while (fgets(linea, MAX_LONGITUD_LINEA, puntero_archivo) != NULL) // Lee una linea del archivo
    {
        numero_linea++; // Incrementa el contador de lineas

        if (strstr(linea, patron) != NULL) // Comprobar si el patron se encuentra en la linea
        {
            printf("[PROCESADOR %d] Patron '%s' encontrado en la linea %d\n", getpid(), patron, numero_linea); // Imprimir el resultado
        }
    }

    fclose(puntero_archivo); // Cerrar el archivo

    return EXIT_SUCCESS; // Salir con exito
}