#ifndef _LISTA_
#define _LISTA_

typedef struct Nodo
{
  char *valor;
  struct Nodo *pSiguiente;
} TNodo;

typedef struct
{
  TNodo *pPrimero;
} TLista;

void crear(TLista *pLista, char *valor);
void destruir(TLista *pLista);

// 'insertar' inserta al ppio de la lista.
void insertar(TLista *pLista, char *valor);
void insertarFinal(TLista *pLista, char *valor);
void insertarN(TLista *pLista, int index, char *valor);

// 'elimina' elimina el último de la lista.
void eliminar(TLista *pLista);
void eliminarN(TLista *pLista, int index);

int getElementoN(TLista *pLista, int index);

void imprimir(TLista *pLista);
int longitud(TLista *pLista);

#endif
