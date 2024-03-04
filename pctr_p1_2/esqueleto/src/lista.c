#include <stdio.h>
#include <stdlib.h>
#include <lista.h>

// Crea una lista con un nodo.
void crear(TLista *pLista, char valor[])
{
  pLista->pPrimero = malloc(sizeof(TNodo));
  pLista->pPrimero->valor = valor;
  pLista->pPrimero->pSiguiente = NULL;
}

// Elimina todos los nodos de la lista, liberando la memoria que ocupan los nodos y la lista en si misma (pLista).
void destruir(TLista *pLista)
{
  TNodo *pAux1;
  struct Nodo *pAux2;

  for (pAux1 = pLista->pPrimero; pAux1 != NULL;)
  {
    pAux2 = pAux1->pSiguiente;
    free(pAux1);
    pAux1 = pAux2;
  }

  free(pLista);
}

// Inserta al principio de la lista.
void insertar(TLista *pLista, char valor[])
{
  TNodo *nuevoNodo = malloc(sizeof(TNodo));
  nuevoNodo->valor = valor;
  nuevoNodo->pSiguiente = pLista->pPrimero;
  pLista->pPrimero = nuevoNodo;
}

// Inserta al final de la lista.
void insertarFinal(TLista *pLista, char valor[])
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
void insertarN(TLista *pLista, int index, char valor[])
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
  TNodo *pAux = pLista->pPrimero;
  pLista->pPrimero = pLista->pPrimero->pSiguiente;
  free(pAux);

  // Si la lista esta vacia, la creamos con un nodo vacio
  if (pLista->pPrimero == NULL)
  {
    crear(pLista, 0);
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
  TNodo *pAux = pLista->pPrimero;

  // Recorremos la lista e imprimimos los valores
  while (pAux != NULL)
  {
    printf("%d\n", pAux->valor);
    pAux = pAux->pSiguiente;
  }
  printf("\n");
}

// Devuelve la longitud de la lista.
int longitud(TLista *pLista)
{
  TNodo *pAux = pLista->pPrimero;
  int longitud = 0;

  // Recorremos la lista y contamos los nodos
  while (pAux != NULL)
  {
    longitud++;
    pAux = pAux->pSiguiente;
  }

  // Devolvemos la longitud
  return longitud;
}