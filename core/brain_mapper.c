#include "brain_mapper.h"

void
appendNewConnection(Connections* connections,
                    const int input_layer,
                    const int input_neuron,
                    const int output_layer,
                    const int output_neuron)
{
    if (connections)
    {
        if (connections->_size == 0)
        {
            connections->_connection = (Connection *)malloc(sizeof(Connection));
            connections->_size = 1;
        }
        else
        {
            ++(connections->_size);
            connections->_connection = (Connection *)realloc(connections->_connection, connections->_size*sizeof(Connection));
        }

        connections->_connection[connections->_size - 1]._input_layer = input_layer;
        connections->_connection[connections->_size - 1]._input_neuron = input_neuron;
        connections->_connection[connections->_size - 1]._output_layer = output_layer;
        connections->_connection[connections->_size - 1]._output_neuron = output_neuron;
    }
}

Connections*
brain_map_from_file(const char* xml_file)
{
    xmlDocPtr doc;
    xmlNodePtr node;
    xmlNodePtr root;

    Connections* connections = NULL;
    doc = xmlParseFile (xml_file);
    if (!doc)
    {
        fprintf (stderr, "error %s:%d\n",  __FILE__, __LINE__);
        return NULL;
    }

    //process the xml file
    root = xmlDocGetRootElement(doc);

    connections = brain_map_from_node(root);

    xmlFreeNode(root);
    xmlFreeDoc (doc);

    return connections;
}

Connections*
brain_map_from_node(xmlNodePtr node)
{
    xmlNodePtr child;

    if (!node)
    {
        return NULL;
    }

    Connections* connections = (Connections *)malloc(sizeof(Connections));
    connections->_size = 0;

    child = node->xmlChildrenNode;

    while(child != NULL)
    {
        if (!xmlStrcmp(child->name, (const xmlChar*)"connect"))
        {
            xmlChar* input_layer   = xmlGetProp(child, "input_layer");
            xmlChar* input_neuron  = xmlGetProp(child, "input_neuron");
            xmlChar* output_layer  = xmlGetProp(child, "output_layer");
            xmlChar* output_neuron = xmlGetProp(child, "output_neuron");

            if (input_layer && input_neuron && output_layer && output_neuron)
            {
                printf("%s %s %s %s\n",
                        input_layer,
                        input_neuron,
                        output_layer,
                        output_neuron);
                appendNewConnection(connections,
                                    atoi(input_layer),
                                    atoi(input_neuron),
                                    atoi(output_layer),
                                    atoi(output_neuron));
            }

            xmlFree(input_layer);
            xmlFree(input_neuron);
            xmlFree(output_layer);
            xmlFree(output_neuron);
        }
        child = child->next;
    }

    xmlFreeNode(child);

    return connections;
}
