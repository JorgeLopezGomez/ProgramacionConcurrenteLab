
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <definitions.h>
#include <memoriaI.h>
#include <semaforoI.h>

// Modulo principal
int main(int argc,char *argv[]){

    //TODO: Esquema especificado en la práctica

    // Define variables locales

    // Coge semáforos y memoria compartida

    // Espera en entre 30..60 segundos
    printf("Avion[%d] en camino al aeropuerto\n",pid);
    sleep(rand() % 31 + 30);

    //Aumenta los aviones en espera

    // Espera una pista libre

    // Comienza el aterrizaje

    // Espera de 60 segundos
    sleep(60);
    printf("Avion[%d] aparcado...\n",pid);

    return EXIT_SUCCESS;
}