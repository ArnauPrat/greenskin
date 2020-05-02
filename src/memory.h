
#ifndef _GS_MEMORY_H_
#define _GS_MEMORY_H_

#include "platform.h"
#include <furious/common/memory/memory.h>
#include <furious/common/memory/pool_allocator.h>
#include <furious/common/memory/stack_allocator.h>
#include <furious/common/memory/page_allocator.h>

#define GS_NO_HINT FDB_NO_HINT

typedef void* (*gs_alloc_t) (void* state, int32_t size, int32_t alignment);
typedef void (*gs_free_t) (void* state, void* ptr);

typedef fdb_mem_allocator_t gs_mem_allocator_t;
#define gs_alloc mem_alloc
#define gs_free mem_free
#define gs_get_global_allocator fdb_get_global_mem_allocator

typedef fdb_pool_alloc_t gs_pool_alloc_t;
#define gs_pool_alloc_alloc fdb_pool_alloc_alloc
#define gs_pool_alloc_free fdb_pool_alloc_free
#define gs_pool_alloc_init fdb_pool_alloc_init
#define gs_pool_alloc_release fdb_pool_alloc_release
#define gs_pool_alloc_flush fdb_pool_alloc_flush

typedef fdb_stack_alloc_t gs_stack_alloc_t;
#define gs_stack_alloc_alloc fdb_stack_alloc_alloc
#define gs_stack_alloc_free fdb_stack_alloc_free
#define gs_stack_alloc_pop fdb_stack_alloc_pop
#define gs_stack_alloc_init fdb_stack_alloc_init
#define gs_stack_alloc_release fdb_stack_alloc_release

typedef fdb_page_alloc_t gs_page_alloc_t;
#define gs_page_alloc_alloc fdb_page_alloc_alloc
#define gs_page_alloc_free fdb_page_alloc_free
#define gs_page_alloc_init fdb_page_alloc_init
#define gs_page_alloc_release fdb_page_alloc_release

extern gs_page_alloc_t gameplay_allocator;  
extern gs_page_alloc_t rendering_allocator;  
extern gs_page_alloc_t resources_allocator;  
extern gs_page_alloc_t system_allocator;

/**
 * \brief Initializes the memory subsystem
 */
void
gs_mem_init(void);

/**
 * \brief Releases the memory subsystem
 */
void
gs_mem_release(void);


#endif /* ifndef _GS_MEMORY_H_ */

