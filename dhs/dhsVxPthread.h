/* 30apr99 kdk: 	ETIME definition not needed in VxWorks 5.3 
			but still needed for VxWorks 5.2		*/
#ifndef _PTHREAD_H
#define	_PTHREAD_H

#include <time.h>

#define PTHREAD_CREATE_DETACHED		1
#define PTHREAD_KEYS_MAX		128

#ifndef ETIME
#define	ETIME		ETIMEDOUT
#endif

typedef unsigned int	pthread_t;

typedef unsigned int	pthread_key_t;

typedef unsigned int	pthread_mutex_t;
typedef struct _pthread_mutexattr
{
    void    *pthread_mutexattrp;
} pthread_mutexattr_t;

typedef unsigned int	pthread_cond_t;
typedef struct _pthread_condattr
{
    void    *pthread_condattrp;
} pthread_condattr_t;

typedef struct _pthread_attr
{
    void    *pthread_attrp;
} pthread_attr_t;

extern pthread_t	pthread_self(void);
extern int		pthread_mutex_init(pthread_mutex_t *,
					   const pthread_mutexattr_t *);
extern int		pthread_mutex_lock(pthread_mutex_t *);
extern int		pthread_mutex_trylock(pthread_mutex_t *);
extern int		pthread_mutex_unlock(pthread_mutex_t *);
extern int		pthread_mutex_destroy(pthread_mutex_t *);
extern int		pthread_cond_init(pthread_cond_t *,
					  const pthread_condattr_t *);
extern int		pthread_cond_wait(pthread_cond_t *,
					  pthread_mutex_t *);
extern int		pthread_cond_timedwait(pthread_cond_t *,
					  pthread_mutex_t *, 
					  struct timespec * );
extern int		pthread_cond_broadcast(pthread_cond_t *);
extern int		pthread_attr_init(pthread_attr_t *attr);
extern int		pthread_attr_destroy(pthread_attr_t *attr);
extern int		pthread_attr_setdetachstate(pthread_attr_t *attr,
						    int detachstate);
extern int		pthread_create(pthread_t *thread,
				       const pthread_attr_t *attr,
				       void * (*start_routine)(void *),
				       void *arg);
extern int		pthread_key_create(pthread_key_t *key,
					   void (*destructor)(void *));
extern int		pthread_key_delete(pthread_key_t key);
extern int		pthread_setspecific(pthread_key_t key,
					    const void *value);
extern void		*pthread_getspecific(pthread_key_t key);
extern int		pthread_detach( pthread_t );


#endif	/* _PTHREAD_H */
