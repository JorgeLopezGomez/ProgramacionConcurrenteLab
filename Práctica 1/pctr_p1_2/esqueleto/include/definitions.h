#define CLASE_PROCESADOR "PROCESADOR"
#define RUTA_PROCESADOR "./exec/procesador"
#define CLASE_CONTADOR "CONTADOR"
#define RUTA_CONTADOR "./exec/contador"

struct TProcess_t
{
  pid_t pid;
  char *clase;
};
