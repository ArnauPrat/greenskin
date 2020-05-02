
#ifndef _GS_BUFFER_H_
#define _GS_BUFFER_H_ 

#include "platform.h"

#define GS_RENDERING_INVALID_BUFFER NULL 

typedef void* gs_vbuffer_t;
typedef void* gs_ibuffer_t;

/**
 * \brief Initializes a vertex buffer
 *
 * \param buffer The vertex buffer to initialize
 * \param data The data to initialize the vertex buffer with
 * \param size The size in bytes of the data to initialize
 */
void
gs_vertex_buffer_init(gs_vbuffer_t* buffer, 
                      void* data,
                      size_t size);

/**
 * \brief Releases a vertex buffer
 *
 * \param buffer The vertex buffer to release
 */
void
gs_vertex_buffer_release(gs_vbuffer_t* buffer);

/**
 * \brief Initializes an index buffer
 *
 * \param buffer The index buffer to initialize
 * \param data The data to initialize the index buffer with
 * \param size The size in bytes of the data to initialize
 */
void
gs_index_buffer_init(gs_ibuffer_t* buffer, 
                    void* data,
                    size_t size);

/**
 * \brief Releases an index buffer
 *
 * \param buffer The index buffer to release
 */
void
gs_index_buffer_release(gs_ibuffer_t* buffer);

#endif 
