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
        fprintf(stderr, "[SLOT] Error. Usa: ./exec/manager.\n");
        exit(EXIT_FAILURE);
    }
    sprintf(buzonSlot, "%s", argv[1]);

    // TODO
    // Abre la cola de mensajes para slot
    qHandlerSlot = mq_open(buzonSlot, O_RDONLY);
    if (qHandlerSlot == (mqd_t)-1)
    {
        fprintf(stderr, "Slot [%d] Error al abrir la cola de mensajes: %s\n", pid, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Abre la cola de mensajes para aterrizajes
    qHandlerAterrizajes = mq_open(argv[2], O_RDONLY);
    if (qHandlerAterrizajes == (mqd_t)-1)
    {
        fprintf(stderr, "Slot [%d] Error al abrir la cola de mensajes: %s\n", pid, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Bucle principal
    while (1)
    {
        // Recibe notificación de pista libre
        printf("Slot [%d] recibe notificación de pista libre...\n", pid);
        printf("Slot [%d] esperando avión...\n", pid);

        ssize_t bytesRead = mq_receive(qHandlerSlot, buffer, TAMANO_MENSAJES, NULL);
        if (bytesRead == -1)
        {
            fprintf(stderr, "Slot [%d] Error al recibir el mensaje: %s\n", pid, strerror(errno));
            exit(EXIT_FAILURE);
        }

        // Asegura que el buffer es una cadena de caracteres terminada en NULL
        buffer[bytesRead] = '\0';

        printf("Slot [%d] recibido avión (%s)...\n", pid, buffer);
    }

    // Cierra las colas de mensajes
    mq_close(qHandlerSlot);
    mq_close(qHandlerAterrizajes);

    return 0;
}