
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <mqueue.h>
#include <definitions.h>

// Modulo principal
int main(int argc, char *argv[])
{

    // Define variables locales
    int pid = getpid();
    mqd_t qHandlerAterrizajes;
    mqd_t qHandlerSlot;
    char buzonSlot[TAMANO_MENSAJES];
    char buffer[TAMANO_MENSAJES + 1];

    // Inicia Random
    srand(pid);

    // Verifica los parámetros
    if (argc != 2)
    {
        fprintf(stderr, "Error. Usa: ./exec/slot <cola_slot_llamante>.\n");
        exit(EXIT_FAILURE);
    }
    sprintf(buzonSlot, "%s", argv[1]);

    // TODO

    // Abre la cola de mensajes de aterrizajes
    qHandlerAterrizajes = mq_open(BUZON_ATERRIZAJES, O_RDONLY);
    if (qHandlerAterrizajes == (mqd_t)-1)
    {
        fprintf(stderr, "Error en mq_open de la cola de aterrizajes: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // Abre la cola de mensajes de slot
    qHandlerSlot = mq_open(buzonSlot, O_WRONLY);
    if (qHandlerSlot == (mqd_t)-1)
    {
        fprintf(stderr, "Error en mq_open de la cola de slot: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    // Mientras haya mensajes en la cola de aterrizajes
    while (mq_receive(qHandlerAterrizajes, buffer, TAMANO_MENSAJES, NULL) != -1)
    {
        // Enviar mensajes a la cola de slot
        mq_send(qHandlerSlot, buffer, TAMANO_MENSAJES, 0);
        // Mientras haya mensajes en la cola de slot
        while (mq_receive(qHandlerSlot, buffer, TAMANO_MENSAJES, NULL) != -1)
        {
            // Imprimir mensaje
            printf("Slot %d: %s\n", pid, buffer);
        }
    }
    return EXIT_SUCCESS;
}