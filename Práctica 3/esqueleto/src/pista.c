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
    mqd_t qHandlerAterrizajes; // Abre la cola de mensajes para aterrizajes
    mqd_t qHandlerSlot; // Abre la cola de mensajes para slot
    char buffer[TAMANO_MENSAJES]; // buffer para el mensaje

    srand(pid);

    // TODO
    qHandlerAterrizajes = mq_open(BUZON_ATERRIZAJES, O_RDWR); // Abre la cola de mensajes para aterrizajes

    // Bucle principal
    while (1)
    {
        // Recibe notificacion de pista libre
        printf("Pista [%d] en espera...\n", pid);

        // Recibe notificacion de pista libre
        mq_receive(qHandlerAterrizajes, buffer, sizeof(buffer), NULL);

        // Muestra mensaje de avión en aproximación desde el Slot y duerme un tiempo aleatorio entre 10 y 20 segundos
        printf("Pista [%d] con avión en aproximación desde el Slot: %s\n", pid, buffer);
        sleep(rand() % 10 + 10);
        
        // Muestra mensaje de pista ha liberado el Slot
        printf("Pista [%d] ha liberado el Slot. %s\n", pid, buffer);

        // Envia notificacion de pista libre al Slot
        qHandlerSlot = mq_open(buffer, O_RDWR);
        mq_send(qHandlerSlot, buffer, sizeof(buffer), 0);

        // Cierra la cola de mensajes para aterrizajes
        mq_close(qHandlerSlot);
    }

    return EXIT_SUCCESS; // Sale con éxito
}