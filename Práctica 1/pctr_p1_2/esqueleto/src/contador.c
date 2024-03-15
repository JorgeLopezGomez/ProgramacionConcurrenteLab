#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void contar(char *line, int line_number); // line_number es el numero de linea que se esta procesando

// Funcion principal
int main(int argc, char *argv[])
{
  if (argc < 3)                                                          // Comprobar si se pasan los argumentos necesarios para el programa
  {                                                                      // Si no se pasan los argumentos necesarios
    fprintf(stderr, "ERROR. Uso: %s <linea> <numero_linea>\n", argv[0]); // Mensaje de error
    return EXIT_FAILURE;                                                 // Salir con error
  }

  char *linea = argv[1];            // Linea a procesar
  int numero_linea = atoi(argv[2]); // Numero de linea a procesar y con la funcion atoi lo convertimos la cadena de caracteres a entero

  contar(linea, numero_linea); // Llamar a la funcion contar

  return EXIT_SUCCESS; // Salir con exito
}

// Funcion para contar las palabras de una linea
void contar(char *linea, int numero_linea)
{
  int n_palabras = 0, dentro_palabra = 0; // Inicializar variables
  const char *it = linea;                 // Inicializar iterador

  do // Bucle para recorrer la linea
  {
    switch (*it) // Comprobar el caracter actual de la linea
    {
    case '\0':            // Si es el final de la linea
    case ' ':             // Si es un espacio
    case '\t':            // Si es un tabulador
    case '\n':            // Si es un salto de linea
    case '\r':            // Si es un retorno de carro
      if (dentro_palabra) // Si se estaba dentro de una palabra
      {
        dentro_palabra = 0; // Ya no se esta dentro de una palabra
        n_palabras++;       // Incrementar el contador de palabras
      }
      break; // Salir del switch y continuar con el bucle
    default:
      dentro_palabra = 1; // Si no es un espacio, un tabulador, un salto de linea o un retorno de carro, entonces se esta dentro de una palabra
    }
  } while (*it++); // Incrementar el iterador y comprobar si se ha llegado al final de la linea

  printf("[CONTADOR %d] La linea '%d' tiene %d palabras\n", getpid(), numero_linea, n_palabras); // Imprimir el resultado
}