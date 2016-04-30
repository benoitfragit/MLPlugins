#include "brain_synapse_builder.h"

Synapse*
new_synapse_from_context(Context context)
{
    Synapse* _synapse = NULL;

    if (!context || !is_node_with_name(context, "connect"))
    {
        fprintf (stderr, "<%s:%d> Context is not valid !\n",  __FILE__, __LINE__);
        return NULL;
    }

    _synapse = (Synapse *)malloc(sizeof(Synapse));

    _synapse->_input_layer   = node_get_int(context, "input-layer",   0);
    _synapse->_output_layer  = node_get_int(context, "output-layer",  0);
    _synapse->_input_neuron  = node_get_int(context, "input-neuron",  0);
    _synapse->_output_neuron = node_get_int(context, "output-neuron", 0);
    _synapse->_input_index   = node_get_int(context, "input-index",   0);

    return _synapse;
}

void
delete_synapse(Synapse* synapse)
{
    if (synapse)
    {
        free(synapse);
    }
}

void
dump_synapse(Synapse* synapse, FILE* file)
{
	if (synapse && file)
	{
		fprintf(file, "\t<connect input-layer=\"%d\" input-neuron=\"%d\" output-layer=\"%d\" output-neuron=\"%d\" input-index=\"%d\">\n", synapse->_input_layer,
																																  synapse->_input_neuron,
																																  synapse->_output_layer,
																																  synapse->_output_neuron,
																																  synapse->_input_index);
	}
}
