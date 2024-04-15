
// CLASES y PATHS
#define CLASE_PISTA "PISTA"
#define RUTA_PISTA "./exec/pista"
#define CLASE_SLOT "SLOT"
#define RUTA_SLOT "./exec/slot"

// Dispositivos
#define NUMSLOTS 10    // /proc/sys/fs/mqueue/msg_max
#define NUMPISTAS 3

// Colas Mensajes
#define BUZON_ATERRIZAJES "/buzon_aterrizajes"
#define BUZON_SLOTS   "/buzon_slot_"
#define TAMANO_MENSAJES 64

// Slots
#define FIN_ATERRIZAJE "Aterrizado"

// Procesos
struct TProcess_t
{
  pid_t pid;
  char *clase;
};
