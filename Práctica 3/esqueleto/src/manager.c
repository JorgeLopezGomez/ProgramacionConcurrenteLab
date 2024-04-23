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

    // Esperamos a que finalicen los slots (No acabara nunca)
    esperar_procesos();

    // Matamos las pistas y cualquier otro proceso restante
    // terminar_procesos();

    // Finalizamos Manager
    printf("\n[MANAGER] Terminacion del programa (todos los procesos terminados).\n");
    liberar_recursos();

    return EXIT_SUCCESS;
}

// Crear buzones
void crear_buzones()
{
    struct mq_attr attr; // Atributos de la cola de mensajes

    attr.mq_maxmsg = NUMSLOTS;
    attr.mq_msgsize = TAMANO_MENSAJES;
    mqd_t mq = mq_open(BUZON_ATERRIZAJES, O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR, &attr);

    char caux[TAMANO_MENSAJES];

    attr.mq_maxmsg = 1;
    int i = 0;
    for (i = 0; i < NUMSLOTS; i++)
    {
        sprintf(caux, "%s%d", BUZON_SLOTS, i);
        mq = mq_open(caux, O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR, &attr);
        if (mq == -1)
        {
            perror("mq_open");
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
    g_pistasProcesses = n_procesos_telefono; // Numero de procesos de pista
    g_slotsProcesses = n_procesos_linea;     // Numero de procesos de slot

    // Inicializar la tabla de procesos de slot
    g_process_slots_table = malloc(g_slotsProcesses * sizeof(struct TProcess_t));
    for (int i = 0; i < g_slotsProcesses; i++) // Inicializar la tabla de procesos de slot
    {
        g_process_slots_table[i].pid = 0; // Inicializar el PID del proceso
    }

    // Inicializar la tabla de procesos de pista
    g_process_pistas_table = malloc(g_pistasProcesses * sizeof(struct TProcess_t));
    for (int i = 0; i < g_pistasProcesses; i++) // Inicializar la tabla de procesos de pista
    {
        g_process_pistas_table[i].pid = 0; // Inicializar el PID del proceso
    }
}

// Crear procesos
void crear_procesos(int numPistas, int numSlots)
{

    // Crear procesos de slots
    printf("[MANAGER] %d slots creados.\n", numSlots); // Mensaje de creacion de procesos de slots
    for (int i = 0; i < numSlots; i++)
    {
        lanzar_proceso_slot(i); // Lanzar un proceso de slot
    }

    // Crear procesos de pistas
    printf("[MANAGER] %d pistas creadas.\n", numPistas); // Mensaje de creacion de procesos de pistas
    for (int i = 0; i < numPistas; i++)
    {
        lanzar_proceso_pista(i); // Lanzar un proceso de pista
    }

    sleep(1); // Esperar un segundo
}

// Lanzar un proceso de pista
void lanzar_proceso_pista(const int indice_tabla)
{
    pid_t pid; // PID del proceso

    switch (pid = fork()) // Crear un proceso hijo
    {
    case -1:                                                                                // Error
        fprintf(stderr, "[MANAGER] Error al lanzar proceso pista: %s.\n", strerror(errno)); // Mensaje de error
        // terminar_procesos();                                                                // Terminar los procesos
        liberar_recursos();                             // Liberar los recursos
        exit(EXIT_FAILURE);                             // Salir con error
    case 0:                                             // Proceso hijo
        if (execl(RUTA_PISTA, CLASE_PISTA, NULL) == -1) // Ejecutar el proceso pista
        {
            fprintf(stderr, "[MANAGER] Error usando execl() en el proceso %s: %s.\n", CLASE_PISTA, strerror(errno)); // Mensaje de error
            exit(EXIT_FAILURE);                                                                                      // Salir con error
        }
    }

    g_process_pistas_table[indice_tabla].pid = pid;           // Asignar el PID del proceso a la tabla de procesos
    g_process_pistas_table[indice_tabla].clase = CLASE_PISTA; // Asignar la clase del proceso a la tabla de procesos
    g_pistasProcesses++;                                      // Incrementar el numero de procesos de pistas
}

// Lanzar un proceso de slot
void lanzar_proceso_slot(const int indice_tabla)
{
    pid_t pid; // PID del proceso

    switch (pid = fork()) // Crear un proceso hijo
    {
    case -1:                                                                               // Error
        fprintf(stderr, "[MANAGER] Error al lanzar proceso slot: %s.\n", strerror(errno)); // Mensaje de error
        // terminar_procesos();                                                               // Terminar los procesos
        liberar_recursos();                           // Liberar los recursos
        exit(EXIT_FAILURE);                           // Salir con error
    case 0:                                           // Proceso hijo
        if (execl(RUTA_SLOT, CLASE_SLOT, NULL) == -1) // Ejecutar el proceso slot
        {
            fprintf(stderr, "[MANAGER] Error usando execl() en el proceso %s: %s.\n", CLASE_SLOT, strerror(errno)); // Mensaje de error
            exit(EXIT_FAILURE);                                                                                     // Salir con error
        }
    }
    g_process_slots_table[indice_tabla].pid = pid;          // Asignar el PID del proceso a la tabla de procesos
    g_process_slots_table[indice_tabla].clase = CLASE_SLOT; // Asignar la clase del proceso a la tabla de procesos
    g_slotsProcesses++;                                     // Incrementar el numero de procesos de slots
}

// Esperar a que terminen los procesos
void esperar_procesos()
{
    int i;

    for (i = 0; i < NUMSLOTS; i++)
    {
        waitpid(g_process_slots_table[i].pid, 0, 0);
    }
}

// Terminar los procesos
void terminar_procesos(void)
{
    printf("\n----- [MANAGER] Terminar con cualquier proceso pendiente ejecutándose -----\n"); // Mensaje de terminacion

    terminar_procesos_especificos(g_process_slots_table, g_slotsProcesses);   // Terminar los procesos de slots
    terminar_procesos_especificos(g_process_pistas_table, g_pistasProcesses); // Terminar los procesos de pistas
}

// Terminar los procesos especificos
void terminar_procesos_especificos(struct TProcess_t *process_table, int process_num)
{
    int i; // Indice de la tabla de procesos

    for (i = 0; i < process_num; i++) // Recorrer la tabla de procesos
    {
        if (process_table[i].pid != 0) // Si el PID del proceso no es 0
        {
            printf("[MANAGER] Terminando proceso %s [%d]...\n", process_table[i].clase, process_table[i].pid); // Mensaje de terminacion
            if (kill(process_table[i].pid, SIGINT) == -1)                                                      // Enviar la senhal SIGINT al proceso
            {
                fprintf(stderr, "[MANAGER] Error al usar kill() en proceso %d: %s.\n", process_table[i].pid, strerror(errno)); // Mensaje de error
                exit(EXIT_FAILURE);                                                                                            // Salir con error
            }
        }
    }
}

// Liberar recursos
void liberar_recursos()
{
    free(g_process_pistas_table); // Liberar la memoria de la tabla de procesos de pistas
    free(g_process_slots_table);  // Liberar la memoria de la tabla de procesos de slots

    // Cerrar los buzones
    // if (mq_unlink(BUZON_ATERRIZAJES) == -1)
    // {
    //     fprintf(stderr, "[MANAGER] Error al eliminar el buzón BUZON_ATERRIZAJES. Detalles: %s\n", strerror(errno)); // Mensaje de error
    //     exit(EXIT_FAILURE);                                                                                         // Salir con error
    // }

    mq_unlink(BUZON_ATERRIZAJES);  // Cerrar el buzon de aterrizajes
    mq_close(qHandlerAterrizajes); // Cerrar el buzon de aterrizajes

    // Cerrar los buzones de slots
    for (int i = 0; i < NUMSLOTS; i++)
    {
        char buzon_slot[20];                         // Buzon de slot
        sprintf(buzon_slot, "%s%d", BUZON_SLOTS, i); // Formatear el nombre del buzon

        // Cerrar el buzon de slot si no se ha cerrado
        // if (mq_unlink(buzon_slot) == -1)
        // {
        //     fprintf(stderr, "[MANAGER] Error al eliminar el buzón %s. Detalles: %s\n", buzon_slot, strerror(errno)); // Mensaje de error
        //     exit(EXIT_FAILURE);                                                                                      // Salir con error
        // }

        mq_close(qHandlerSlots[i]); // Cerrar el buzon de slot
        mq_unlink(buzon_slot);      // Cerrar el buzon de slot
    }
}