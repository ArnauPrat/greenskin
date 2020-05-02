

#ifndef _GS_BTREE_H_
#define _GS_BTREE_H_

#include <furious/common/btree.h>

#define GS_BTREE_ALIGNMENT FDB_BTREE_ALIGNMENT
#define GS_BTREE_PAGE_SIZE FDB_BTREE_PAGE_SIZE

typedef fdb_btree_t gs_btree_t;
typedef fdb_btree_entry_t gs_btree_entry_t;
typedef fdb_btree_insert_t gs_btree_insert_t;
typedef fdb_btree_iter_t gs_btree_iter_t;
#define gs_btree_init fdb_btree_init
#define gs_btree_release fdb_btree_release
#define gs_btree_get fdb_btree_get
#define gs_btree_insert fdb_btree_insert
#define gs_btree_remove fdb_btree_remove
#define gs_btree_clear fdb_btree_clear

#define gs_btree_iter_init fdb_btree_iter_init
#define gs_btree_iter_release fdb_btree_iter_release
#define gs_btree_iter_has_next fdb_btree_iter_has_next
#define gs_btree_iter_next fdb_btree_iter_next

#endif
