#include <pthread.h>

pthread_mutex_t RESYNC_LOCK = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t BACKUP_TRANSITION_LOCK = PTHREAD_MUTEX_INITIALIZER;