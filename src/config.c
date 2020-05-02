
#include "platform.h"
#include "log.h"
#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>


static int 
fread_line(FILE* fd, char* buffer, size_t buffer_size)
{
  size_t curr = 0;
  int c = getc(fd) ;
  while(c != '\n' && c != EOF)
  {
    if(curr == buffer_size-1)
    {
      GS_LOG_ERROR(E_MEMORY_INSUFFICIENT_BUFFER_SIZE_ERROR, 
                   "Configuration file line is too long");
    }
    buffer[curr] = c;
    curr++;
    c = getc(fd);
  }
  buffer[curr] = '\0';
  return curr;
}

void 
gs_config_init(gs_config_t* config, const char* file_name) 
{
  config->m_vwidth      = 1440;
  config->m_vheight     = 900;
  config->m_fscreen     = false;
  config->m_nthreads    = 1;
  config->m_nvkvls      = 0;

  FILE* fd = fopen(file_name, "r");
  if(fd == NULL)
  {
    GS_LOG_ERROR(E_IO_FILE_NOT_FOUND, 
                 "Configuration file %s does not exist", file_name);
  }

  char buffer[GS_CONF_MAX_LINE_LEN];
  char option[GS_CONF_MAX_OPTION_KEY_LEN];
  char value[GS_CONF_MAX_OPTION_VALUE_LEN];
  uint32_t line_count = 0;
  while (fread_line(fd, buffer, GS_CONF_MAX_LINE_LEN) > 0) 
  {
    if(buffer[0] == '#')
    {
      continue;
    }
    char* tok = strtok(buffer," \t" );
    if(tok == NULL)
    {
      GS_LOG_ERROR(E_IO_UNEXPECTED_INPUT_ERROR, 
                   "Ill-formed configuration file entry at line %d", line_count);
    }

    size_t len = strlen(tok);
    if (len > GS_CONF_MAX_OPTION_KEY_LEN-1)
    {
      GS_LOG_ERROR(E_MEMORY_INSUFFICIENT_BUFFER_SIZE_ERROR, 
                   "Configuration file option key is too long")
    }
    memcpy(option, tok, sizeof(char)*(len+1));
    printf("option: %s ",option);

    tok = strtok(NULL," \t");
    if(tok == NULL)
    {
      GS_LOG_ERROR(E_IO_UNEXPECTED_INPUT_ERROR, 
                    "Ill-formed configuration file entry at line %d", line_count);
    }

    len = strlen(tok);
    if (len > GS_CONF_MAX_OPTION_VALUE_LEN-1)
    {
      GS_LOG_ERROR(E_MEMORY_INSUFFICIENT_BUFFER_SIZE_ERROR, 
                   "Configuration file option value is too long")
    }
    memcpy(value, tok, sizeof(char)*(len+1));

    printf("value: %s\n",value);

    if (strcmp(option,"ViewportWidth") == 0) 
    {
      config->m_vwidth = atoi(value);
    }

    if (strcmp(option,"ViewportHeight") == 0) 
    {
      config->m_vheight = atoi(value);
    }

    if (strcmp(option,"FullScreen") == 0)
    {
      config->m_fscreen = atoi(value);
    }

    if (strcmp(option,"VkValidationLayer") == 0) 
    {
      gs_config_insert_vk_layer(config, value);
    }

    if (strcmp(option,"ResourceDirectory") == 0)
    {
      gs_config_insert_rsrc_dir(config, value); 
    }

    if (strcmp(option,"NumWorkerThreads") == 0)
    {
      config->m_nthreads = atoi(value);
    }

    GS_LOG_INFO("Parsed option %s with value %s", option, value);
    line_count++;
  }
  fclose(fd);
  return;
} 

void
gs_config_release(gs_config_t* config)
{
    config->m_nvkvls = 0;
}


void
gs_config_insert_vk_layer(gs_config_t* config, 
                          const char* layer_name)
{
  GS_ASSERT(config->m_nvkvls < GS_CONF_MAX_VKVL_NUM && "Maximum number of validarion layers exceeded");

  uint32_t layer_name_length = strlen(layer_name);
  GS_ASSERT(layer_name_length < GS_CONF_MAX_VKVL_NAME_LEN && "Maximum validation layer name length exceeded");
  strcpy(config->m_vkvls[config->m_nvkvls], layer_name);
  config->m_nvkvls++;
}

void
gs_config_insert_rsrc_dir(gs_config_t* config, 
                          const char* rsrc_dir)
{
  GS_ASSERT(config->m_nrdirs < GS_CONF_MAX_RDIR_NUM && "Maximum number of resource directories exceeded");

  uint32_t rdir_length = strlen(rsrc_dir);
  GS_ASSERT(rdir_length < GS_CONF_MAX_RDIR_LEN && "Maximum resource directory length exceeded");
  strcpy(config->m_rdirs[config->m_nvkvls], rsrc_dir);
  config->m_nrdirs++;
}
