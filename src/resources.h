


#ifndef _GS_RESOURCES_H_
#define _GS_RESOURCES_H_ value

#include "platform.h"

/**
 * \brief Initializes the resources subsystem 
 */
void
gs_resources_init(void);

/**
 * \brief Release the resources subsystem
 */
void
gs_resources_release(void);

/**
 * \brief Adds a resource directory to the resources subsystem
 *
 * \param rdir The resource directory to add
 */
void
gs_resources_add_rdir(const char* rdir);


/**
 * \brief finds the path to a file 
 *
 * \param filename The namae of the file
 */
bool
gs_resources_find_file(const char* filename, 
                       char* buffer, 
                       uint32_t blength);

/**
 * \brief Reads a file into the given buffer
 *
 * \param filename The name of the file to read
 * \param buffer The buffer to read the file into 
 * \param bsize The size of the buffer 
 *
 */
uint32_t 
gs_resources_read_file(const char*  filename, 
                       char* buffer, 
                       uint32_t bsize);


#endif /* ifndef _GS_RESOURCES_H_ */
