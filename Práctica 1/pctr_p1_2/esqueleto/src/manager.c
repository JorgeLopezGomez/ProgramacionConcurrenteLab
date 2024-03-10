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
#include <lista.h>

void procesar_argumentos(int argc, char *argv[], char **filename, char **pattern, int *lines);          // Procesar los argumentos pasados al programa
void instalar_manejador_senhal();                                                                       // Instalar el manejador de senhal
void manejador_senhal(int sign);                                                                        // Manejador de senhal
void procesar_patrones(const char *fichero_patrones);                                                   // Procesar los patrones del fichero
void procesar_linea(char *linea);                                                                       // Procesar una linea del fichero de patrones
void iniciar_tabla_procesos(int n_procesos_contador, int n_procesos_procesador);                        // Iniciar la tabla de procesos
void crear_procesos(const char *nombre_fichero);                                                        // Crear los procesos
void lanzar_proceso_contador(const int indice_tabla, const char *linea, const char *numero_linea_str);  // Lanzar un proceso contador
void lanzar_proceso_procesador(const int indice_tabla, const char *patron, const char *nombre_fichero); // Lanzar un proceso procesador
void esperar_procesos();                                                                                // Esperar a que terminen los procesos
void terminar_procesos(void);                                                                           // Terminar los procesos
void liberar_recursos();                                                                                // Liberar los recursos

int g_nProcesses;                   // Numero de procesos
struct TProcess_t *g_process_table; // Tabla de procesos
TLista *patrones;                   // Lista de patrones

// Función principal
int main(int argc, char *argv[])
{
  char *nombre_fichero = NULL, *fichero_patrones = NULL; // Nombres de los ficheros
  int lineas = 0;                                        // Numero de lineas del fichero
  patrones = malloc(sizeof(TLista));                     // Inicializar la lista de patrones

  crear(patrones, "PATRONES");                                                  // Crear la lista de patrones
  procesar_argumentos(argc, argv, &nombre_fichero, &fichero_patrones, &lineas); // Procesar los argumentos pasados al programa
  instalar_manejador_senhal();                                                  // Instalar el manejador de senhal
  procesar_patrones(fichero_patrones);                                          // Procesar los patrones del fichero
  iniciar_tabla_procesos(lineas, longitud(patrones) - 1);                       // Iniciar la tabla de procesos
  crear_procesos(nombre_fichero);                                               // Crear los procesos
  esperar_procesos();                                                           // Esperar a que terminen los procesos

  printf("\n[MANAGER] Terminacion del programa (todos los procesos terminados).\n"); // Mensaje de terminacion
  liberar_recursos();                                                                // Liberar los recursos

  return EXIT_SUCCESS; // Salir con exito
}

// Procesar los argumentos pasados al programa
void procesar_argumentos(int argc, char *argv[], char **nombrefichero, char **fichero_patrones, int *lineas)
{
  FILE *fp; // Puntero al fichero
  int ch;   // Caracter leido

  if (argc != 3) // Comprobar si se pasan los argumentos necesarios para el programa
  {
    fprintf(stderr, "Error. Usa: ./exec/manager <fichero> <fichero_patrones>.\n"); // Mensaje de error
    exit(EXIT_FAILURE);                                                            // Salir con error
  }

  *nombrefichero = argv[1];    // Nombre del fichero
  *fichero_patrones = argv[2]; // Nombre del fichero de patrones

  if ((fp = fopen(*nombrefichero, "r")) == NULL) // Comprobar si se puede abrir el fichero de entrada
  {
    fprintf(stderr, "Error al abrir el fichero %s\n", *nombrefichero); // Mensaje de error
    exit(EXIT_FAILURE);                                                // Salir con error
  }

  while ((ch = fgetc(fp)) != EOF) // Leer el fichero de entrada
  {
    if (ch == '\n') // Si se encuentra un salto de linea
    {
      ++*lineas; // Contar las lineas del fichero
    }
  }

  fclose(fp); // Cerrar el fichero
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
  printf("\n[MANAGER] Terminacion del programa (Ctrl + C).\n"); // Mensaje de terminacion
  terminar_procesos();                                          // Terminar los procesos
  liberar_recursos();                                           // Liberar los recursos
  exit(EXIT_SUCCESS);                                           // Salir con exito
}

// Procesar los patrones del fichero
void procesar_patrones(const char *fichero_patrones)
{
  FILE *fp;             // Puntero al fichero
  char linea[PATH_MAX]; // Linea leida

  if ((fp = fopen(fichero_patrones, "r")) == NULL) // Comprobar si se puede abrir el fichero de patrones
  {
    fprintf(stderr, "Error al abrir el fichero %s\n", fichero_patrones); // Mensaje de error
    exit(EXIT_FAILURE);                                                  // Salir con error
  }

  while (fgets(linea, sizeof(linea), fp) != NULL) // Leer el fichero de patrones
  {
    procesar_linea(linea); // Procesar una linea del fichero de patrones
  }

  fclose(fp); // Cerrar el fichero de patrones
}

// Procesar una linea del fichero de patrones
void procesar_linea(char *linea)
{
  char *token; // Token

  token = strtok(linea, " "); // Obtener el primer token de la linea
  while (token != NULL)       // Mientras haya tokens en la linea
  {
    insertarFinal(patrones, token); // Insertar el token en la lista de patrones
    token = strtok(NULL, " ");      // Obtener el siguiente token de la línea
  }
}

// Iniciar la tabla de procesos
void iniciar_tabla_procesos(int n_procesos_contador, int n_procesos_procesador)
{
  g_nProcesses = n_procesos_contador + n_procesos_procesador;         // Numero de procesos
  g_process_table = malloc(g_nProcesses * sizeof(struct TProcess_t)); // Inicializar la tabla de procesos

  for (int i = 0; i < g_nProcesses; i++) // Inicializar la tabla de procesos
  {
    g_process_table[i].pid = 0; // Inicializar el PID del proceso
  }
}

// Crear los procesos
void crear_procesos(const char *nombre_fichero)
{
  FILE *fp;                                   // Puntero al fichero
  char linea[PATH_MAX], numero_linea_str[11]; // Linea leida y numero de linea como cadena
  int indice_tabla = 0;                       // Indice de la tabla de procesos

  if ((fp = fopen(nombre_fichero, "r")) == NULL) // Comprobar si se puede abrir el fichero de entrada
  {
    fprintf(stderr, "Error al abrir el fichero %s\n", nombre_fichero); // Mensaje de error
    exit(EXIT_FAILURE);                                                // Salir con error
  }

  while (fgets(linea, sizeof(linea), fp) != NULL) // Leer el fichero de entrada
  {
    sprintf(numero_linea_str, "%d", indice_tabla);                  // Convertir el numero de linea a cadena
    lanzar_proceso_contador(indice_tabla, linea, numero_linea_str); // Lanzar un proceso contador

    indice_tabla++; // Incrementar el indice de la tabla de procesos
  }

  for (int i = 2; i <= longitud(patrones); i++) // Recorrer la lista de patrones
  {
    lanzar_proceso_procesador(indice_tabla, getElementoN(patrones, i), nombre_fichero); // Lanzar un proceso procesador
    indice_tabla++;                                                                     // Incrementar el indice de la tabla de procesos
  }

  printf("[MANAGER] %d procesos creados.\n", indice_tabla); // Mensaje de creacion de procesos
  sleep(1);                                                 // Esperar un segundo

  fclose(fp); // Cerrar el fichero de entrada
}

// Lanzar un proceso contador
void lanzar_proceso_contador(const int indice_tabla, const char *linea, const char *numero_linea_str)
{
  pid_t pid; // PID del proceso

  switch (pid = fork()) // Crear un proceso hijo
  {
  case -1:                                                                                 // Error
    fprintf(stderr, "[MANAGER] Error al lanzar proceso contador: %s.\n", strerror(errno)); // Mensaje de error
    terminar_procesos();                                                                   // Terminar los procesos
    liberar_recursos();                                                                    // Liberar los recursos
    exit(EXIT_FAILURE);                                                                    // Salir con error
  case 0:                                                                                  // Proceso hijo
    if (execl(RUTA_CONTADOR, CLASE_CONTADOR, linea, numero_linea_str, NULL) == -1)         // Ejecutar el proceso contador
    {
      fprintf(stderr, "[MANAGER] Error usando execl() en el poceso %s: %s.\n", CLASE_CONTADOR, strerror(errno)); // Mensaje de error
      exit(EXIT_FAILURE);                                                                                        // Salir con error
    }
  }

  g_process_table[indice_tabla].pid = pid;              // Asignar el PID del proceso a la tabla de procesos
  g_process_table[indice_tabla].clase = CLASE_CONTADOR; // Asignar la clase del proceso a la tabla de procesos
}

// Lanzar un proceso procesador
void lanzar_proceso_procesador(const int indice_tabla, const char *patron, const char *nombre_fichero)
{
  pid_t pid; // PID del proceso

  switch (pid = fork()) // Crear un proceso hijo
  {
  case -1:                                                                                   // Error
    fprintf(stderr, "[MANAGER] Error al lanzar proceso procesador: %s.\n", strerror(errno)); // Mensaje de error
    terminar_procesos();                                                                     // Terminar los procesos
    liberar_recursos();                                                                      // Liberar los recursos
    exit(EXIT_FAILURE);                                                                      // Salir con error
  case 0:                                                                                    // Proceso hijo
    if (execl(RUTA_PROCESADOR, CLASE_PROCESADOR, nombre_fichero, patron, NULL) == -1)        // Ejecutar el proceso procesador
    {
      fprintf(stderr, "[MANAGER] Error usando execl() en el proceso %s: %s.\n", CLASE_PROCESADOR, strerror(errno)); // Mensaje de error
      exit(EXIT_FAILURE);                                                                                           // Salir con error
    }
  }

  g_process_table[indice_tabla].pid = pid;                // Asignar el PID del proceso a la tabla de procesos
  g_process_table[indice_tabla].clase = CLASE_PROCESADOR; // Asignar la clase del proceso a la tabla de procesos
}

// Esperar a que terminen los procesos
void esperar_procesos()
{
  int i, n_processes = g_nProcesses; // Numero de procesos
  pid_t pid;                         // PID del proceso

  while (n_processes > 0) // Mientras haya procesos en ejecucion
  {
    pid = wait(NULL);                  // Esperar a que termine un proceso
    for (i = 0; i < g_nProcesses; i++) // Recorrer la tabla de procesos
    {
      if (pid == g_process_table[i].pid) // Si el PID del proceso coincide con el PID de la tabla de procesos
      {
        printf("[MANAGER] Proceso %s terminado [%d]...\n", g_process_table[i].clase, g_process_table[i].pid); // Mensaje de terminacion
        g_process_table[i].pid = 0;                                                                           // Reiniciar el PID del proceso
        n_processes--;                                                                                        // Decrementar el numero de procesos
        break;                                                                                                // Salir del bucle
      }
    }
  }
}

void liberar_recursos() // Liberar los recursos
{
  free(g_process_table); // Liberar la tabla de procesos
  destruir(patrones);    // Liberar la lista de patrones
}

void terminar_procesos(void) // Terminar los procesos
{
  int i; // Indice de la tabla de procesos

  printf("\n----- [MANAGER] Terminar con los procesos hijos ejecutándose ----- \n"); // Mensaje de terminacion
  for (i = 0; i < g_nProcesses; i++)                                                 // Recorrer la tabla de procesos
  {
    if (g_process_table[i].pid != 0) // Si el PID del proceso no es 0
    {
      printf("[MANAGER] Terminando proceso %s [%d]...\n", g_process_table[i].clase, g_process_table[i].pid); // Mensaje de terminacion
      if (kill(g_process_table[i].pid, SIGINT) == -1)                                                        // Enviar la senhal SIGINT al proceso
      {
        fprintf(stderr, "[MANAGER] Error al usar kill() en proceso %d: %s.\n", g_process_table[i].pid, strerror(errno)); // Mensaje de error
      }
    }
  }
}