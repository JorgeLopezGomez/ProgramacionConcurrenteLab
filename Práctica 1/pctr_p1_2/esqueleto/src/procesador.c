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
    char line[MAX_LONGITUD_LINEA];    // Linea leida del archivo
    char patron[MAX_LONGITUD_PATRON]; // Patron a buscar
    int lineNum = 0;                  // Numero de linea

    if (argc != 3) // Comprobar si se pasan los argumentos necesarios para el programa
    {
        fprintf(stderr, "ERROR. Uso: %s <ruta_archivo> <patron>\n", argv[0]); // Mensaje de error
        return EXIT_FAILURE;                                                  // Salir con error
    }

    char *filePath = argv[1];                  // Ruta del archivo
    if (strlen(argv[2]) >= MAX_LONGITUD_LINEA) // Comprobar si el patron es demasiado largo
    {
        fprintf(stderr, "ERROR. El patron es demasiado largo.\n"); // Mensaje de error
        return EXIT_FAILURE;                                       // Salir con error
    }
    strncpy(patron, argv[2], MAX_LONGITUD_PATRON); // Copiar el patron

    FILE *fp = fopen(filePath, "r"); // Abrir el archivo en modo lectura
    if (fp == NULL)                  // Comprobar si se ha podido abrir el archivo
    {
        fprintf(stderr, filePath); // Mensaje de error
        return EXIT_FAILURE;       // Salir con error
    }

    while (fgets(line, MAX_LONGITUD_LINEA, fp) != NULL) // Lee una linea del archivo
    {
        lineNum++; // Incrementa el contador de lineas

        if (strstr(line, patron) != NULL) // Comprobar si el patron se encuentra en la linea
        {
            printf("[PROCESADOR %d] Patron '%s' encontrado en la línea %d\n", getpid(), patron, lineNum); // Imprimir el resultado
        }
    }

    fclose(fp); // Cerrar el archivo

    return EXIT_SUCCESS; // Salir con exito
}