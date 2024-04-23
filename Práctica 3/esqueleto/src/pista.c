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

    // TODO
    mqd_t qHandlerAterrizajes;
    mqd_t qHandlerSlot;
    char buzonPista[TAMANO_MENSAJES];
    char buffer[TAMANO_MENSAJES + 1];

    srand(pid);

    // TODO
    if ((qHandlerAterrizajes = mq_open(BUZON_ATERRIZAJES, O_RDWR)) == -1)
    {
        fprintf(stderr, "Pista [%d] Error al abrir la cola de mensajes: %s\n", pid, strerror(errno));
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        printf("Pista [%d] en espera...\n", pid);
        sleep(rand() % 11 + 10);

        printf("Pista [%d] con avión en aproximación desde el slot: [%s]…\n", pid, buffer);

        if ((mq_receive(qHandlerAterrizajes, buffer, TAMANO_MENSAJES, NULL)) == -1)
        {
            fprintf(stderr, "Pista [%d] Error al recibir el mensaje: %s\n", pid, strerror(errno));
            exit(EXIT_FAILURE);
        }

        printf("Pista [%d] con avión en aproximación desde Slot: %s\n", pid, buffer);
        printf("Pista [%d] ha liberado el Slot. %s\n", pid, buffer);
        printf("Pista [%d] en espera...\n", pid);

        qHandlerSlot = mq_open(buffer, O_RDWR);

        mq_send(qHandlerSlot, buffer, sizeof(buffer), 0);

        // cierra las colas
        mq_close(qHandlerAterrizajes);
    }

    return EXIT_SUCCESS;
}