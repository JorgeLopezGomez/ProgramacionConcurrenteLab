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
    pid_t pid = getpid(); // Obtiene el pid del proceso
    srand(pid);           // Inicializa el generador de numeros aleatorios
    int valorEspera = 0;  // Numero de aviones en espera

    // Coge semáforos y memoria compartida
    sem_t *sem_pistas = get_sem(PISTAS);     // Semaforo de pistas
    sem_t *sem_aviones = get_sem(AVIONES);   // Semaforo de aviones
    sem_t *sem_mutex = get_sem(MUTEXESPERA); // Semaforo mutex de espera
    int shm_fd = obtener_var(AVIONESESPERA); // Memoria compartida de aviones en espera

    // Espera en entre 30..60 segundos
    printf("Avion[%d] en camino al aeropuerto\n", pid); // Mensaje de avion en camino al aeropuerto
    sleep(rand() % 31 + 30);                            // Espera entre 30 y 60 segundos

    // Aumenta los aviones en espera
    wait_sem(sem_mutex);                                  // Espera a que el semaforo de espera este libre
    consultar_var(shm_fd, &valorEspera);                  // Consulta el numero de aviones en espera
    valorEspera++;                                        // Incrementa el numero de aviones en espera
    modificar_var(shm_fd, valorEspera);                   // Modifica el numero de aviones en espera
    printf("Nº de Aviones en espera: %d\n", valorEspera); // Muestra el numero de aviones en espera
    signal_sem(sem_mutex);                                // Libera el semaforo de espera

    // Espera una pista libre
    printf("Avion [%d] esperando pista libre...\n", pid); // Mensaje de avion esperando pista libre
    wait_sem(sem_pistas);                                 // Espera a que haya una pista libre
    signal_sem(sem_aviones);                              // Incrementa el numero de aviones en espera

    // Comienza el aterrizaje
    printf("Avion [%d] comenzando aterrizaje...\n", pid);

    // Espera de 60 segundos
    sleep(60);
    printf("Avion [%d] aparcado...\n", pid); // Mensaje de avion aparcado

    return EXIT_SUCCESS; // Finaliza el programa
}