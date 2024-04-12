#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <definitions.h>
#include <memoriaI.h>
#include <semaforoI.h>

// Modulo principal
int main(int argc, char *argv[])
{

    // Define variables locales
    pid_t pid = getpid(); // Obtiene el PID del proceso actual
    srand(pid);           // Inicializa el generador de numeros aleatorios
    int valorEspera = 0;  // Numero de aviones en espera

    // Coge semaforos y memoria compartida
    sem_t *sem_pistas = get_sem(PISTAS);     // Semaforo de pistas
    sem_t *sem_aviones = get_sem(AVIONES);   // Semaforo de aviones
    sem_t *sem_mutex = get_sem(MUTEXESPERA); // Semaforo mutex de espera
    int shm_fd = obtener_var(AVIONESESPERA); // Memoria compartida de aviones en espera

    // Se pone en estado de libre incrementando el numero de pistas libres
    while (1)
    {

        // Mensaje de Espera
        printf("Pista [%d] en servicio...\n", pid); // Mensaje de pista en servicio
        signal_sem(sem_pistas);                     // Incrementa el numero de pistas libres
        wait_sem(sem_aviones);                      // Espera a que un avion aterrice

        // Espera a que un avion aterrice
        wait_sem(sem_mutex);                 // Espera a que el semaforo de espera este libre
        consultar_var(shm_fd, &valorEspera); // Consulta el numero de aviones en espera en la memoria compartida de aviones en espera
        valorEspera--;                       // Decrementa el numero de aviones en espera
        modificar_var(shm_fd, valorEspera);  // Modifica el numero de aviones en espera en la memoria compartida de aviones en espera
        signal_sem(sem_mutex);               // Libera el semaforo de espera

        // Mensaje de pista ocupada
        printf("Pista [%d] ocupada... \n", pid);
        printf("Nº de Aviones en espera: %d\n", valorEspera); // Muestra el numero de aviones en espera

        // Espera en entre 30..60 segundos
        sleep(rand() % 31 + 30);
    }

    return EXIT_SUCCESS; // Finaliza el programa
}