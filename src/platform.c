

#include "platform.h"

#ifdef GS_OS_LINUX
#include <unistd.h>
#include <sys/mman.h>
#endif

uint32_t
gs_os_page_size()
{
  return sysconf(_SC_PAGESIZE);
}

bool
gs_os_protect_mem(void* ptr, 
                  uint32_t size)
{
#ifdef GS_OS_LINUX
  int result =  mprotect(ptr, 
                         size, 
                         PROT_NONE);
return result == 0;
#endif
}

bool
gs_os_unprotect_mem(void* ptr, uint32_t size)
{
#ifdef GS_OS_LINUX
  int result = mprotect(ptr, 
                        size, 
                        PROT_WRITE | PROT_READ);
  return result == 0;
#endif
}

uint32_t
gs_os_min_alignment()
{
  return 16;
}
