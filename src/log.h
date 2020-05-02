
#ifndef _GS_TLOG_H_
#define _GS_TLOG_H_

#include "platform.h"
#include "error.h"
#include <stdio.h>

/**
 * \brief Initializes the log module
 *
 * \param filename The filename of the log
 */
void
gs_log_init(const char* filename);

/**
 * \brief Releases the log module
 */
void
gs_log_release(void);

/**
 * \brief Logs an INFO message to the log
 *
 * \param message The message to log
 * \param ...
 */
void 
gs_log_info(const char* message, ... );

/**
 * \brief Logs an error message to the log
 *
 * \param error The error to log
 * \param message The message error to log
 * \param ...
 */
void 
gs_log_error(gs_error_t error, const char* message, ... );

/**
 * \brief Logs a warning to the log
 *
 * \param message The message to log
 * \param ...
 */
void 
gs_log_warning(const char* message, ... );

#define GS_LOG_INFO(message, ...) \
                gs_log_info(message, ##__VA_ARGS__);\

#define GS_LOG_ERROR(error_type, message, ...)\
                gs_log_error(error_type, message, ##__VA_ARGS__);\

#define GS_LOG_WARNING(message, ...)\
                gs_log_warning(message, ##__VA_ARGS__);\

#endif
