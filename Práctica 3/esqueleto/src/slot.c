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
    if (argc != 2)
    {
        fprintf(stderr, "Error. Usa: ./exec/slot <cola_slot_llamante>.\n");
        exit(EXIT_FAILURE);
    }
    sprintf(buzonSlot, "%s", argv[1]);

    // TODO
    if (sscanf(argv[1], "%s", buzonSlot) != 1)
    {
        fprintf(stderr, "Error. Usa: ./exec/slot <cola_slot_llamante>.\n");
        exit(EXIT_FAILURE);
    }

    // Abre la cola de mensajes para slot
    qHandlerAterrizajes = mq_open(BUZON_ATERRIZAJES, O_RDWR);
    qHandlerSlot = mq_open(buzonSlot, O_RDWR);

    // Bucle principal
    while (1)
    {
        // Recibe notificacion de esperando pista libre y duerme un tiempo aleatorio entre 1 y 30 segundos
        printf("Slot [%d] esperando avión...\n", pid);
        sleep(rand() % 30 + 1);

        // Recibe el avión y envía notificación de pista libre
        printf("Slot [%d] recibido avión (%s)...\n", pid, buzonSlot);

        mq_send(qHandlerAterrizajes, buzonSlot, sizeof(buzonSlot), 0);

        mq_receive(qHandlerSlot, buffer, sizeof(buffer), NULL);

        printf("Slot [%d] recibe notificación de pista libre...\n", pid);
    }

    return EXIT_SUCCESS;
}