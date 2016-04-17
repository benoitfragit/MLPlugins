#ifndef BRAIN_MAPPER_H
#define BRAIN_MAPPER_H
#include <libxml/tree.h>
#include <libxml/xpath.h>

typedef struct Connection
{
    int _input_neuron;
    int _input_layer;
    int _output_neuron;
    int _output_layer;
} Connection;

typedef struct Connections
{
    Connection* _connection;
    int _size;
} Connections;

Connections* brain_map_from_file(const char* xml_file);
Connections* brain_map_from_node(xmlNodePtr node);

#endif /* BRAIN_MAPPER_H */
