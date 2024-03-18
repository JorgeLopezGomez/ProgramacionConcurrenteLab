
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

    // Coge semáforos y memoria compartida
    
    // Se pone en estado de libre incrementando el número de pistas libres
    while(1){

        // Mensaje de Espera
        printf("Pista [%d] en servicio...\n",pid);
        
        //TODO: Aquí hay que realizar procesos
        
        // Mensaje de pista ocupada
        printf("Pista [%d] ocupada... \n",pid);
        printf("Nº de Aviones en espera: %d\n",valorEspera);

        // Espera en entre 30..60 segundos
        sleep(rand() % 31 + 30);
        
    }

    return EXIT_SUCCESS;
}