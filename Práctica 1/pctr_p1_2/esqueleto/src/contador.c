#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void contar(char *line, int line_number); // line_number es el número de línea que se está procesando

// Función principal
int main(int argc, char *argv[])
{
  if (argc < 3)                                                   // Comprobar si se pasan los argumentos necesarios para el programa
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
    switch (*it) // Comprobar el carácter actual de la línea
    {
    case '\0':            // Si es el final de la línea
    case ' ':             // Si es un espacio
    case '\t':            // Si es un tabulador
    case '\n':            // Si es un salto de línea
    case '\r':            // Si es un retorno de carro
      if (dentro_palabra) // Si se estaba dentro de una palabra
      {
        dentro_palabra = 0; // Ya no se está dentro de una palabra
        n_palabras++;       // Incrementar el contador de palabras
      }
      break; // Salir del switch y continuar con el bucle
    default:
      dentro_palabra = 1; // Si no es un espacio, tabulador, salto de línea o retorno de carro, se está dentro de una palabra
    }
  } while (*it++); // Incrementar el iterador y comprobar si se ha llegado al final de la línea

  printf("[CONTADOR %d] La linea '%d' tiene %d palabras\n", getpid(), numero_linea, n_palabras); // Imprimir el resultado
}