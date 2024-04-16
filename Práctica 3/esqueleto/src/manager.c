
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

// Crear buzones
void crear_buzones() {
    mqd_t mq;
    struct mq_attr attr;
    char buzon_slot[20];

    // Atributos del buzón
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = TAMANO_MENSAJES;
    attr.mq_curmsgs = 0;

    // Crear buzón para aterrizajes
    mq = mq_open(BUZON_ATERRIZAJES, O_CREAT | O_RDWR, 0644, &attr);
    if (mq == (mqd_t) -1) {
        fprintf(stderr, "[MANAGER] Error al abrir el buzón BUZON_ATERRIZAJES. Detalles: %s\n", strerror(errno));
        exit(1);
    }
    mq_close(mq);

    // Crear un buzón para cada slot
    for (int i = 0; i < NUMSLOTS; i++) {
        sprintf(buzon_slot, "%s%d", BUZON_SLOTS, i);
        mq = mq_open(buzon_slot, O_CREAT | O_RDWR, 0644, &attr);
        if (mq == (mqd_t) -1) {
            fprintf(stderr, "[MANAGER] Error al abrir el buzón %s. Detalles: %s\n", buzon_slot, strerror(errno));
            exit(1);
        }
        mq_close(mq);
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
    g_process_pistas_table = (struct TProcess_t *)malloc(n_procesos_telefono * sizeof(struct TProcess_t)); // Reservar memoria para la tabla de procesos de pistas

    // Comprobar si se ha reservado memoria correctamente
    if (g_process_pistas_table == NULL)
    {
        fprintf(stderr, "[MANAGER] Error al reservar memoria para la tabla de procesos de pistas: %s\n", strerror(errno)); // Mensaje de error
        exit(1);                                                                                                           // Salir con error
    }

    g_process_slots_table = (struct TProcess_t *)malloc(n_procesos_linea * sizeof(struct TProcess_t)); // Reservar memoria para la tabla de procesos de slots

    // Comprobar si se ha reservado memoria correctamente
    if (g_process_slots_table == NULL)
    {
        fprintf(stderr, "[MANAGER] Error al reservar memoria para la tabla de procesos de slots: %s\n", strerror(errno)); // Mensaje de error
        exit(1);                                                                                                          // Salir con error
    }
}

// Crear procesos
void crear_procesos(int numPistas, int numSlots)
{
    // Crear procesos de slots
    for (int i = 0; i < numSlots; i++)
    {
        // Crear un proceso de slot
        if (fork() == 0)
        {
            lanzar_proceso_slot(i); // Lanzar un proceso de slot
            return;                 // Salir del bucle
        }
    }
    printf("[MANAGER] %d slots creados.\n", numSlots); // Mensaje de creacion de slots

    // Crear procesos de pistas
    for (int i = 0; i < numPistas; i++)
    {
        // Crear un proceso de pista
        if (fork() == 0)
        {
            lanzar_proceso_pista(i); // Lanzar un proceso de pista
            return;                  // Salir del bucle
        }
    }
    printf("[MANAGER] %d pistas creadas.\n", numPistas); // Mensaje de creacion de pistas
}

// Lanzar un proceso de pista
void lanzar_proceso_pista(const int indice_tabla)
{
    pid_t pid; // PID del proceso

    switch (pid = fork()) // Crear un proceso hijo
    {
    case -1:                                                                                // Error
        fprintf(stderr, "[MANAGER] Error al lanzar proceso pista: %s.\n", strerror(errno)); // Mensaje de error
        terminar_procesos();                                                                // Terminar los procesos
        liberar_recursos();                                                                 // Liberar los recursos
        exit(EXIT_FAILURE);                                                                 // Salir con error
    case 0:                                                                                 // Proceso hijo
        if (execl(RUTA_PISTA, CLASE_PISTA, NULL) == -1)                                     // Ejecutar el proceso pista
        {
            fprintf(stderr, "[MANAGER] Error usando execl() en el proceso %s: %s.\n", CLASE_PISTA, strerror(errno)); // Mensaje de error
            exit(EXIT_FAILURE);                                                                                      // Salir con error
        }
    }

    g_process_pistas_table[indice_tabla].pid = pid;           // Asignar el PID del proceso a la tabla de procesos
    g_process_pistas_table[indice_tabla].clase = CLASE_PISTA; // Asignar la clase del proceso a la tabla de procesos
}

// Lanzar un proceso de slot
void lanzar_proceso_slot(const int indice_tabla)
{
    pid_t pid; // PID del proceso

    switch (pid = fork()) // Crear un proceso hijo
    {
    case -1:                                                                               // Error
        fprintf(stderr, "[MANAGER] Error al lanzar proceso slot: %s.\n", strerror(errno)); // Mensaje de error
        terminar_procesos();                                                               // Terminar los procesos
        liberar_recursos();                                                                // Liberar los recursos
        exit(EXIT_FAILURE);                                                                // Salir con error
    case 0:                                                                                // Proceso hijo
        if (execl(RUTA_SLOT, CLASE_SLOT, NULL) == -1)                                      // Ejecutar el proceso slot
        {
            fprintf(stderr, "[MANAGER] Error usando execl() en el proceso %s: %s.\n", CLASE_SLOT, strerror(errno)); // Mensaje de error
            exit(EXIT_FAILURE);                                                                                     // Salir con error
        }
    }
    g_process_slots_table[indice_tabla].pid = pid;          // Asignar el PID del proceso a la tabla de procesos
    g_process_slots_table[indice_tabla].clase = CLASE_SLOT; // Asignar la clase del proceso a la tabla de procesos
}

// Esperar a que terminen los procesos
void esperar_procesos()
{
    int i;                            // Contador
    int nProcesos = g_slotsProcesses; // Numero de procesos
    pid_t pid;                        // PID del proceso

    // Esperar a que terminen los procesos
    while (nProcesos > 0)
    {
        pid = wait(NULL); // Esperar a que termine un proceso
        nProcesos--;      // Decrementar el numero de procesos

        // Buscar el proceso en la tabla de procesos
        for (i = 0; i < g_slotsProcesses; i++)
        {
            // Si el PID del proceso es igual al PID del proceso en la tabla de procesos
            if (pid == g_process_slots_table[i].pid)
            {
                printf("[MANAGER] Proceso %s terminado [%d]...\n", g_process_slots_table[i].clase, g_process_slots_table[i].pid); // Mensaje de terminacion
                g_process_slots_table[i].pid = 0;                                                                                 // Limpiar el PID del proceso en la tabla de procesos
                break;                                                                                                            // Salir del bucle
            }
        }
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
            if (kill(process_table[i].pid, SIGINT) == -1)                                                      // Enviar la señal SIGINT al proceso
            {
                fprintf(stderr, "[MANAGER] Error al usar kill() en proceso %d: %s.\n", process_table[i].pid, strerror(errno)); // Mensaje de error
            }
        }
    }
}

// Liberar recursos
void liberar_recursos()
{
    // Cerrar los buzones
    if (mq_unlink(BUZON_ATERRIZAJES) == -1) {
        fprintf(stderr, "[MANAGER] Error al eliminar el buzón BUZON_ATERRIZAJES. Detalles: %s\n", strerror(errno));
    }
    for (int i = 0; i < NUMSLOTS; i++)
    {
        char buzon_slot[20];
        sprintf(buzon_slot, "%s%d", BUZON_SLOTS, i);
        if (mq_unlink(buzon_slot) == -1) {
            fprintf(stderr, "[MANAGER] Error al eliminar el buzón %s. Detalles: %s\n", buzon_slot, strerror(errno));
        }
    }

    // Liberar la memoria de la tabla de procesos de pistas
    if (g_process_pistas_table != NULL)
    {
        free(g_process_pistas_table);
    }

    // Liberar la memoria de la tabla de procesos de slots
    if (g_process_slots_table != NULL)
    {
        free(g_process_slots_table);
    }
}