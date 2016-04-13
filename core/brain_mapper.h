#ifndef BRAIN_MAPPER_H
#define BRAIN_MAPPER_H

typedef struct Connection
{
    int _neuron;
    int _left;
} Connection;

typedef struct Connection* Connections;

Connections brain_map_from_file(const char* xml_file);

#endif /* BRAIN_MAPPER_H */
