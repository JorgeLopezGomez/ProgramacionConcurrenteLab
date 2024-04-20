
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
    char buffer[TAMANO_MENSAJES + 1];

    srand(pid);

    // TODO
    // if (argc != 2)
    // {
    //     fprintf(stderr, "Error. Usa: ./exec/slot <cola_pista_llamante>.\n");
    //     exit(EXIT_FAILURE);
    // }
    sprintf(buffer, "%s", argv[1]);

    qHandlerAterrizajes = mq_open(BUZON_ATERRIZAJES, O_RDONLY);
    if (qHandlerAterrizajes == (mqd_t)-1)
    {
        fprintf(stderr, "Pista [%d] Error al abrir la cola de mensajes: %s\n", pid, strerror(errno));
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        printf("Pista [%d] en espera...\n", pid);
        sleep(rand() % 11 + 10);

        ssize_t bytesRead = mq_receive(qHandlerAterrizajes, buffer, TAMANO_MENSAJES, NULL);
        if (bytesRead == -1)
        {
            fprintf(stderr, "Pista [%d] Error al recibir el mensaje: %s\n", pid, strerror(errno));
            exit(EXIT_FAILURE);
        }

        printf("Pista [%d] con avión en aproximación desde Slot: %s\n", pid, buffer);
        printf("Pista [%d] ha liberado el Slot. %s\n", pid, buffer);
        printf("Pista [%d] en espera...\n", pid);
    }

    // cierra las colas
    mq_close(qHandlerAterrizajes);

    return EXIT_SUCCESS;
}