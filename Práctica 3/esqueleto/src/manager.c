
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#include <mqueue.h>
#include <definitions.h>

void crear_buzones();
void instalar_manejador_senhal();
void manejador_senhal(int sign);
void iniciar_tabla_procesos(int n_procesos_telefono, int n_procesos_linea);
void crear_procesos(int numPistas, int numSlots);
void lanzar_proceso_pista(const int indice_tabla);
void lanzar_proceso_slot(const int indice_tabla);
void esperar_procesos();
void terminar_procesos(void);
void terminar_procesos_especificos(struct TProcess_t *process_table, int process_num);
void liberar_recursos();

int g_pistasProcesses = 0;
int g_slotsProcesses = 0;
struct TProcess_t *g_process_pistas_table;
struct TProcess_t *g_process_slots_table;
mqd_t qHandlerAterrizajes;
mqd_t qHandlerSlots[NUMSLOTS];

int main(int argc, char *argv[])
{
    // Define variables locales

    // Creamos los buzones
    crear_buzones();

    // Manejador de Ctrl-C
    instalar_manejador_senhal();

    // Crea Tabla para almacenar los pids de los procesos
    iniciar_tabla_procesos(NUMPISTAS, NUMSLOTS);

    // Tenemos todo
    // Lanzamos los procesos
    crear_procesos(NUMPISTAS, NUMSLOTS);

    // Esperamos a que finalicen los slots (No acabará nunca)
    esperar_procesos();

    // Matamos las pistas y cualquier otro proceso restante
    terminar_procesos();

    // Finalizamos Manager
    printf("\n[MANAGER] Terminacion del programa (todos los procesos terminados).\n");
    liberar_recursos();

    return EXIT_SUCCESS;
}

// TODO
void crear_buzones()
{
    // TODO
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct TMessage_t);
    attr.mq_curmsgs = 0;

    qHandlerAterrizajes = mq_open(ATERRIZAJES, O_CREAT | O_RDWR, 0666, &attr);
    if (qHandlerAterrizajes == -1)
    {
        fprintf(stderr, "Error al crear el buzon de aterrizajes: %s\n", strerror(errno));
        exit(1);
    }

    for (int i = 0; i < NUMSLOTS; i++)
    {
        qHandlerSlots[i] = mq_open(SLOTS[i], O_CREAT | O_RDWR, 0666, &attr);
        if (qHandlerSlots[i] == -1)
        {
            fprintf(stderr, "Error al crear el buzon de slots: %s\n", strerror(errno));
            exit(1);
        }
    }
}

// Instalar el manejador de senhal
void instalar_manejador_senhal()
{
    if (signal(SIGINT, manejador_senhal) == SIG_ERR) // Instalar el manejador de senhal para Ctrl + C
    {
        fprintf(stderr, "[MANAGER] Error al instalar el manejador se senhal: %s.\n", strerror(errno)); // Mensaje de error
        exit(EXIT_FAILURE);                                                                            // Salir con error
    }
}

// Manejador de senhal
void manejador_senhal(int sign)
{
    printf("\n[MANAGER] Terminacion del programa (Ctrl + C).\n");                      // Mensaje de terminacion
    terminar_procesos();                                                               // Terminar los procesos
    liberar_recursos();                                                                // Liberar los recursos
    printf("\n[MANAGER] Terminacion del programa (todos los procesos terminados).\n"); // Mensaje de terminacion
    exit(EXIT_SUCCESS);                                                                // Salir con exito
}

// Iniciar tabla de procesos
void iniciar_tabla_procesos(int n_procesos_telefono, int n_procesos_linea)
{
    // TODO
    g_process_pistas_table = (struct TProcess_t *)malloc(n_procesos_telefono * sizeof(struct TProcess_t));
    if (g_process_pistas_table == NULL)
    {
        fprintf(stderr, "Error al reservar memoria para la tabla de procesos de pistas: %s\n", strerror(errno));
        exit(1);
    }

    g_process_slots_table = (struct TProcess_t *)malloc(n_procesos_linea * sizeof(struct TProcess_t));
    if (g_process_slots_table == NULL)
    {
        fprintf(stderr, "Error al reservar memoria para la tabla de procesos de slots: %s\n", strerror(errno));
        exit(1);
    }
}

// Crear procesos
void crear_procesos(int numPistas, int numSlots)
{
    for (int i = 0; i < numSlots; i++)
    {
        if (fork() == 0)
        {
            lanzar_proceso_slot(i);
            return;
        }
    }
    printf("[MANAGER] %d slots creados.\n", numSlots);

    for (int i = 0; i < numPistas; i++)
    {
        if (fork() == 0)
        {
            lanzar_proceso_pista(i);
            return;
        }
    }
    printf("[MANAGER] %d pistas creadas.\n", numPistas);
}
