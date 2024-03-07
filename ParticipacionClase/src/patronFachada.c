#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>

// Estructura para almacenar los datos compartidos
struct shared_data {
    int saldo_en_euros;
    sem_t sem_saldo; // Semáforo para proteger el acceso al saldo
};

void *crear_shared_memory(size_t size) {
    int protection = PROT_READ | PROT_WRITE;
    int visibility = MAP_SHARED | MAP_ANONYMOUS;
    return mmap(NULL, size, protection, visibility, -1, 0);
}

void inicializar_variables_compartidas(struct shared_data *data) {
    data->saldo_en_euros = 0;
    sem_init(&(data->sem_saldo), 1, 1); // Inicialización del semáforo
}

void ahorrador(struct shared_data *data, int num_ingresos, int cantidad_ingreso) {
    for (int i = 0; i < num_ingresos; i++) {
        sem_wait(&(data->sem_saldo)); // Esperar por el semáforo
        data->saldo_en_euros += cantidad_ingreso; // Modificar el saldo compartido
        printf("Ahorrador: Ingreso de %d euros. Nuevo saldo: %d euros\n", cantidad_ingreso, data->saldo_en_euros);
        sem_post(&(data->sem_saldo)); // Liberar el semáforo
        usleep(100000); // Simular procesamiento
    }
}

void gaston(struct shared_data *data, int num_retiradas, int cantidad_retirada) {
    for (int i = 0; i < num_retiradas; i++) {
        sem_wait(&(data->sem_saldo)); // Esperar por el semáforo
        if (data->saldo_en_euros >= cantidad_retirada) {
            data->saldo_en_euros -= cantidad_retirada; // Modificar el saldo compartido
            printf("Gastón: Retirada de %d euros. Nuevo saldo: %d euros\n", cantidad_retirada, data->saldo_en_euros);
        } else {
            printf("Gastón: No hay suficiente saldo para retirar %d euros\n", cantidad_retirada);
        }
        sem_post(&(data->sem_saldo)); // Liberar el semáforo
        usleep(150000); // Simular procesamiento
    }
}

int main() {
    const int NUM_AHORRADORES = 2;
    const int NUM_GASTONES = 3;
    const int NUM_INGRESOS = 5;
    const int NUM_RETIRADAS = 4;
    const int CANTIDAD_INGRESO = 100;
    const int CANTIDAD_RETIRADA = 150;

    // Crear memoria compartida para almacenar datos
    struct shared_data *data = crear_shared_memory(sizeof(struct shared_data));
    if (data == NULL) {
        fprintf(stderr, "Error al crear memoria compartida");
        exit(EXIT_FAILURE);
    }

    // Inicializar variables compartidas
    inicializar_variables_compartidas(data);

    // Crear procesos ahorradores
    for (int i = 0; i < NUM_AHORRADORES; i++) {
        pid_t pid = fork();
        if (pid == 0) { // Proceso hijo
            ahorrador(data, NUM_INGRESOS, CANTIDAD_INGRESO);
            exit(EXIT_SUCCESS);
        } else if (pid < 0) { // Error en fork
            fprintf(stderr, "Error al crear el proceso ahorrador");
            exit(EXIT_FAILURE);
        }
    }

    // Crear procesos gastones
    for (int i = 0; i < NUM_GASTONES; i++) {
        pid_t pid = fork();
        if (pid == 0) { // Proceso hijo
            gaston(data, NUM_RETIRADAS, CANTIDAD_RETIRADA);
            exit(EXIT_SUCCESS);
        } else if (pid < 0) { // Error en fork
            fprintf(stderr, "Error al crear el proceso gastón");
            exit(EXIT_FAILURE);
        }
    }

    // Esperar a que todos los procesos hijos terminen
    for (int i = 0; i < NUM_AHORRADORES + NUM_GASTONES; i++) {
        wait(NULL);
    }

    // Liberar recursos
    sem_destroy(&(data->sem_saldo));
    munmap(data, sizeof(struct shared_data));

    return 0;
}