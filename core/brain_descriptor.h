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

typedef struct NetworkDescriptor
{
    Connection* _connections;
    int   _number_of_connections;
    int   _number_of_layers;
    int*  _number_of_neuron_per_layers;
    int** _number_of_input_per_neuron;
} NetworkDescriptor;

NetworkDescriptor* brain_from_file(const char* xml_file);
void printNetworkDescriptor(const NetworkDescriptor* descriptor);
void deleteDescriptor(NetworkDescriptor* descriptor);

#endif /* BRAIN_MAPPER_H */
