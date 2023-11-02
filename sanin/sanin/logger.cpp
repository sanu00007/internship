#include "logger.h"
#include <stdarg.h>
#include <stdio.h>

static int log_mask = LOG_MASK(LOG_ERR)|LOG_MASK(LOG_INFO);

void init_log(const char* name){
  openlog(name, LOG_NDELAY|LOG_PID, LOG_USER);
}

void log(const char *file_name, const char* function_name, int line_number,int priority, const char* format, ...){
  char log_buff[200];
  char net_buff[250];
  va_list args;
  int new_mask = LOG_MASK(priority);
  if(log_mask & new_mask){
    va_start(args, format);
    vsnprintf(log_buff,199,format,args);
    va_end(args);
    snprintf(net_buff,249,"%s(%s):%d > %s", file_name, function_name, line_number, log_buff);
    syslog(LOG_INFO, net_buff);
  }
}

void close_log() {
  closelog();
}

void set_log_level(int level){
  int mask = 0;
  switch(level){
    case 3:
      mask |= LOG_MASK(LOG_DEBUG);
    case 2:
      mask |= LOG_MASK(LOG_INFO);
    case 1:
      mask |= LOG_MASK(LOG_ERR);
  }
  if(mask){
    log_mask = mask;
  }
}
