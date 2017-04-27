#ifndef UTIL_SRV_COM_H 
#define UTIL_SRV_COM_H 

#include <libxml/parser.h>

struct url_data;
void write_data(void *ptr, size_t size, size_t nmemb, struct url_data *data);
char * sc_sendDataToServer(char message[], char * type);
void server_communication_init();
void server_communication_cleanup();
xmlChar * sc_exHolderToXmlChar(struct ExceptionHolder * exHolder);
#endif
