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

    // Se pone en estado de libre incrementando el número de pistas libres
    while (1)
    {

        // Mensaje de Espera
        printf("Pista [%d] en servicio...\n", pid);
        signal_sem(sem_pistas);
        wait_sem(sem_aviones);

        // Espera a que un avión aterrice
        wait_sem(sem_mutex);
        consultar_var(shm_fd, &valorEspera);
        valorEspera--;
        modificar_var(shm_fd, valorEspera);
        signal_sem(sem_mutex);

        // Mensaje de pista ocupada
        printf("Pista [%d] ocupada... \n", pid);
        printf("Nº de Aviones en espera: %d\n", valorEspera);

        // Espera en entre 30..60 segundos
        sleep(rand() % 31 + 30);
    }

    return EXIT_SUCCESS;
}