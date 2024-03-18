#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

// Estructura que almacena los datos compartidos entre los procesos
struct datos_compartidos
{
    int saldo_en_euros; // Saldo en euros
    sem_t *sem_saldo;   // Semaforo para controlar el acceso a la variable saldo_en_euros
};

// Funciones para crear un semaforo
sem_t *crear_sem(char *nombre, int valor_inicial)
{
    sem_t *sem = sem_open(nombre, O_CREAT, 0644, valor_inicial); // Crea un semaforo con el nombre y el valor inicial

    // Si hay un error al crear el semaforo
    if (sem == SEM_FAILED)
    {
        fprintf(stderr, "sem_open"); // Se imprime un mensaje de error
        exit(EXIT_FAILURE);          // Se termina el programa
    }
    return sem;
}

// Funcion para decrementar el semáforo
void wait_sem(sem_t *sem)
{
    while (sem_wait(sem) != 0); // Para decrementar el valor al que apunta sem
}
// Funciones para incrementar sem cada vez que le llamamos
void signal_sem(sem_t *sem)
{
    sem_post(sem); // Para incrementar el valor al que apunta sem
}

// Funcion para destruir un semaforo
void destruir_sem(char *nombre)
{

    // Si hay un error al destruir el semaforo
    if (sem_unlink(nombre) < 0)
    {
        fprintf(stderr, "Error al destruir el semáforo"); // Se imprime un mensaje de error
        exit(EXIT_FAILURE);                               // Se termina el programa
    }
}

// Funcion para crear memoria compartida
void *crear_memoria_compartida(size_t tamano)
{
    int proteccion = PROT_READ | PROT_WRITE;                   // Lectura y escritura permitidas para el proceso de memoria
    int visibilidad = MAP_SHARED | MAP_ANONYMOUS;              // Compartida y sin archivo asociado a la memoria compartida
    return mmap(NULL, tamano, proteccion, visibilidad, -1, 0); // Devuelve un puntero a la memoria compartida
}

// Funcion para inicializar las variables compartidas
void inicializar_variables_compartidas(struct datos_compartidos *datos)
{
    datos->saldo_en_euros = 0;                    // Inicializa el saldo en euros a 0
    datos->sem_saldo = crear_sem("sem_saldo", 1); // Crea el semaforo para controlar el acceso a la variable saldo_en_euros y lo inicializaremos a 1
    // para que trabaje al estilo mutex
}

// Funcion para los ahorradores, que buscarán ahorrar a toda costa
void ahorrador(struct datos_compartidos *datos, int num_ingresos, int cantidad_ingreso, pid_t pid)
{

    // Bucle para realizar los ingresos
    for (int i = 0; i < num_ingresos; i++)
    {
        // Accedemos a sección crítica y decrementamos el valor de sem saldo
        wait_sem(datos->sem_saldo);
        // Con cada ingremos incrementamos el saldo
        datos->saldo_en_euros += cantidad_ingreso;
        // Imprime el ingreso
        printf("[Ahorrador %d]: Ingreso de %d euros. Nuevo saldo: %d euros\n", pid, cantidad_ingreso, datos->saldo_en_euros);
        // Desbloquea la sección crítica aumentando el valor al que apunta sem
        signal_sem(datos->sem_saldo);
        // Esperamos un segundo
        usleep(1000000);
    }
}

// Funcion para los gastones, que pretenderán gastar a toda costa
void gastones(struct datos_compartidos *datos, int num_retiradas, int cantidad_retirada, pid_t pid)
{

    // Bucle para realizar las retiradas
    for (int i = 0; i < num_retiradas; i++)
    {
        // Accedemos a sección crítica y decrementamos el valor de sem saldo
        wait_sem(datos->sem_saldo); // Espera el semaforo

        // Si hay suficiente saldo para retirar la cantidad
        if (datos->saldo_en_euros >= cantidad_retirada)
        {
            // Retiramos la cantidad que queremos
            datos->saldo_en_euros -= cantidad_retirada;
            printf("[Gaston %d]: Retirada de %d euros. Nuevo saldo: %d euros\n", pid, cantidad_retirada, datos->saldo_en_euros);
        }
        else
        {
            // Si no hay suficiente saldo imprimimos que no hay suficiente saldo
        }
        printf("[Gaston %d]: No hay suficiente saldo para retirar %d euros :( \n", pid, cantidad_retirada);
    }
    signal_sem(datos->sem_saldo); // Incrementamos el semáforo
    usleep(1500000);              // Espera 1.5 segundos
}

int main()
{
    const int NUM_AHORRADORES = 2;     // Numero de ahorradores
    const int NUM_GASTONES = 3;        // Numero de gastones
    const int NUM_INGRESOS = 5;        // Numero de ingresos
    const int NUM_RETIRADAS = 4;       // Numero de retiradas
    const int CANTIDAD_INGRESO = 100;  // Cantidad de ingreso
    const int CANTIDAD_RETIRADA = 150; // Cantidad de retirada

    // Crea la memoria compartida con tamaño de datos_compartidos
    struct datos_compartidos *datos = crear_memoria_compartida(sizeof(struct datos_compartidos));

    // Si hay un error al crear la memoria compartida
    if (datos == NULL)
    {
        fprintf(stderr, "Error al crear memoria compartida"); // Se imprime un mensaje de error
        exit(EXIT_FAILURE);                                   // Se termina el programa
    }

    inicializar_variables_compartidas(datos); // Inicializa las variables compartidas

    // Bucle para crear los procesos ahorradores
    for (int i = 0; i < NUM_AHORRADORES; i++)
    {
        pid_t pid = fork(); // Crea un proceso hijo

        // Si el proceso es el proceso hijo
        if (pid == 0)
        {
            ahorrador(datos, NUM_INGRESOS, CANTIDAD_INGRESO, pid); // Realiza los ingresos
            exit(EXIT_SUCCESS);                                    // Termina el proceso hijo
        }
        else if (pid < 0)
        {                                                           // Si hay un error al crear el proceso ahorrador
            fprintf(stderr, "Error al crear el proceso ahorrador"); // Se imprime un mensaje de error
            exit(EXIT_FAILURE);                                     // Se termina el programa
        }
    }

    // Bucle para crear los procesos gastones
    for (int i = 0; i < NUM_GASTONES; i++)
    {
        pid_t pid = fork(); // Crea un proceso hijo

        // Si el proceso es el proceso hijo
        if (pid == 0)
        {
            gastones(datos, NUM_RETIRADAS, CANTIDAD_RETIRADA, pid); // Realiza las retiradas
            exit(EXIT_SUCCESS);                                     // Termina el proceso hijo
        }
        else if (pid < 0)
        {                                                          // Si hay un error al crear el proceso gastones
            fprintf(stderr, "Error al crear el proceso gastones"); // Se imprime un mensaje de error
            exit(EXIT_FAILURE);                                    // Se termina el programa
        }
    }

    // Bucle para esperar a que terminen los procesos ahorradores y gastones
    for (int i = 0; i < NUM_AHORRADORES + NUM_GASTONES; i++)
    {
        wait(NULL); // Espera a que termine un proceso hijo, da igual quien sea y el orden de fin
    }

    destruir_sem("sem_saldo");                       // Destruye el semaforo
    munmap(datos, sizeof(struct datos_compartidos)); // Libera la memoria compartida

    return 0; // Termina el programa con éxito :)
}