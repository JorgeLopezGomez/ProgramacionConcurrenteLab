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

    // Verifica los parametros
    // if (argc != 2)
    // {
    //     fprintf(stderr, "Error. Usa: ./exec/slot <cola_slot_llamante>.\n");
    //     exit(EXIT_FAILURE);
    // }
    sprintf(buzonSlot, "%s", argv[1]);

    // TODO
    // Abre la cola de mensajes para slot
    qHandlerSlot = mq_open(buzonSlot, O_RDWR);
    qHandlerAterrizajes = mq_open(BUZON_ATERRIZAJES, O_RDWR);

    // Bucle principal
    while (1)
    {
        // Recibe notificacion de pista libre
        printf("Slot [%d] esperando avión...\n", pid);
        sleep(rand() % 30 + 1);
        printf("Slot [%d] recibe notificación de pista libre...\n", pid);
        mq_send(qHandlerAterrizajes, buzonSlot, sizeof(buzonSlot), 0);

        printf("Slot [%d] esperando finalización de avión...\n", pid);

        mq_receive(qHandlerSlot, buffer, TAMANO_MENSAJES, NULL);

        printf("Slot [%d] recibido avión (%s)...\n", pid, buffer);
    }

    // Cierra las colas de mensajes
    mq_close(qHandlerSlot);
    mq_close(qHandlerAterrizajes);

    return EXIT_SUCCESS;
}