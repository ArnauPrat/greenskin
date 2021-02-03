
#ifndef _GS_PLATFORM_H_
#define _GS_PLATFORM_H_


#ifdef GS_COMPILER_GCC
  #define GS_COMPILER_DEFINED
#endif

#ifdef GS_OS_LINUX
  #define GS_OS_DEFINED
  #define _GNU_SOURCE
#endif

#ifndef GS_COMPILER_DEFINED
  #error compiler not defined
#endif

#ifndef GS_COMPILER_DEFINED
  #error os not defined
#endif

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef GS_ENABLE_ASSERTS
#define GS_ASSERT(_cond) if(!(_cond)) abort();
#else
#define GS_ASSERT(_cond)
#endif

#define GS_PERMA_ASSERT(_cond) if(!(_cond)) { \
    printf(#_cond"\n");\
    abort();\
  }

#define GS_CONF_MAX_LINE_LEN              512
#define GS_CONF_MAX_OPTION_KEY_LEN        256
#define GS_CONF_MAX_OPTION_VALUE_LEN      256
#define GS_CONF_MAX_VKVL_NAME_LEN         GS_CONF_MAX_OPTION_VALUE_LEN
#define GS_CONF_MAX_VKVL_NUM              16
#define GS_CONF_MAX_RDIR_NUM              32
#define GS_CONF_MAX_RDIR_LEN              128

#define GS_GAMEPLAY_MEMORY                MEGABYTES(128)
#define GS_GAMEPLAY_SMALL_PAGE_SIZE       KILOBYTES(4)
#define GS_GAMEPLAY_LARGE_PAGE_SIZE       KILOBYTES(64)

#define GS_RENDERING_MEMORY               MEGABYTES(128)
#define GS_RENDERING_SMALL_PAGE_SIZE      KILOBYTES(4)
#define GS_RENDERING_LARGE_PAGE_SIZE      KILOBYTES(64)
#define GS_RENDERING_VMEM_BFF_HOST        MEGABYTES(32)
#define GS_RENDERING_VMEM_IMG_DEVICE      MEGABYTES(192)
#define GS_RENDERING_VMEM_BFF_DEVICE      MEGABYTES(64)

#define GS_RESOURCES_MEMORY               MEGABYTES(256)
#define GS_RESOURCES_SMALL_PAGE_SIZE      KILOBYTES(4)
#define GS_RESOURCES_LARGE_PAGE_SIZE      KILOBYTES(64)
#define GS_RESOURCES_MAX_RDIR_PATH_LEN    GS_CONF_MAX_RDIR_LEN
#define GS_RESOURCES_MAX_FILE_LEN         GS_RESOURCES_LARGE_PAGE_SIZE 
#define GS_RESOURCES_MAX_FILE_PATH_LEN    256 

#define GS_SYSTEM_MEMORY                  MEGABYTES(64)
#define GS_SYSTEM_SMALL_PAGE_SIZE         KILOBYTES(4)
#define GS_SYSTEM_LARGE_PAGE_SIZE         KILOBYTES(64)

#define GS_TRACE_MAX_INFO_LEN             1024
#define GS_TRACE_MAX_NAME_LEN             128

#define GS_TASKING_MAX_INFO_LEN           GS_TRACE_MAX_INFO_LEN 
#define GS_TASKING_MAX_NAME_LEN           GS_TRACE_MAX_NAME_LEN 
#define GS_TASKING_STACK_SIZE             16384
#define GS_TASKING_MAX_TASK_CONTEXTS      1024
#define GS_TASKING_MAX_EXEC_CONTEXTS      GS_TASKING_MAX_TASK_CONTEXTS

#define GS_INPUT_MAX_CALLBACKS            16

#ifdef GS_OS_LINUX
  #define GS_THREAD __thread
#endif


/**
 * \brief Gets the operating system's page size
 *
 * \return  The operating system's page size
 */
uint32_t
gs_os_page_size();

/**
 * \brief Protects the given memory region
 *
 * \param ptr The pointer to the beginning of the memory region
 * \param size The size of the memory region
 *
 * \return Returns true if the protection was successful. False otherwise
 */
bool
gs_os_protect_mem(void* ptr, uint32_t size);

/**
 * \brief Removes the protection of a memory region
 *
 * \param ptr The pointer to the memory to protect
 * \param size The size of the memory region to protect
 *
 * \return Returns true if the operation was successful. False otherwise
 */
bool
gs_os_unprotect_mem(void* ptr, uint32_t size);

/**
 * \brief Gets the minimum alignment for all types of allocations
 *
 * \return The minimum alignment
 */
uint32_t
gs_os_min_alignment();


#endif 
