

#include "memory.h"

gs_page_alloc_t gameplay_allocator;
gs_page_alloc_t rendering_allocator;
gs_page_alloc_t resources_allocator;
gs_page_alloc_t system_allocator;

/**
 * \brief Initializes the memory subsystem
 */
void
gs_mem_init(void)
{
  gs_page_alloc_init(&gameplay_allocator, 
                      GS_GAMEPLAY_MEMORY,
                      GS_GAMEPLAY_SMALL_PAGE_SIZE, 
                      GS_GAMEPLAY_LARGE_PAGE_SIZE);

  gs_page_alloc_init(&rendering_allocator, 
                      GS_RENDERING_MEMORY,
                      GS_RENDERING_SMALL_PAGE_SIZE, 
                      GS_RENDERING_LARGE_PAGE_SIZE);

  gs_page_alloc_init(&resources_allocator, 
                      GS_RESOURCES_MEMORY,
                      GS_RESOURCES_SMALL_PAGE_SIZE, 
                      GS_RESOURCES_LARGE_PAGE_SIZE);

  gs_page_alloc_init(&system_allocator, 
                      GS_SYSTEM_MEMORY,
                      GS_SYSTEM_SMALL_PAGE_SIZE, 
                      GS_SYSTEM_LARGE_PAGE_SIZE);

}

/**
 * \brief Releases the memory subsystem
 */
void
gs_mem_release(void)
{
  gs_page_alloc_release(&gameplay_allocator);
  gs_page_alloc_release(&rendering_allocator);
  gs_page_alloc_release(&resources_allocator);
  gs_page_alloc_release(&system_allocator);
}
