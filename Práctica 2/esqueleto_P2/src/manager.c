#include <errno.h>
#include <linux/limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#include <definitions.h>
#include <memoriaI.h>
#include <semaforoI.h>


void procesar_argumentos(int argc, char *argv[], int *numPistas, int *numAviones);
void instalar_manejador_senhal();
void manejador_senhal(int sign);
void iniciar_tabla_procesos(int n_procesos_pista, int n_procesos_avion);
void crear_procesos(int numPistas, int numAviones);
void lanzar_proceso_pista(const int indice_tabla);
void lanzar_proceso_avion(const int indice_tabla);
void esperar_procesos();
void terminar_procesos(void);
void terminar_procesos_especificos(struct TProcess_t *process_table, int process_num);
void liberar_recursos();

int g_pistasProcesses = 0;
int g_avionesProcesses = 0;
struct TProcess_t *g_process_pistas_table;
struct TProcess_t *g_process_aviones_table;


int main(int argc, char *argv[])
{
    // Define variables locales
    int numPistas;
    int numAviones;

    // Procesa los argumentos y los guarda en las dos variables
    procesar_argumentos(argc,argv,&numPistas,&numAviones);

    // Creamos semáforos y memoria compartida
    crear_sem(MUTEXESPERA,1); crear_sem(PISTAS,0);crear_sem(AVIONES,0);
    crear_var(AVIONESESPERA,0);  // No hay llamadas en espera

    // Manejador de Ctrl-C
    instalar_manejador_senhal();
 
    // Crea Tabla para almacenar los pids de los procesos
    iniciar_tabla_procesos(numPistas, numAviones);

    // Tenemos todo
    // Lanzamos los procesos
    crear_procesos(numPistas,numAviones);
  
    // Esperamos a que finalicen los aviones
    esperar_procesos();

    // Matamos las pistas y cualquier otro proceso restante
    terminar_procesos();


    // Finalizamos Manager
    printf("\n[MANAGER] Terminacion del programa (todos los procesos terminados).\n");
    liberar_recursos();

    return EXIT_SUCCESS;
}
//TODO: Realizar todas las funciones necesarias.