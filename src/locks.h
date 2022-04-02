#include <pthread.h>

/*
 * Lock for stalling client requests at the primary 
 * during Resync process */
extern pthread_rwlock_t RESYNC_LOCK;

/*
 * Lock for atomically transitioning our Backup Server
 * from STATE_BACKUP to STATE_PRIMARY. All GetState() RPCs
 * sent to this server will stall until the transition
 * is complete */
extern pthread_mutex_t BACKUP_TRANSITION_LOCK;