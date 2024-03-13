#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

// Estructura para almacenar los datos compartidos
struct datos_compartidos {
    int saldo_en_euros; // Saldo compartido
    sem_t sem_saldo; // Semáforo para proteger el acceso al saldo
};

// Funciones para crear y gestionar la memoria compartida
void *crear_memoria_compartida(size_t tamano) { 
    int proteccion = PROT_READ | PROT_WRITE; // Lectura y escritura 
    int visibilidad = MAP_SHARED | MAP_ANONYMOUS; // Compartida y sin archivo asociado 
    return mmap(NULL, tamano, proteccion, visibilidad, -1, 0); // Crear memoria compartida
}

// Funcion para inicializar las variables compartidas
void inicializar_variables_compartidas(struct datos_compartidos *datos) {
    datos->saldo_en_euros = 0; // Inicializacion del saldo compartido
    sem_init(&datos->sem_saldo, 1, 1); // Inicializacion del semaforo
}

// Funcion para realizar ingresos
void ahorrador(struct datos_compartidos *datos, int num_ingresos, int cantidad_ingreso) {
    for (int i = 0; i < num_ingresos; i++) { // Bucle para realizar ingresos 
        sem_wait(&datos->sem_saldo); // Esperar por el semaforo
        datos->saldo_en_euros += cantidad_ingreso; // Modificar el saldo compartido
        printf("Ahorrador: Ingreso de %d euros. Nuevo saldo: %d euros\n", cantidad_ingreso, datos->saldo_en_euros); // Imprimir mensaje de ingreso
        sem_post(&datos->sem_saldo); // Liberar el semaforo
        usleep(100000); // Simular procesamiento
    }
}

// Funcion para realizar retiradas
void gastos(struct datos_compartidos *datos, int num_retiradas, int cantidad_retirada) {
    for (int i = 0; i < num_retiradas; i++) { // Bucle para realizar retiradas
        sem_wait(&datos->sem_saldo); // Esperar por el semaforo
        if (datos->saldo_en_euros >= cantidad_retirada) { // Comprobar si hay suficiente saldo
            datos->saldo_en_euros -= cantidad_retirada; // Modificar el saldo compartido
            printf("Gastos: Retirada de %d euros. Nuevo saldo: %d euros\n", cantidad_retirada, datos->saldo_en_euros); // Imprimir mensaje de retirada
        } else {
            printf("Gastos: No hay suficiente saldo para retirar %d euros\n", cantidad_retirada); // Imprimir mensaje de error
        }
        sem_post(&datos->sem_saldo); // Liberar el semaforo
        usleep(150000); // Simular procesamiento
    }
}

// Funcion principal
int main() {
    const int NUM_AHORRADORES = 2; // Numero de ahorradores
    const int NUM_GASTOS = 3; // Numero de gastos
    const int NUM_INGRESOS = 5; // Numero de ingresos
    const int NUM_RETIRADAS = 4; // Numero de retiradas
    const int CANTIDAD_INGRESO = 100; // Cantidad de dinero a ingresar
    const int CANTIDAD_RETIRADA = 150; // Cantidad de dinero a retirar

    // Crear memoria compartida para almacenar datos
    struct datos_compartidos *datos = crear_memoria_compartida(sizeof(struct datos_compartidos));

    // Comprobar si se ha creado la memoria compartida
    if (datos == NULL) { 
        fprintf(stderr, "Error al crear memoria compartida"); // Imprimir mensaje de error
        exit(EXIT_FAILURE); // Terminar el proceso principal
    }

    // Inicializar variables compartidas
    inicializar_variables_compartidas(datos);

    // Crear procesos ahorradores
    for (int i = 0; i < NUM_AHORRADORES; i++) {
        pid_t pid = fork(); // Crear un nuevo proceso hijo en cada iteracion del bucle de ahorradores de la clase principal
        if (pid == 0) { // Proceso hijo
            ahorrador(datos, NUM_INGRESOS, CANTIDAD_INGRESO); // Llamar a la funcion ahorrador para cada proceso hijo creado en la clase principal
            exit(EXIT_SUCCESS); // Terminar el proceso hijo
        } else if (pid < 0) { // Error en fork
            fprintf(stderr, "Error al crear el proceso ahorrador"); // Imprimir mensaje de error
            exit(EXIT_FAILURE); // Terminar el proceso principal
        }
    } 

    // Crear procesos gastos
    for (int i = 0; i < NUM_GASTOS; i++) { // Bucle para crear procesos gastos
        pid_t pid = fork(); // Crear un nuevo proceso hijo en cada iteracion del bucle de gastoses de la clase principal
        if (pid == 0) { // Proceso hijo
            gastos(datos, NUM_RETIRADAS, CANTIDAD_RETIRADA); // Llamar a la funcion gastos
            exit(EXIT_SUCCESS); // Terminar el proceso hijo
        } else if (pid < 0) { // Error en fork
            fprintf(stderr, "Error al crear el proceso gastos"); // Imprimir mensaje de error
            exit(EXIT_FAILURE); // Terminar el proceso principal
        }
    }

    // Esperar a que todos los procesos hijos terminen
    for (int i = 0; i < NUM_AHORRADORES + NUM_GASTOS; i++) {
        wait(NULL); // Esperar a que un proceso hijo termine
    }

    // Liberar recursos
    sem_destroy(&datos->sem_saldo); // Destruir el semaforo
    munmap(datos, sizeof(struct datos_compartidos)); // Liberar memoria compartida 

    return 0; // Terminar el proceso principal
}