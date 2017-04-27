#ifndef UTIL_XML_H 
#define UTIL_XML_H 

#include <libxml/parser.h>

xmlChar * docToChar(xmlDocPtr doc);
xmlDocPtr charToXML(const char * content);
char * findNodeValue(xmlNode * a_node, char * node_name);

#endif
