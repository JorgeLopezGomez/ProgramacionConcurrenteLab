#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <definitions.h>
#include <memoriaI.h>
#include <semaforoI.h>

// Modulo principal
int main(int argc,char *argv[]){

	//TODO: Esquema según especificado en la práctica.

    // Define variables locales
    pid_t pid = getpid();
    int valorEspera;

    // Coge semáforos y memoria compartida
    sem_t *sem_pistas = get_semaphore(PISTAS);
    int *num_aviones_en_espera = get_shared_mem(AVIONESESPERA);

    // Se pone en estado de libre incrementando el número de pistas libres
    while(1){

        // Mensaje de Espera
        printf("Pista [%d] en servicio...\n",pid);
        
        //TODO: Aquí hay que realizar procesos
        sem_wait(sem_pistas);
        valorEspera = --(*num_aviones_en_espera);

        // Mensaje de pista ocupada
        printf("Pista [%d] ocupada... \n",pid);
        printf("Nº de Aviones en espera: %d\n",valorEspera);

        // Espera en entre 30..60 segundos
        sleep(rand() % 31 + 30);
        
        sem_post(sem_pistas);
    }

    return EXIT_SUCCESS;
}