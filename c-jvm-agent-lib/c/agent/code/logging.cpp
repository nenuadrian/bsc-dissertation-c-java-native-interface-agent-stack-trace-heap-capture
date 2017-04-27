#include <stdio.h>
#include <stdarg.h>
#include <time.h>  
#include <string.h>

extern int VERBOSE;

void log_format(const char* tag, const char* message, va_list args) {   
   time_t now;     
   time(&now);     
   char *date = ctime(&now);   
   date[strlen(date) - 1] = '\0';  
   printf("[AGENT][%s][%s] ", date, tag);  
   vprintf(message, args);     
   printf("\n"); 
 }

 void error(const char* message, ...) { if (VERBOSE < 3) return; va_list args;   va_start(args, message);    log_format("ERROR", message, args);     va_end(args); }
 void info(const char* message, ...) { if (VERBOSE < 2) return; va_list args;   va_start(args, message);    log_format("INFO", message, args);  va_end(args); }
 void debug(const char* message, ...) { if (VERBOSE < 1) return; va_list args;   va_start(args, message);    log_format("DEBUG", message, args);     va_end(args); }
 void log_c(const char* tag, const char* message, ...) { if (VERBOSE == 0) return; va_list args;   va_start(args, message);    log_format(tag, message, args);     va_end(args); }
