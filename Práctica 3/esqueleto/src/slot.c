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
    int pid = getpid();               // Obtiene el PID del proceso
    mqd_t qHandlerAterrizajes;        // Abre la cola de mensajes para aterrizajes
    mqd_t qHandlerSlot;               // Abre la cola de mensajes para slot
    char buzonSlot[TAMANO_MENSAJES];  // buzonSLot para el mensaje
    char buffer[TAMANO_MENSAJES + 1]; // buffer para el mensaje

    // Inicia Random
    srand(pid);

    // Verifica los parametros
    if (argc != 2)
    {
        fprintf(stderr, "Error. Usa: ./exec/slot <cola_slot_llamante>.\n"); // Mensaje de error
        exit(EXIT_FAILURE);                                                 // Sale con error
    }
    sprintf(buzonSlot, "%s", argv[1]); // Obtiene el buzonSlot

    // TODO
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

        // Envia
        mq_send(qHandlerAterrizajes, buzonSlot, sizeof(buzonSlot), 0);

        // Recibe
        mq_receive(qHandlerSlot, buffer, sizeof(buffer), NULL);

        // Muestra mensaje de recibe notificación de pista libre
        printf("Slot [%d] recibe notificación de pista libre...\n", pid);
    }

    return EXIT_SUCCESS; // Sale con éxito
}