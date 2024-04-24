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
    char buzonSlot[TAMANO_MENSAJES];
    char buffer[TAMANO_MENSAJES + 1];

    srand(pid);

    // TODO
    qHandlerAterrizajes = mq_open(BUZON_ATERRIZAJES, O_RDWR); 

    while (1)
    {
        printf("Pista [%d] en espera...\n", pid);
        // sleep(rand() % 11 + 10);

        // printf("Pista [%d] con avión en aproximación desde el slot: [%s]…\n", pid, buzonSlot);

        mq_receive(qHandlerAterrizajes, buzonSlot, sizeof(buzonSlot), NULL);
        printf("Pista [%d] con avión en aproximación desde el Slot: %s\n", pid, buzonSlot);
        sleep(rand() % 10 + 10);

        // printf("Pista [%d] con avión en aproximación desde Slot: %s\n", pid, buffer);
        printf("Pista [%d] ha liberado el Slot. %s\n", pid, buzonSlot);
        // printf("Pista [%d] en espera...\n", pid);

        qHandlerSlot = mq_open(buzonSlot, O_RDWR);
        sprintf(buffer, "%d", pid);
        mq_send(qHandlerSlot, buffer, sizeof(buffer), 0);

        // cierra las colas
        mq_close(qHandlerAterrizajes);
    }

    return EXIT_SUCCESS;
}