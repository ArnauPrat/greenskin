
#include "resources.h"
#include "log.h"
#include <string.h>

static char m_resource_dirs[GS_CONF_MAX_RDIR_NUM][GS_CONF_MAX_RDIR_LEN];
static uint32_t     m_next_free;

void
gs_resources_init(void)
{
  m_next_free = 0;
}

void
gs_resources_release(void)
{
}

void
gs_resources_add_rdir(const char* rdir)
{
  uint32_t len = strlen(rdir);
  if(len >= GS_RESOURCES_MAX_RDIR_PATH_LEN)
  {
    GS_LOG_ERROR(E_RESOURCES_RDIR_PATH_NAME_ERROR, "The resource folder path length is too long");
  }
  memcpy(m_resource_dirs[m_next_free], rdir, sizeof(char)*(len+1));
  m_next_free++;
}

bool
gs_resources_find_file(const char* filename, 
                       char* buffer, 
                       uint32_t blength)
{
  for(uint32_t i = 0; i < m_next_free; ++i) 
  {
    uint32_t written = snprintf(buffer,
                               blength,
                               "%s/%s",
                               m_resource_dirs[i],
                               filename);
    if(written >= GS_RESOURCES_MAX_FILE_PATH_LEN)
    {
      GS_LOG_ERROR(E_RESOURCES_FILE_PATH_NAME_ERROR,"The file path lenght is to long");
    }
    FILE* fd = fopen(buffer,"r");
    if (fd != NULL) 
    { 
      fclose(fd);
      return true; 
    }
  } 
  return false;
}

uint32_t
gs_resources_read_file(const char* filename, 
             char* buffer, 
             uint32_t bsize) 
{
  FILE*  file = fopen(filename, "rb");

  if (!file) 
  {
    GS_LOG_ERROR(E_IO_FILE_NOT_FOUND, 
                 "failed to open file %s",filename);
  }

  fseek(file, 0, SEEK_END);
  size_t file_size = (size_t) ftell(file);
  GS_ASSERT(bsize >= file_size && "File buffer is not large enough to read the file");
  fseek(file, 0, SEEK_SET);
  fread(buffer, sizeof(char), file_size, file);
  fclose(file);
  return file_size;
}
