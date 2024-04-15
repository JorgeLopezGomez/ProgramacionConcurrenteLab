
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <mqueue.h>
#include <definitions.h>

// Modulo principal
int main(int argc,char *argv[]){

    // Define variables locales
    int pid = getpid();
    mqd_t qHandlerAterrizajes;
    mqd_t qHandlerSlot;
    char buzonSlot[TAMANO_MENSAJES];
    char buffer[TAMANO_MENSAJES+1];

    // Inicia Random
    srand(pid);

    // Verifica los parámetros
    if (argc != 2)
    {
        fprintf(stderr, "Error. Usa: ./exec/slot <cola_slot_llamante>.\n");
        exit(EXIT_FAILURE);
    }
    sprintf(buzonSlot,"%s",argv[1]);

    //TODO

    return EXIT_SUCCESS;
}

