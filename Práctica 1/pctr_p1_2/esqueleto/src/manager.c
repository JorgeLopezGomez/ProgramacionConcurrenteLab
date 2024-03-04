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

void procesar_argumentos(int argc, char *argv[], char **filename, char **pattern, int *lines);
void instalar_manejador_senhal();
void manejador_senhal(int sign);
void procesar_patrones(const char *fichero_patrones);
void procesar_linea(char *linea);
void iniciar_tabla_procesos(int n_procesos_contador, int n_procesos_procesador);
void crear_procesos(const char *nombre_fichero);
void lanzar_proceso_contador(const int indice_tabla, const char *linea, const char *numero_linea_str);
void lanzar_proceso_procesador(const int indice_tabla, const char *patron, const char *nombre_fichero);
void esperar_procesos();
void terminar_procesos(void);
void liberar_recursos();

int g_nProcesses;
struct TProcess_t *g_process_table;
TLista *patrones;

int main(int argc, char *argv[])
{
  char *nombre_fichero = NULL, *fichero_patrones = NULL;
  int lineas = 0;
  patrones = malloc(sizeof(TLista));

  crear(patrones, "PATRONES");
  procesar_argumentos(argc, argv, &nombre_fichero, &fichero_patrones, &lineas);
  instalar_manejador_senhal();
  procesar_patrones(fichero_patrones);
  iniciar_tabla_procesos(lineas, longitud(patrones) - 1);
  crear_procesos(nombre_fichero);
  esperar_procesos();

  printf("\n[MANAGER] Terminacion del programa (todos los procesos terminados).\n");
  liberar_recursos();

  return EXIT_SUCCESS;
}

void procesar_argumentos(int argc, char *argv[], char **nombrefichero, char **fichero_patrones, int *lineas)
{
  FILE *fp;
  int ch;

  if (argc != 3)
  {
    fprintf(stderr, "Error. Usa: ./exec/manager <fichero> <fichero_patrones>.\n");
    exit(EXIT_FAILURE);
  }

  *nombrefichero = argv[1];
  *fichero_patrones = argv[2];

  if ((fp = fopen(*nombrefichero, "r")) == NULL)
  {
    fprintf(stderr, "Error al abrir el fichero %s\n", *nombrefichero);
    exit(EXIT_FAILURE);
  }

  while ((ch = fgetc(fp)) != EOF)
  {
    if (ch == '\n')
    {
      ++*lineas;
    }
  }

  fclose(fp);
}

void instalar_manejador_senhal()
{
  if (signal(SIGINT, manejador_senhal) == SIG_ERR)
  {
    fprintf(stderr, "[MANAGER] Error al instalar el manejador se senhal: %s.\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
}

void manejador_senhal(int sign)
{
  printf("\n[MANAGER] Terminacion del programa (Ctrl + C).\n");
  terminar_procesos();
  liberar_recursos();
  exit(EXIT_SUCCESS);
}

void procesar_patrones(const char *fichero_patrones)
{
  FILE *fp;
  char linea[PATH_MAX];

  if ((fp = fopen(fichero_patrones, "r")) == NULL)
  {
    fprintf(stderr, "Error al abrir el fichero %s\n", fichero_patrones);
    exit(EXIT_FAILURE);
  }

  while (fgets(linea, sizeof(linea), fp) != NULL)
  {
    procesar_linea(linea);
  }

  fclose(fp);
}

void procesar_linea(char *linea)
{
  char *token;

  token = strtok(linea, " ");
  while (token != NULL)
  {
    insertarFinal(patrones, token);
    token = strtok(NULL, " ");
  }
}

void iniciar_tabla_procesos(int n_procesos_contador, int n_procesos_procesador)
{
  g_nProcesses = n_procesos_contador + n_procesos_procesador;
  g_process_table = malloc(g_nProcesses * sizeof(struct TProcess_t));

  for (int i = 0; i < g_nProcesses; i++)
  {
    g_process_table[i].pid = 0;
  }
}

void crear_procesos(const char *nombre_fichero)
{
  FILE *fp;
  char linea[PATH_MAX], numero_linea_str[3];
  int indice_tabla = 0;
  
  if ((fp = fopen(nombre_fichero, "r")) == NULL)
  {
    fprintf(stderr, "Error al abrir el fichero %s\n", nombre_fichero);
    exit(EXIT_FAILURE);
  }

  while (fgets(linea, sizeof(linea), fp) != NULL)
  {
    sprintf(numero_linea_str, "%d", indice_tabla);
    lanzar_proceso_contador(indice_tabla, linea, numero_linea_str);

    indice_tabla++;
  }

  for (int i = 2; i <= longitud(patrones); i++)
  {
    lanzar_proceso_procesador(indice_tabla, getElementoN(patrones, i), nombre_fichero);
    indice_tabla++;
  }

  printf("[MANAGER] %d procesos creados.\n", indice_tabla);
  sleep(1);

  fclose(fp);
}

void lanzar_proceso_contador(const int indice_tabla, const char *linea, const char *numero_linea_str)
{
  pid_t pid;

  switch (pid = fork())
  {
  case -1:
    fprintf(stderr, "[MANAGER] Error al lanzar proceso contador: %s.\n", strerror(errno));
    terminar_procesos();
    liberar_recursos();
    exit(EXIT_FAILURE);
  case 0:
    if (execl(RUTA_CONTADOR, CLASE_CONTADOR, linea, numero_linea_str, NULL) == -1)
    {
      fprintf(stderr, "[MANAGER] Error usando execl() en el poceso %s: %s.\n", CLASE_CONTADOR, strerror(errno));
      exit(EXIT_FAILURE);
    }
  }

  g_process_table[indice_tabla].pid = pid;
  g_process_table[indice_tabla].clase = CLASE_CONTADOR;
}

void lanzar_proceso_procesador(const int indice_tabla, const char *patron, const char *nombre_fichero)
{
  pid_t pid;

  switch (pid = fork())
  {
  case -1:
    fprintf(stderr, "[MANAGER] Error al lanzar proceso procesador: %s.\n", strerror(errno));
    terminar_procesos();
    liberar_recursos();
    exit(EXIT_FAILURE);
  case 0:
    if (execl(RUTA_PROCESADOR, CLASE_PROCESADOR, nombre_fichero, patron, NULL) == -1)
    {
      fprintf(stderr, "[MANAGER] Error usando execl() en el proceso %s: %s.\n", CLASE_PROCESADOR, strerror(errno));
      exit(EXIT_FAILURE);
    }
  }

  g_process_table[indice_tabla].pid = pid;
  g_process_table[indice_tabla].clase = CLASE_PROCESADOR;
}

void esperar_procesos()
{
  int i, n_processes = g_nProcesses;
  pid_t pid;

  while (n_processes > 0)
  {
    pid = wait(NULL);
    for (i = 0; i < g_nProcesses; i++)
    {
      if (pid == g_process_table[i].pid)
      {
        printf("[MANAGER] Proceso %s terminado [%d]...\n", g_process_table[i].clase, g_process_table[i].pid);
        g_process_table[i].pid = 0;
        n_processes--;
        break;
      }
    }
  }
}

void liberar_recursos()
{
  free(g_process_table);
  destruir(patrones);
}

void terminar_procesos(void)
{
  int i;

  printf("\n----- [MANAGER] Terminar con los procesos hijos ejecutándose ----- \n");
  for (i = 0; i < g_nProcesses; i++)
  {
    if (g_process_table[i].pid != 0)
    {
      printf("[MANAGER] Terminando proceso %s [%d]...\n", g_process_table[i].clase, g_process_table[i].pid);
      if (kill(g_process_table[i].pid, SIGINT) == -1)
      {
        fprintf(stderr, "[MANAGER] Error al usar kill() en proceso %d: %s.\n", g_process_table[i].pid, strerror(errno));
      }
    }
  }
}
