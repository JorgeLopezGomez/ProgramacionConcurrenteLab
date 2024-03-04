#ifndef _LISTA_
#define _LISTA_

typedef struct Nodo
{
  int valor;
  struct Nodo *pSiguiente;
} TNodo;

typedef struct
{
  TNodo *pPrimero;
} TLista;

void crear(TLista *pLista, int valor);
void destruir(TLista *pLista);

// 'insertar' inserta al ppio de la lista.
void insertar(TLista *pLista, int valor);
void insertarFinal(TLista *pLista, int valor);
void insertarN(TLista *pLista, int index, int valor);

// 'elimina' elimina el último de la lista.
void eliminar(TLista *pLista);
void eliminarN(TLista *pLista, int index);

int getElementoN(TLista *pLista, int index);

void imprimir(TLista *pLista);
int longitud(TLista *pLista);

#endif
