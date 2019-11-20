#ifndef ABSL_CONTAINER_INTERNAL_ESP8266_MUTEX_H_
#define ABSL_CONTAINER_INTERNAL_ESP8266_MUTEX_H_

#include <errno.h>

typedef unsigned long pthread_mutex_t;
typedef unsigned long pthread_cond_t;
typedef unsigned long pthread_key_t;

typedef struct {
  int type;
} pthread_mutexattr_t;

typedef struct {
  int   is_initialized;
} pthread_condattr_t; 

int pthread_mutex_init(pthread_mutex_t *__mutex, _CONST pthread_mutexattr_t *__attr) { return 1; }
int pthread_mutex_lock(pthread_mutex_t *__mutex) { return 1; }
int pthread_mutex_unlock(pthread_mutex_t *__mutex) { return 1; }
int pthread_mutex_trylock(pthread_mutex_t *__mutex) { return 1; }
int pthread_mutex_destroy(pthread_mutex_t *__mutex) { return 1; }
int pthread_cond_init(pthread_cond_t *__cond, _CONST pthread_condattr_t *__attr) { return 1; }
int pthread_cond_destroy(pthread_cond_t *__mutex) { return 1; }
int pthread_cond_signal(pthread_cond_t *__mutex) { return 1; }
int pthread_cond_broadcast(pthread_cond_t *__mutex) { return 1; }
int pthread_cond_wait(pthread_cond_t *__cond, pthread_mutex_t *__mutex) { return 1; }
int pthread_cond_timedwait(pthread_cond_t *__cond, pthread_mutex_t *__mutex, _CONST struct timespec *__abstime)  { return 1; }

#endif