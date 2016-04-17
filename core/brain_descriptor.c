#include "brain_descriptor.h"

int
isNodeWithName(xmlNodePtr node, const char* name)
{
    if (!xmlStrcmp(node->name, (const xmlChar *)name))
    {
        return 1;
    }

    return 0;
}

xmlNodePtr
getNodeByNameAndIndex(xmlNodePtr node, const char* name, const int i)
{
    int index = 0;
    xmlNodePtr child;

    if (node)
    {
        child = node->xmlChildrenNode;

        while (child)
        {
            if(isNodeWithName(child, name))
            {
                if (i == index)
                {
                    return child;
                }

                ++index;
            }

            child = child->next;
        }

    }

    return NULL;
}

int
getNumberOfNodeByName(xmlNodePtr node, const char* node_name)
{
    int number_of_node = 0;
    xmlNodePtr child;

    if (node)
    {
        child = node->xmlChildrenNode;

        while (child)
        {
            if(isNodeWithName(child, node_name))
            {
                ++number_of_node;
            }

            child = child->next;
        }

        xmlFree(child);
    }

    return number_of_node;
}

void
appendNewConnection(NetworkDescriptor* descriptor,
                    const int input_layer,
                    const int input_neuron,
                    const int output_layer,
                    const int output_neuron)
{
    if (descriptor)
    {
        if (!descriptor->_connections)
        {
            descriptor->_connections = (Connection *)malloc(descriptor->_number_of_connections * sizeof(Connection));
        }
        else
        {
            descriptor->_connections = (Connection *)realloc(descriptor->_connections, descriptor->_number_of_connections*sizeof(Connection));
        }

        descriptor->_connections[descriptor->_number_of_connections - 1]._input_layer = input_layer;
        descriptor->_connections[descriptor->_number_of_connections - 1]._input_neuron = input_neuron;
        descriptor->_connections[descriptor->_number_of_connections - 1]._output_layer = output_layer;
        descriptor->_connections[descriptor->_number_of_connections - 1]._output_neuron = output_neuron;
    }
}

NetworkDescriptor*
brain_from_file(const char* xml_file)
{
    xmlDocPtr doc;
    xmlNodePtr root, node_layer, node_neuron, node_connect;
    NetworkDescriptor* descriptor = NULL;
    int layer_index = 0, neuron_index = 0, connect_index = 0, layer_id = -1, neuron_id = -1, tmp_connect;
    xmlChar *layer_id_str, *neuron_id_str, *neuron_inputs, *output_layer, *output_neuron;

    doc = xmlParseFile (xml_file);
    if (!doc)
    {
        fprintf (stderr, "error %s:%d\n",  __FILE__, __LINE__);
        return NULL;
    }

    //process the xml file
    root = xmlDocGetRootElement(doc);

    descriptor = (NetworkDescriptor *)malloc(sizeof(NetworkDescriptor));

    descriptor->_number_of_connections = 0;
    descriptor->_connections = NULL;
    descriptor->_number_of_layers = getNumberOfNodeByName(root, "layer");
    descriptor->_number_of_neuron_per_layers = (int *)malloc(descriptor->_number_of_layers*sizeof(int));
    descriptor->_number_of_input_per_neuron  = (int **)malloc(descriptor->_number_of_layers*sizeof(int *));

    for (layer_index = 0; layer_index < descriptor->_number_of_layers; ++layer_index)
    {
        node_layer = getNodeByNameAndIndex(root, "layer", layer_index);

        if (node_layer)
        {
            descriptor->_number_of_neuron_per_layers[layer_index] = getNumberOfNodeByName(node_layer, "neuron");
            descriptor->_number_of_input_per_neuron[layer_index] = (int *)malloc(descriptor->_number_of_neuron_per_layers[layer_index] * sizeof(int));

            layer_id_str = xmlGetProp(node_layer, "id");
            if (layer_id_str)
            {
                layer_id = atoi(layer_id_str);
            }

            for (neuron_index = 0; neuron_index < descriptor->_number_of_neuron_per_layers[layer_index]; ++neuron_index)
            {
                node_neuron = getNodeByNameAndIndex(node_layer, "neuron", neuron_index);

                if (node_neuron)
                {
                    neuron_inputs = xmlGetProp(node_neuron, "inputs");
                    neuron_id_str = xmlGetProp(node_neuron, "id");

                    if (neuron_inputs)
                    {
                        descriptor->_number_of_input_per_neuron[layer_index][neuron_index] = atoi(neuron_inputs);
                    }

                    if (neuron_id_str)
                    {
                        neuron_id = atoi(neuron_id_str);
                    }

                    tmp_connect = getNumberOfNodeByName(node_neuron, "connect");

                    for (connect_index = 0; connect_index < tmp_connect; ++connect_index)
                    {
                        node_connect = getNodeByNameAndIndex(node_neuron, "connect", connect_index);

                        if (node_connect)
                        {
                            output_layer  = xmlGetProp(node_connect, "layer");
                            output_neuron = xmlGetProp(node_connect, "neuron");

                            if (output_layer && output_neuron)
                            {
                                ++descriptor->_number_of_connections;

                                appendNewConnection(descriptor,
                                                    layer_id,
                                                    neuron_id,
                                                    atoi(output_layer),
                                                    atoi(output_neuron));
                            }
                        }
                    }
                }

                xmlFree(node_neuron);
            }
        }

        xmlFree(node_layer);
    }

    xmlFreeNode(root);
    xmlFreeDoc (doc);

    return descriptor;
}

void
printNetworkDescriptor(const NetworkDescriptor* descriptor)
{
    int i = 0, j = 0;

    if (descriptor)
    {
        if (descriptor->_number_of_neuron_per_layers)
        {
            for (i = 0; i < descriptor->_number_of_layers;++i)
            {
                printf("layer %d has %d neurons\n", i, descriptor->_number_of_neuron_per_layers[i]);

                if (descriptor->_number_of_input_per_neuron[i])
                {
                    for (j = 0; j < descriptor->_number_of_neuron_per_layers[i]; ++j)
                    {
                        printf("\tneuron %d has inputs %d\n", j, descriptor->_number_of_input_per_neuron[i][j]);
                    }
                }
            }
        }

        if (descriptor->_connections)
        {
            for (i = 0; i < descriptor->_number_of_connections; ++i)
            {
                printf("connect (%d, %d) to (%d, %d)\n",
                        descriptor->_connections[i]._input_layer,
                        descriptor->_connections[i]._input_neuron,
                        descriptor->_connections[i]._output_layer,
                        descriptor->_connections[i]._output_neuron);
            }
        }
    }
}

void
deleteDescriptor(NetworkDescriptor* descriptor)
{
    int i;
    if (descriptor)
    {
        if (descriptor->_connections)
        {
            free(descriptor->_connections);
        }

        for (i = 0; i < descriptor->_number_of_layers; ++i)
        {
            if (descriptor->_number_of_input_per_neuron[i])
            {
                free(descriptor->_number_of_input_per_neuron[i]);
            }
        }

        free(descriptor->_number_of_input_per_neuron);
        free(descriptor->_number_of_neuron_per_layers);
    }
}
