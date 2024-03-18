// Semáforos
#define MUTEXESPERA "mutexesp"
#define PISTAS "contadorpistas"
#define AVIONES "contadoraviones"

//Memoria Compartida
#define AVIONESESPERA "avionesespera"

// CLASES y PATHS
#define CLASE_PISTA "PISTA"
#define RUTA_PISTA "./exec/pista"
#define CLASE_AVION "AVION"
#define RUTA_AVION "./exec/avion"

struct TProcess_t
{
  pid_t pid;
  char *clase;
};
