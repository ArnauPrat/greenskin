

#ifndef _GS_SHADER_H_
#define _GS_SHADER_H_

typedef void* shader_t;

shader_t* 
gs_shader_create(const char* path);

void 
gs_shader_destroy(shader_t* shader);

#endif /* ifndef _TNA_SHADER_H_ */
