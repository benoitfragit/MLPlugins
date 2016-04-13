#include "brain_mapper.h"

#include <libxml/tree.h>
#include <libxml/xpath.h>

Connections
brain_map_from_file(const char* xml_file)
{
    Connections connections = (Connections)malloc(sizeof(Connection));

    xmlDocPtr xmldoc = NULL;
    xmldoc = xmlParseFile (xml_file);

    if (!xmldoc)
    {
        fprintf (stderr, "%s:%d: y'aurait pas comme un petit probleme ?\n",  __FILE__, __LINE__);
        return NULL;
    }

    //process the xml file


    //free the document
    xmlFreeDoc (xmldoc);

    return connections;
}
