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
    pid_t pid = getpid();
    srand(pid);
    int valorEspera = 0;

    // Coge semáforos y memoria compartida
    sem_t *sem_pistas = get_sem(PISTAS);
    sem_t *sem_aviones = get_sem(AVIONES);
    sem_t *sem_mutex = get_sem(MUTEXESPERA);
    int shm_fd = obtener_var(AVIONESESPERA);

    // Espera en entre 30..60 segundos
    printf("Avion[%d] en camino al aeropuerto\n", pid);
    sleep(rand() % 31 + 30);

    // Aumenta los aviones en espera
    wait_sem(sem_mutex);
    consultar_var(shm_fd, &valorEspera);
    modificar_var(shm_fd, ++valorEspera);
    printf("Nº de Aviones en espera: %d\n", valorEspera);
    signal_sem(sem_mutex);

    // Espera una pista libre
    printf("Avion [%d] esperando pista libre...\n", pid);
    wait_sem(sem_pistas);

    // Comienza el aterrizaje
    printf("Avion [%d] comenzando aterrizaje...\n", pid);

    // Espera de 60 segundos
    sleep(60);
    printf("Avion [%d] aparcado...\n", pid);

    // Libera la pista
    signal_sem(sem_aviones);

    return EXIT_SUCCESS;
}