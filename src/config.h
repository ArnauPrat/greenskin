
#ifndef _GS_CONFIG_H_
#define _GS_CONFIG_H_ value

#include "platform.h"

/**
 * \brief Structure used to store the engine's configuration
 */
typedef struct gs_config_t 
{
  uint32_t   m_vwidth;      // the viewport width
  uint32_t   m_vheight;     // the viewport heigth
  bool       m_fscreen;     // fullscreen mode
  uint32_t   m_nthreads;    // number of worker threads

  uint32_t   m_nvkvls;      // number of vulkan validation layers to load
  uint32_t   m_nrdirs;      // number of resource directories to register
  char       m_vkvls[GS_CONF_MAX_VKVL_NUM][GS_CONF_MAX_VKVL_NAME_LEN];  // vulkan validation layers to load
  char       m_rdirs[GS_CONF_MAX_RDIR_NUM][GS_CONF_MAX_RDIR_LEN];       // resource directories to register
} gs_config_t;


/**
 * \brief Loads the configuration stored in the given configuration file
 *
 * \param config A pointer to the config object to store the configuration to
 * \param configFileName The file containing the configuration
 *
 * \return E_NO_ERROR if successful.  
 */
void 
gs_config_init(gs_config_t* config,
               const char* file_name);

/**
 * \brief Releases the configuration object
 *
 * \param config
 */
void
gs_config_release(gs_config_t* config);

/**
 * \brief Inserts a vulkan validation layer to the configuration
 *
 * \param layer_name The name of the layer
 */
void
gs_config_insert_vk_layer(gs_config_t* config,
                          const char* layer_name); 

/**
 * \brief Inserts a resource directory to the conmfiguration
 *
 * \param config The configuration structure
 * \param rsrc_dir The resource directory
 */
void
gs_config_insert_rsrc_dir(gs_config_t* config,
                          const char* rsrc_dir); 

#endif
