#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024
#define MAX_PATRON_LENGTH 100

int main(int argc, char *argv[])
{
    char line[MAX_LINE_LENGTH];
    char patron[MAX_PATRON_LENGTH];
    int lineNum = 0;

    // Verifica si se proporcionaron los argumentos correctos
    if (argc != 3)
    {
        fprintf(stderr, "Uso: %s <ruta_archivo> <patron>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Obtiene la ruta del archivo y el patrón de los argumentos
    char *filePath = argv[1];
    strncpy(patron, argv[2], MAX_PATRON_LENGTH);
    patron[strcspn(patron, "\n")] = 0; // Elimina el salto de línea al final del patrón

    // Abre el archivo
    FILE *fp = fopen(filePath, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "ERROR. No se pudo abrir el archivo %s.\n", filePath);
        return EXIT_FAILURE;
    }

    // Procesa el archivo
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL)
    {
        lineNum++;

        // Busca el patrón en la línea
        if (strstr(line, patron) != NULL)
        {
            printf("[PROCESADOR %d] Patron '%s' encontrado en la línea %d\n", getpid(), patron, lineNum);
        }
    }

    // Cierra el archivo
    fclose(fp);

    return EXIT_SUCCESS;
}