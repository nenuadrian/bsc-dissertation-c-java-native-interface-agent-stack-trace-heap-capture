#ifndef UTIL_LOGGING_H 
#define UTIL_LOGGING_H 

void error(const char* message, ...);
void info(const char* message, ...);
void debug(const char* message, ...);
void log_c(const char* tag, const char* message, ...);

#endif