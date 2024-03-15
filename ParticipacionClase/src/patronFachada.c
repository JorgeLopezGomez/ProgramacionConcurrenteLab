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

// Funciones para esperar y senalizar un semaforo
void wait_sem(sem_t *sem)
{

    // Si hay un error al esperar el semaforo
    if (sem_wait(sem) < 0)
    {
        fprintf(stderr, "sem_wait"); // Se imprime un mensaje de error
        exit(EXIT_FAILURE);          // Se termina el programa
    }
}
// Funciones para esperar y senalizar un semaforo
void signal_sem(sem_t *sem)
{

    // Si hay un error al esperar el semaforo
    if (sem_post(sem) < 0)
    {
        fprintf(stderr, "sem_post"); // Se imprime un mensaje de error
        exit(EXIT_FAILURE);          // Se termina el programa
    }
}

// Funcion para destruir un semaforo
void destruir_sem(char *nombre)
{

    // Si hay un error al destruir el semaforo
    if (sem_unlink(nombre) < 0)
    {
        fprintf(stderr, "sem_unlink"); // Se imprime un mensaje de error
        exit(EXIT_FAILURE);            // Se termina el programa
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
    datos->sem_saldo = crear_sem("sem_saldo", 1); // Crea el semaforo para controlar el acceso a la variable saldo_en_euros
}

// Funcion para ahorrar dinero
void ahorrador(struct datos_compartidos *datos, int num_ingresos, int cantidad_ingreso)
{

    // Bucle para realizar los ingresos
    for (int i = 0; i < num_ingresos; i++)
    {
        wait_sem(datos->sem_saldo);                                                                                 // Espera el semaforo
        datos->saldo_en_euros += cantidad_ingreso;                                                                  // Ingresa la cantidad de dinero
        printf("Ahorrador: Ingreso de %d euros. Nuevo saldo: %d euros\n", cantidad_ingreso, datos->saldo_en_euros); // Imprime el ingreso
        signal_sem(datos->sem_saldo);                                                                               // Senaliza el semaforo
        usleep(100000);                                                                                             // Espera 100 milisegundos
    }
}

// Funcion para retirar dinero
void gastos(struct datos_compartidos *datos, int num_retiradas, int cantidad_retirada)
{

    // Bucle para realizar las retiradas
    for (int i = 0; i < num_retiradas; i++)
    {
        wait_sem(datos->sem_saldo); // Espera el semaforo

        // Si hay suficiente saldo para retirar la cantidad
        if (datos->saldo_en_euros >= cantidad_retirada)
        {
            datos->saldo_en_euros -= cantidad_retirada;                                                                // Retira la cantidad de dinero
            printf("Gastos: Retirada de %d euros. Nuevo saldo: %d euros\n", cantidad_retirada, datos->saldo_en_euros); // Imprime la retirada
        }
        else
        {                                                                                         // Si no hay suficiente saldo para retirar la cantidad
            printf("Gastos: No hay suficiente saldo para retirar %d euros\n", cantidad_retirada); // Imprime que no hay suficiente saldo
        }
        signal_sem(datos->sem_saldo); // Senaliza el semaforo
        usleep(150000);               // Espera 150 milisegundos
    }
}

// Funcion principal
int main()
{
    const int NUM_AHORRADORES = 2;     // Numero de ahorradores
    const int NUM_GASTOS = 3;          // Numero de gastos
    const int NUM_INGRESOS = 5;        // Numero de ingresos
    const int NUM_RETIRADAS = 4;       // Numero de retiradas
    const int CANTIDAD_INGRESO = 100;  // Cantidad de ingreso
    const int CANTIDAD_RETIRADA = 150; // Cantidad de retirada

    struct datos_compartidos *datos = crear_memoria_compartida(sizeof(struct datos_compartidos)); // Crea la memoria compartida para los datos compartidos

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
            ahorrador(datos, NUM_INGRESOS, CANTIDAD_INGRESO); // Realiza los ingresos
            exit(EXIT_SUCCESS);                               // Termina el proceso hijo
        }
        else if (pid < 0)
        {                                                           // Si hay un error al crear el proceso ahorrador
            fprintf(stderr, "Error al crear el proceso ahorrador"); // Se imprime un mensaje de error
            exit(EXIT_FAILURE);                                     // Se termina el programa
        }
    }

    // Bucle para crear los procesos gastos
    for (int i = 0; i < NUM_GASTOS; i++)
    {
        pid_t pid = fork(); // Crea un proceso hijo

        // Si el proceso es el proceso hijo
        if (pid == 0)
        {
            gastos(datos, NUM_RETIRADAS, CANTIDAD_RETIRADA); // Realiza las retiradas
            exit(EXIT_SUCCESS);                              // Termina el proceso hijo
        }
        else if (pid < 0)
        {                                                        // Si hay un error al crear el proceso gastos
            fprintf(stderr, "Error al crear el proceso gastos"); // Se imprime un mensaje de error
            exit(EXIT_FAILURE);                                  // Se termina el programa
        }
    }

    // Bucle para esperar a que terminen los procesos ahorradores y gastos
    for (int i = 0; i < NUM_AHORRADORES + NUM_GASTOS; i++)
    {
        wait(NULL); // Espera a que termine un proceso hijo
    }

    destruir_sem("sem_saldo");                       // Destruye el semaforo
    munmap(datos, sizeof(struct datos_compartidos)); // Libera la memoria compartida

    return 0; // Termina el programa
}