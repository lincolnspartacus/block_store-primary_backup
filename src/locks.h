#include <pthread.h>

/*
 * Lock for stalling client requests at the primary 
 * during Resync process */
extern pthread_mutex_t RESYNC_LOCK;