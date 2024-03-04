#include <stdio.h>
#include <stdlib.h>
#include <lista.h>

int main(int argc, char *argv[])
{
  TLista *pLista;

  pLista = malloc(sizeof(TLista));

  // Crear la lista con un nodo inicial con valor 7
  crear(pLista, 7);

  // Insertar un nodo con valor 5 al principio
  insertar(pLista, 5);

  // Insertar un nodo con valor 10 al final
  insertarFinal(pLista, 10);

  // Insertar un nodo con valor 15 en la posición 2
  insertarN(pLista, 2, 15);

  // Imprimir la lista
  imprimir(pLista);

  // Eliminar el primer nodo
  eliminar(pLista);

  // Eliminar el nodo en la posición 2
  eliminarN(pLista, 2);

  // Imprimir la lista después de las eliminaciones
  imprimir(pLista);

  // Imprimir la longitud de la lista
  printf("Longitud de la lista: %d\n", longitud(pLista));

  // Obtener y imprimir el primer elemento de la lista
  printf("Primer elemento de la lista: %d\n", getElementoN(pLista, 0));

  // Destruir la lista
  destruir(pLista);

  return 0;
}