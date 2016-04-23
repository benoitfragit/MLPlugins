#ifndef BRAIN_XML_UTILS_H
#define BRAIN_XML_UTILS_H

#include <libxml/tree.h>
#include <libxml/xpath.h>

typedef xmlNodePtr Context;
typedef xmlDocPtr Document;

int      is_node_with_name(Context node, const char* name);
Context  get_node_with_name_and_index(Context node, const char* name, const int i);
int      get_number_of_node_with_name(Context node, const char* node_name);
xmlChar* node_get_prop(Context node, const char* key);
int      node_get_int(Context node, const char* key, const int _default);
double   node_get_double(Context node, const char* key, const double _default);
Document open_document(const char *doc);
void     close_document(Document doc);
Context  get_root_node(Document doc);

#endif /*BRAIN_XML_UTILS_H*/
