#include <stdio.h>
#include <stdlib.h>
#include <lista.h>
#include <string.h>

// Crea una lista con un nodo.
void crear(TLista *pLista, char *valor)
{
  pLista->pPrimero = malloc(sizeof(TNodo)); // Reservamos memoria para el primer nodo

  char *pValor = malloc(strlen(valor) * sizeof(char)); // Reservamos memoria para el valor del primer nodo
  strcpy(pValor, valor);                               // Copiamos el valor pasado por parametro al primer nodo
  pLista->pPrimero->valor = pValor;                    // Asignamos el valor al primer nodo

  pLista->pPrimero->pSiguiente = NULL; // Asignamos NULL al siguiente del primer nodo
}

// Elimina todos los nodos de la lista, liberando la memoria que ocupan los nodos y la lista en si misma (pLista).
void destruir(TLista *pLista)
{
  TNodo *pAux1;       // Creamos dos punteros auxiliares
  struct Nodo *pAux2; // Creamos dos punteros auxiliares

  for (pAux1 = pLista->pPrimero; pAux1 != NULL;) // Recorremos la lista
  {
    pAux2 = pAux1->pSiguiente; // Guardamos el siguiente nodo
    free(pAux1);               // Liberamos el nodo actual
    pAux1 = pAux2;             // Asignamos el siguiente nodo al nodo actual
  }

  free(pLista); // Liberamos la lista
}

// Inserta al principio de la lista.
void insertar(TLista *pLista, char *valor)
{
  struct Nodo *pNodo = malloc(sizeof(TNodo)); // Reservamos memoria para el nuevo nodo

  char *pNodoValor = malloc(strlen(valor) * sizeof(char)); // Reservamos memoria para el valor del nuevo nodo
  strcpy(pNodoValor, valor);                               // Copiamos el valor pasado por parametro al nuevo nodo
  pNodo->valor = pNodoValor;                               // Asignamos el valor al nuevo nodo
  pNodo->pSiguiente = pLista->pPrimero;                    // Asignamos el primer nodo de la lista como siguiente del nuevo nodo
  pLista->pPrimero = pNodo;                                // Asignamos el nuevo nodo como primer nodo de la lista
}

// Inserta al final de la lista.
void insertarFinal(TLista *pLista, char *valor)
{
  if (pLista->pPrimero == NULL) // Si la lista esta vacia
  {
    crear(pLista, valor); // Creamos un nodo con el valor y lo asignamos como primer nodo
  }
  else // Si la lista no esta vacía
  {
    struct Nodo *pNodo = malloc(sizeof(TNodo)); // Reservamos memoria para el nuevo nodo

    char *pNodoValor = malloc(strlen(valor) * sizeof(char)); // Reservamos memoria para el valor del nuevo nodo
    strcpy(pNodoValor, valor);                               // Copiamos el valor pasado por parametro al nuevo nodo
    pNodo->valor = pNodoValor;                               // Asignamos el valor al nuevo nodo
    pNodo->pSiguiente = NULL;                                // El siguiente nodo del nuevo nodo es NULL porque es el ultimo nodo

    struct Nodo *pActual = pLista->pPrimero; // Empezamos desde el primer nodo
    while (pActual->pSiguiente != NULL)      // Recorremos la lista hasta llegar al ultimo nodo
    {
      pActual = pActual->pSiguiente; // Asignamos el siguiente nodo al nodo actual
    }
    pActual->pSiguiente = pNodo; // Asignamos el nuevo nodo como el siguiente nodo del ultimo nodo
  }
}

// Inserta en la posición n(index) de la lista. Suponemos n = 1, 2, ...
void insertarN(TLista *pLista, int index, char *valor)
{
  if (index < 0) // Si el indice es negativo
  {
    fprintf(stderr, "ERROR. El indice es invalido.\n"); // Imprimimos un mensaje de error
    return;                                             // Salimos de la funcion
  }

  if (index == 0) // Si el indice es 0, insertamos el nuevo nodo al principio de la lista
  {
    insertar(pLista, valor); // Insertamos el nuevo nodo al principio de la lista
    return;                  // Salimos de la funcion
  }

  TNodo *pActual = pLista->pPrimero;  // Creamos un puntero auxiliar y lo asignamos al primer nodo de la lista
  for (int i = 0; i < index - 1; i++) // Recorremos la lista hasta llegar al nodo en la posicion index - 1
  {
    if (pActual->pSiguiente == NULL) // Si el siguiente nodo es NULL, es porque hemos llegado al final de la lista
    {
      fprintf(stderr, "ERROR. El indice es mayor que la longitud de la lista.\n"); // Imprimimos un mensaje de error
      return;                                                                      // Salimos de la funcion
    }
    pActual = pActual->pSiguiente; // Asignamos el siguiente nodo al nodo actual
  }

  // Insertamos el nuevo nodo en la posicion index
  TNodo *pNodo = malloc(sizeof(TNodo)); // Reservamos memoria para el nuevo

  char *pNodoValor = malloc((strlen(valor) + 1) * sizeof(char)); // Reservamos memoria para el valor del nuevo nodo
  strcpy(pNodoValor, valor);                                     // Copiamos el valor pasado por parametro al nuevo nodo
  pNodo->valor = pNodoValor;                                     // Asignamos el valor al nuevo nodo
  pNodo->pSiguiente = pActual->pSiguiente;                       // Asignamos el siguiente nodo del nodo actual como siguiente del nuevo nodo
  pActual->pSiguiente = pNodo;                                   // Asignamos el nuevo nodo como el siguiente nodo del nodo actual
}

// Elimina el primer elemento de la lista.
void eliminar(TLista *pLista)
{
  TNodo *pAux = pLista->pPrimero;                  // Creamos un puntero auxiliar
  pLista->pPrimero = pLista->pPrimero->pSiguiente; // Asignamos el siguiente nodo al primer nodo
  free(pAux);                                      // Liberamos el primer nodo

  if (pLista->pPrimero == NULL) // Si la lista esta vacia, creamos un nodo con el valor NULL y lo asignamos como primer nodo
  {
    crear(pLista, 0); // Creamos un nodo con el valor NULL y lo asignamos como primer nodo
  }
}

// Elimina el elemento n(index) de la lista.
void eliminarN(TLista *pLista, int index)
{
  if (index < 0 || pLista->pPrimero == NULL) // Si el indice es negativo o la lista esta vacia
  {
    fprintf(stderr, "ERROR. El indice es invalido o lista esta vacia.\n"); // Imprimimos un mensaje de error
    return;                                                                // Salimos de la funcion
  }

  if (index == 0) // Si el indice es 0, eliminamos el primer nodo
  {
    TNodo *pAux = pLista->pPrimero;                  // Creamos un puntero auxiliar
    pLista->pPrimero = pLista->pPrimero->pSiguiente; // Asignamos el siguiente nodo al primer nodo
    free(pAux->valor);                               // Liberamos la memoria del valor del nodo
    free(pAux);                                      // Liberamos el primer nodo
    return;                                          // Salimos de la funcion
  }

  TNodo *pActual = pLista->pPrimero;  // Creamos un puntero auxiliar y lo asignamos al primer nodo de la lista
  for (int i = 0; i < index - 1; i++) // Recorremos la lista hasta llegar al nodo en la posicion index - 1
  {
    if (pActual->pSiguiente == NULL) // Si el siguiente nodo es NULL, es porque hemos llegado al final de la lista
    {
      fprintf(stderr, "ERROR. El indice es mayor que la longitud de la lista.\n"); // Imprimimos un mensaje de error
      return;                                                                      // Salimos de la funcion
    }
    pActual = pActual->pSiguiente; // Asignamos el siguiente nodo al nodo actual
  }

  TNodo *pAux = pActual->pSiguiente; // Creamos un puntero auxiliar y lo asignamos al siguiente nodo del nodo actual
  if (pAux == NULL)                  // Si el siguiente nodo es NULL, es porque hemos llegado al final de la lista
  {
    fprintf(stderr, "ERROR. El indice es mayor que la longitud de la lista.\n"); // Imprimimos un mensaje de error
    return;                                                                      // Salimos de la funcion
  }

  pActual->pSiguiente = pAux->pSiguiente; // Asignamos el siguiente nodo del siguiente nodo al nodo actual
  free(pAux->valor);                      // Liberamos la memoria del valor del nodo
  free(pAux);                             // Liberamos el siguiente nodo
}

// Obtiene el elemento n(index) de la lista.
char *getElementoN(TLista *pLista, int index)
{
  if (index < 0 || pLista->pPrimero == NULL) // Si el indice es negativo o la lista esta vacia
  {
    fprintf(stderr, "ERROR. El indice es invalido o lista vacia.\n"); // Imprimimos un mensaje de error
    return NULL;                                                      // Devolvemos NULL
  }

  TNodo *pActual = pLista->pPrimero;  // Creamos un puntero auxiliar y lo asignamos al primer nodo de la lista
  for (int i = 0; i < index - 1; i++) // Recorremos la lista hasta llegar al nodo en la posicion index - 1
  {
    if (pActual->pSiguiente == NULL) // Si el siguiente nodo es NULL, es porque hemos llegado al final de la lista
    {
      fprintf(stderr, "ERROR. El indice es mayor que la longitud de la lista.\n"); // Imprimimos un mensaje de error
      return NULL;                                                                 // Devolvemos NULL
    }
    pActual = pActual->pSiguiente; // Asignamos el siguiente nodo al nodo actual
  }

  return pActual->valor; // Devolvemos el valor del nodo en la posicion index
}

// Imprime la lista.
void imprimir(TLista *pLista)
{
  TNodo *pAux = pLista->pPrimero; // Creamos un puntero auxiliar

  while (pAux != NULL) // Recorremos la lista e imprimimos los valores de los nodos hasta llegar al final de la lista (NULL)
  {
    printf("%s\n", pAux->valor); // Imprimimos el valor del nodo actual
    pAux = pAux->pSiguiente;     // Asignamos el siguiente nodo al nodo actual
  }
  printf("\n"); // Imprimimos un salto de linea
}

// Devuelve la longitud de la lista.
int longitud(TLista *pLista)
{
  TNodo *pAux = pLista->pPrimero; // Creamos un puntero auxiliar
  int longitud = 0;               // Creamos una variable para almacenar la longitud

  while (pAux != NULL) // Recorremos la lista hasta llegar al final de la lista (NULL) y contamos los nodos que hay en la lista (longitud)
  {
    longitud++;              // Aumentamos la longitud
    pAux = pAux->pSiguiente; // Asignamos el siguiente nodo al nodo actual
  }

  return longitud; // Devolvemos la longitud
}