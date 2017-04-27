#include "common_data_structures.h"

xmlChar * docToChar(xmlDocPtr doc) {
  xmlChar * mem;
  int size;
  xmlDocDumpMemory(doc, &mem, &size);
  //xmlDocDumpFormatMemory(doc, &mem, &size, 1);
  return mem;
}

xmlDocPtr charToXML(const char * content) {
    if (content == NULL) {
      error("XML parse error");
      return NULL;
    }

    xmlDocPtr doc; 
    doc = xmlReadMemory(content, strlen(content) + 1, "noname.xml", NULL, 0);
    if (doc == NULL) {
      error("XML parse error");
    }
    
    return doc;
}

char * findNodeValue(xmlNode * a_node, char * node_name)
{
    xmlNode *cur_node = NULL;
    char * response = NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
          if (!xmlStrcmp(cur_node->name, (const xmlChar *)node_name)) {
            return (char *)xmlNodeGetContent(cur_node);
          }
        }

        response = findNodeValue(cur_node->children, node_name);
        if (response != NULL) return response;
    }

    return NULL;
}
