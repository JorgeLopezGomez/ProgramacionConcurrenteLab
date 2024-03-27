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
  procesar_argumentos(argc, argv, &numPistas, &numAviones);

  // Creamos semáforos y memoria compartida
  crear_sem(MUTEXESPERA, 1);
  crear_sem(PISTAS, 0);
  crear_sem(AVIONES, 0);
  crear_var(AVIONESESPERA, 0); // No hay llamadas en espera

  // Manejador de Ctrl-C
  instalar_manejador_senhal();

  // Crea Tabla para almacenar los pids de los procesos
  iniciar_tabla_procesos(numPistas, numAviones);

  // Tenemos todo
  // Lanzamos los procesos
  crear_procesos(numPistas, numAviones);

  // Esperamos a que finalicen los aviones
  esperar_procesos();

  // Matamos las pistas y cualquier otro proceso restante
  terminar_procesos();

  // Finalizamos Manager
  printf("\n[MANAGER] Terminacion del programa (todos los procesos terminados).\n");
  liberar_recursos();

  return EXIT_SUCCESS;
}
// TODO: Realizar todas las funciones necesarias.
void procesar_argumentos(int argc, char *argv[], int *numPistas, int *numAviones)
{
  if (argc != 3) // Comprobar si se pasan los argumentos necesarios para el programa
  {
    fprintf(stderr, "Error. Usa: ./exec/manager <numPistas> <numAviones>.\n"); // Mensaje de error
    exit(EXIT_FAILURE);                                                        // Salir con error
  }

  *numPistas = atoi(argv[1]);  // Convertir el primer argumento a un entero
  *numAviones = atoi(argv[2]); // Convertir el segundo argumento a un entero

  if (*numPistas <= 0 || *numAviones <= 0) // Comprobar si los argumentos son válidos
  {
    fprintf(stderr, "Error. numPistas y numAviones deben ser mayores que 0.\n"); // Mensaje de error
    exit(EXIT_FAILURE);                                                          // Salir con error
  }
}

// Instalar el manejador de senhal
void instalar_manejador_senhal()
{
  if (signal(SIGINT, manejador_senhal) == SIG_ERR) // Instalar el manejador de senhal para Ctrl + C
  {
    fprintf(stderr, "[MANAGER] Error al instalar el manejador se senhal: %s.\n", strerror(errno)); // Mensaje de error
    exit(EXIT_FAILURE);                                                                            // Salir con error
  }
}

// Manejador de senhal
void manejador_senhal(int sign)
{
  printf("\n[MANAGER] Terminacion del programa (Ctrl + C).\n");                      // Mensaje de terminacion
  terminar_procesos();                                                               // Terminar los procesos
  liberar_recursos();                                                                // Liberar los recursos
  printf("\n[MANAGER] Terminacion del programa (todos los procesos terminados).\n"); // Mensaje de terminacion
  exit(EXIT_SUCCESS);                                                                // Salir con exito
}

// Iniciar la tabla de procesos
void iniciar_tabla_procesos(int n_procesos_pista, int n_procesos_avion)
{
  g_pistasProcesses = n_procesos_pista;  // Numero de procesos de pista
  g_avionesProcesses = n_procesos_avion; // Numero de procesos de avion

  // Inicializar la tabla de procesos de pista
  g_process_pistas_table = malloc(g_pistasProcesses * sizeof(struct TProcess_t));
  for (int i = 0; i < g_pistasProcesses; i++) // Inicializar la tabla de procesos de pista
  {
    g_process_pistas_table[i].pid = 0; // Inicializar el PID del proceso
  }

  // Inicializar la tabla de procesos de avion
  g_process_aviones_table = malloc(g_avionesProcesses * sizeof(struct TProcess_t));
  for (int i = 0; i < g_avionesProcesses; i++) // Inicializar la tabla de procesos de avion
  {
    g_process_aviones_table[i].pid = 0; // Inicializar el PID del proceso
  }
}

// Crear los procesos
void crear_procesos(int numPistas, int numAviones)
{
  g_pistasProcesses = numPistas;
  g_avionesProcesses = numAviones;

  // Crear procesos de pistas
  for (int i = 0; i < g_pistasProcesses; i++)
  {
    lanzar_proceso_pista(i); // Lanzar un proceso de pista
  }
  printf("[MANAGER] %d pistas creadas.\n", g_pistasProcesses); // Mensaje de creacion de procesos de pistas

  // Crear procesos de aviones
  for (int i = 0; i < g_avionesProcesses; i++)
  {
    lanzar_proceso_avion(i); // Lanzar un proceso de avion
  }
  printf("[MANAGER] %d aviones creados.\n", g_avionesProcesses); // Mensaje de creacion de procesos de aviones

  sleep(1); // Esperar un segundo
}

// Lanzar un proceso de pista
void lanzar_proceso_pista(const int indice_tabla)
{
  pid_t pid; // PID del proceso

  switch (pid = fork()) // Crear un proceso hijo
  {
  case -1:                                                                              // Error
    fprintf(stderr, "[MANAGER] Error al lanzar proceso pista: %s.\n", strerror(errno)); // Mensaje de error
    terminar_procesos();                                                                // Terminar los procesos
    liberar_recursos();                                                                 // Liberar los recursos
    exit(EXIT_FAILURE);                                                                 // Salir con error
  case 0:                                                                               // Proceso hijo
    if (execl(RUTA_PISTA, CLASE_PISTA, NULL) == -1)                                     // Ejecutar el proceso pista
    {
      fprintf(stderr, "[MANAGER] Error usando execl() en el proceso %s: %s.\n", CLASE_PISTA, strerror(errno)); // Mensaje de error
      exit(EXIT_FAILURE);                                                                                      // Salir con error
    }
  }

  g_process_pistas_table[indice_tabla].pid = pid;           // Asignar el PID del proceso a la tabla de procesos
  g_process_pistas_table[indice_tabla].clase = CLASE_PISTA; // Asignar la clase del proceso a la tabla de procesos
}

// Lanzar un proceso de avion
void lanzar_proceso_avion(const int indice_tabla)
{
  pid_t pid; // PID del proceso

  switch (pid = fork()) // Crear un proceso hijo
  {
  case -1:                                                                              // Error
    fprintf(stderr, "[MANAGER] Error al lanzar proceso avion: %s.\n", strerror(errno)); // Mensaje de error
    terminar_procesos();                                                                // Terminar los procesos
    liberar_recursos();                                                                 // Liberar los recursos
    exit(EXIT_FAILURE);                                                                 // Salir con error
  case 0:                                                                               // Proceso hijo
    if (execl(RUTA_AVION, CLASE_AVION, NULL) == -1)                                     // Ejecutar el proceso avion
    {
      fprintf(stderr, "[MANAGER] Error usando execl() en el proceso %s: %s.\n", CLASE_AVION, strerror(errno)); // Mensaje de error
      exit(EXIT_FAILURE);                                                                                      // Salir con error
    }
  }

  g_process_aviones_table[indice_tabla].pid = pid;           // Asignar el PID del proceso a la tabla de procesos
  g_process_aviones_table[indice_tabla].clase = CLASE_AVION; // Asignar la clase del proceso a la tabla de procesos
}

// Esperar a que terminen los procesos
void esperar_procesos()
{
  int i;
  int nProcesos = g_avionesProcesses; // Cambiado a aviones
  pid_t pid; // PID del proceso

  while (nProcesos > 0) // Mientras haya procesos en ejecucion
  {
    pid = wait(NULL); // Esperar a que termine un proceso
    nProcesos--; // Decrementar el numero de procesos

    for (i = 0; i < g_avionesProcesses; i++) // Recorrer la tabla de procesos de aviones
    {
      if (pid == g_process_aviones_table[i].pid) // Si el PID del proceso coincide con el PID de la tabla de procesos
      {
        printf("[MANAGER] Proceso %s terminado [%d]...\n", g_process_aviones_table[i].clase, g_process_aviones_table[i].pid); // Mensaje de terminacion
        g_process_aviones_table[i].pid = 0;                                                                                  // Reiniciar el PID del proceso
        break;                                                                                                               // Salir del bucle
      }
    }
  }
}

// Terminar los procesos
void terminar_procesos(void)
{
  printf("\n----- [MANAGER] Terminar con cualquier proceso pendiente ejecutándose -----\n"); // Mensaje de terminacion

  terminar_procesos_especificos(g_process_aviones_table, g_avionesProcesses); // Terminar los procesos de aviones
  terminar_procesos_especificos(g_process_pistas_table, g_pistasProcesses);   // Terminar los procesos de pistas

  printf("\nTodos los aviones en tierra y pistas cerradas\n"); // Mensaje de terminacion
}

// Terminar procesos específicos
void terminar_procesos_especificos(struct TProcess_t *process_table, int process_num)
{
  int i; // Indice de la tabla de procesos

  for (i = 0; i < process_num; i++) // Recorrer la tabla de procesos
  {
    if (process_table[i].pid != 0) // Si el PID del proceso no es 0
    {
      printf("[MANAGER] Terminando proceso %s [%d]...\n", process_table[i].clase, process_table[i].pid); // Mensaje de terminacion
      if (kill(process_table[i].pid, SIGINT) == -1)                                                      // Enviar la señal SIGINT al proceso
      {
        fprintf(stderr, "[MANAGER] Error al usar kill() en proceso %d: %s.\n", process_table[i].pid, strerror(errno)); // Mensaje de error
      }
    }
  }
}

// Liberar los recursos
void liberar_recursos()
{
  free(g_process_pistas_table);  // Liberar la tabla de procesos de pistas
  free(g_process_aviones_table); // Liberar la tabla de procesos de aviones
}