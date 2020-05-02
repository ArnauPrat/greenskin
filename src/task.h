

#ifndef _GS_TASKING_TASK_H_
#define _GS_TASKING_TASK_H_

typedef void(*gs_task_function_t)(void *arg);  

typedef struct gs_task_t 
{
  /**
   * @brief Pointer to the function that executed the task
   */
  gs_task_function_t  p_fp;

  /**
   * @brief Pointer to the argument data that will be passed to the task
   * function
   */
  void*         p_args;

} gs_task_t;

#endif /* ifndef _TASKING_TASK_H_ */
