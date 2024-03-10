#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void contar(char *line, int line_number); // line_number es el número de línea que se está procesando

// Función principal
int main(int argc, char *argv[])
{
  if (argc < 3)
  {                                                               // Si no se pasan los argumentos necesarios
    fprintf(stderr, "Uso: %s <linea> <numero_linea>\n", argv[0]); // Mensaje de error
    return EXIT_FAILURE;                                          // Salir con error
  }

  char *linea = argv[1];            // Línea a procesar
  int numero_linea = atoi(argv[2]); // Número de línea a procesar (convertido a entero)

  contar(linea, numero_linea); // Llamar a la función contar

  return EXIT_SUCCESS; // Salir con éxito
}

void contar(char *linea, int numero_linea) // Función para contar las palabras de una línea
{
  int n_palabras = 0, dentro_palabra = 0; // Inicializar variables
  const char *it = linea;                 // Inicializar iterador

  do // Bucle para recorrer la línea
  {
    switch (*it)
    {
    case '\0':
    case ' ':
    case '\t':
    case '\n':
    case '\r':
      if (dentro_palabra)
      {
        dentro_palabra = 0;
        n_palabras++;
      }
      break;
    default:
      dentro_palabra = 1;
    }
  } while (*it++);

  printf("[CONTADOR %d] La linea '%d' tiene %d palabras\n", getpid(), numero_linea, n_palabras); // Imprimir el resultado
}