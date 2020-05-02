
#ifndef _GS_MUTEX_H_
#define _GS_MUTEX_H_

#include <furious/common/mutex.h>

typedef fdb_mutex_t gs_mutex_t;
#define gs_mutex_init fdb_mutex_init
#define gs_mutex_release fdb_mutex_release
#define gs_mutex_lock fdb_mutex_lock
#define gs_mutex_unlock fdb_mutex_unlock


#endif /* ifndef _GS_MUTEX_H_*/
