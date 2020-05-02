#include "log.h"
#include "error.h"
#include "mutex.h"
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <signal.h>

#define VARIADIC_BUFFER_SIZE 1024
#define EXTRACT_VARIADIC( buffer, message ) \
                            va_list larg; \
                            va_start(larg, message); \
                            vsnprintf(buffer, VARIADIC_BUFFER_SIZE, message, larg); \
                            va_end(larg);

#define TIMED_MESSAGE( message ) \
                           time_t rawtime;  \
                           time(&rawtime); \
                           char* str_time = ctime(&rawtime); \
                           str_time[strlen(str_time)-2] = '\0';\
                           fprintf(m_log.p_log_file, "%s %s\n", str_time, message);

void gs_log_signal_handler(int signal)
{
  gs_log_release();
}

struct gs_log_t 
{
  FILE*           p_log_file;
  int32_t         m_errors;
} m_log;

gs_mutex_t  m_mutex;

void 
gs_log_aux(const char* type, 
        const char* message );

void
gs_log_init(const char* filename)
{
  m_log.m_errors = 0;
  m_log.p_log_file = fopen(filename, "w");
  gs_mutex_init(&m_mutex);

  // Registering signal handlers
  signal(SIGABRT, gs_log_signal_handler);
  signal(SIGSEGV, gs_log_signal_handler);
  
  GS_LOG_INFO("Started execution");
}


void
gs_log_release(void)
{
  GS_LOG_INFO("Finished execution");
  if(m_log.p_log_file) 
  {
    fclose(m_log.p_log_file);
  }
  gs_mutex_release(&m_mutex);
  if(m_log.m_errors > 0) 
  {
    printf("Execution finished with errors. Check the log file\n");
  }
}

void 
gs_log_info(const char* message, ...) {
  GS_ASSERT(m_log.p_log_file != NULL && "Logfile is not initialized");
  char buffer[VARIADIC_BUFFER_SIZE]; 
  EXTRACT_VARIADIC(buffer, message)
  gs_log_aux("INFO", buffer);
}

void 
gs_log_error(gs_error_t error, const char* message, ...) {
    GS_ASSERT(m_log.p_log_file != NULL && "Logfile is not initialized");
    gs_log_aux("ERROR", error_messages[(uint32_t)error]);
    char buffer[VARIADIC_BUFFER_SIZE]; 
    EXTRACT_VARIADIC(buffer, message)
    gs_log_aux("ERROR", buffer);
    gs_log_release();
    abort();
}

void 
gs_log_warning(const char* message, ...) {
    GS_ASSERT(m_log.p_log_file != NULL && "Logfile is not initialized");
    char buffer[VARIADIC_BUFFER_SIZE]; 
    EXTRACT_VARIADIC(buffer, message)
    gs_log_aux("WARNING", buffer);
}

void 
gs_log_aux(const char* type, const char* message) {
    gs_mutex_lock(&m_mutex);
    GS_ASSERT(m_log.p_log_file != NULL && "Logfile is not initialized");
    char buffer[VARIADIC_BUFFER_SIZE];
    snprintf(buffer, VARIADIC_BUFFER_SIZE, ": %5s: %5s",type, message);
    TIMED_MESSAGE(buffer);
    gs_mutex_unlock(&m_mutex);
}

#undef EXTRACT_VARIADIC
#undef VARIADIC_BUFFER_SIZE
