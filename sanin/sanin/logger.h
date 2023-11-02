#ifndef _LOGGER_H_
#define _LOGGER_H_
#include <syslog.h>

#define DEBUG_LOG(args...) log(__FILE__,__func__,__LINE__, LOG_DEBUG, args);
#define INFO_LOG(args...) log(__FILE__,__func__,__LINE__, LOG_INFO, args);
#define ERROR_LOG(args...) log(__FILE__,__func__,__LINE__, LOG_ERR, args);

void init_log(const char* name);
void log(const char *file_name, const char* function_name, int line_number,int priority, const char* format, ...);
void close_log();
void set_log_level(int level);

#endif //_LOGGER_H_
