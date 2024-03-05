#include <stdio.h>
#include <stdlib.h>
#include <lista.h>

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
}

// Inserta al final de la lista.
void insertarFinal(TLista *pLista, char *valor)
{
  TNodo *nuevoNodo = malloc(sizeof(TNodo));
  nuevoNodo->valor = valor;
  nuevoNodo->pSiguiente = NULL;

  // Si la lista esta vacia, creamos un nodo con el valor y lo asignamos como primer nodo
  if (pLista->pPrimero == NULL)
  {
    crear(pLista, valor);
  }
  else // Si no, recorremos la lista hasta llegar al ultimo nodo y lo asignamos como siguiente del ultimo nodo
  {
    TNodo *pAux = pLista->pPrimero;
    while (pAux->pSiguiente != NULL)
    {
      pAux = pAux->pSiguiente;
    }
    pAux->pSiguiente = nuevoNodo;
  }
}

// Suponemos n = 1, 2, ...
void insertarN(TLista *pLista, int index, char *valor)
{
  // Crear un nuevo nodo
  TNodo *nuevoNodo = (TNodo *)malloc(sizeof(TNodo));
  nuevoNodo->valor = valor;
  nuevoNodo->pSiguiente = NULL;

  // Si la lista esta vacia
  if (pLista->pPrimero == NULL)
  {
    // Si el indice es 1, creamos un nodo con el valor y lo asignamos como primer nodo
    if (index != 1)
    {
      fprintf(stderr, "ERROR. El indice es diferente a 1. \n");
      return;
    }
    else
    {
      pLista->pPrimero = nuevoNodo;
      return;
    }
  }

  // Si se inserta al principio de la lista
  if (index == 1)
  {
    nuevoNodo->pSiguiente = pLista->pPrimero;
    pLista->pPrimero = nuevoNodo;
    return;
  }

  // Encontrar el nodo anterior a la posicion que queremos insertar
  TNodo *previo = pLista->pPrimero;
  for (int i = 1; i < index - 1; i++)
  {
    if (previo->pSiguiente == NULL)
    {
      fprintf(stderr, "ERROR. El indice es mayor que la longitud de la lista.\n");
      return;
    }
    previo = previo->pSiguiente;
  }

  // Insertar el nuevo nodo despues del nodo anterior
  nuevoNodo->pSiguiente = previo->pSiguiente;
  previo->pSiguiente = nuevoNodo;
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
  if (index < 0 || pLista->pPrimero == NULL)
  {
    fprintf(stderr, "ERROR. El indice es invalido o lista esta vacia.\n");
    return;
  }

  // Si el indice es 0, eliminamos el primer nodo
  if (index == 0)
  {
    TNodo *pAux = pLista->pPrimero;
    pLista->pPrimero = pLista->pPrimero->pSiguiente;
    free(pAux);
    return;
  }

  // Recorremos la lista hasta llegar al nodo anterior al que queremos eliminar
  TNodo *pActual = pLista->pPrimero;
  for (int i = 0; i < index - 1; i++)
  {
    if (pActual->pSiguiente == NULL)
    {
      fprintf(stderr, "ERROR. El indice es mayor que la longitud de la lista.\n");
      return;
    }
    pActual = pActual->pSiguiente;
  }

  // Eliminamos el nodo en la posicion index
  TNodo *pAux = pActual->pSiguiente;
  if (pAux == NULL)
  {
    fprintf(stderr, "ERROR. El indice es mayor que la longitud de la lista.\n");
    return;
  }

  pActual->pSiguiente = pAux->pSiguiente;
  free(pAux);
}

// Obtiene el elemento n(index) de la lista.
int getElementoN(TLista *pLista, int index)
{
  if (index < 0 || pLista->pPrimero == NULL)
  {
    fprintf(stderr, "ERROR. El indice es invalido o lista vacia.\n");
    return -1;
  }

  // Recorremos la lista hasta llegar al nodo en la posicion index
  TNodo *pActual = pLista->pPrimero;
  for (int i = 0; i < index; i++)
  {
    if (pActual->pSiguiente == NULL)
    {
      fprintf(stderr, "ERROR. El indice es mayor que la longitud de la lista.\n");
      return -1;
    }
    pActual = pActual->pSiguiente;
  }

  // Devolvemos el valor del nodo en la posicion index
  return pActual->valor;
}

// Imprime la lista.
void imprimir(TLista *pLista)
{
  TNodo *pAux = pLista->pPrimero; // Creamos un puntero auxiliar

  while (pAux != NULL) // Recorremos la lista e imprimimos los valores de los nodos hasta llegar al final de la lista (NULL)
  {
    printf("%d\n", pAux->valor); // Imprimimos el valor del nodo actual
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