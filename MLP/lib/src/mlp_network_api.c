#include "mlp_api.h"

#include "mlp_trainer.h"
#include "mlp_network.h"
#include "mlp_layer.h"

#include "brain_data_utils.h"
#include "brain_memory_utils.h"
#include "brain_logging_utils.h"

#include "mlp_config.h"

#ifdef _GNUC
#define __MLP_VISIBLE__ __attribute__((visibility("default")))
#else
#define __MLP_VISIBLE__
#endif 

void __MLP_VISIBLE__
mlp_network_delete(MLPNetwork network)
{
    if (BRAIN_ALLOCATED(network))
    {
        delete_network(network);
    }
}

MLPNetwork __MLP_VISIBLE__
mlp_network_new(BrainString path)
{
    MLPNetwork network = NULL;

    network = new_network_from_context(path);

    return network;
}

void __MLP_VISIBLE__
mlp_network_serialize(MLPNetwork network, BrainString path)
{
    serialize_network(network, path);
}

void __MLP_VISIBLE__
mlp_network_deserialize(MLPNetwork network, BrainString path)
{
    deserialize_network(network, path);
}

void __MLP_VISIBLE__
mlp_network_predict(MLPNetwork network, BrainUint number_of_inputs, void* signal)
{
    predict(network, number_of_inputs, (BrainSignal)signal);
}

BrainSignal __MLP_VISIBLE__
mlp_network_get_output(MLPNetwork network)
{
    BrainSignal output = NULL;

    if (BRAIN_ALLOCATED(network))
    {
        output = get_network_output(network);
    }

    return output;
}

BrainUint __MLP_VISIBLE__
mlp_network_get_output_length(MLPNetwork network)
{
    return get_network_output_length(network);
}

BrainUint __MLP_VISIBLE__
mlp_network_get_number_of_layer(MLPNetwork network)
{
    return get_network_number_of_layer(network);
}

BrainUint __MLP_VISIBLE__
mlp_network_get_number_of_input(MLPNetwork network)
{
    return get_network_number_of_input(network);
}

BrainUint __MLP_VISIBLE__
mlp_network_get_layer_number_of_neuron(MLPNetwork network, BrainUint layer_index)
{
    BrainUint ret = 0;

    const MLPLayer layer = get_network_layer(network, layer_index);
    if (BRAIN_ALLOCATED(layer))
    {
        ret = get_layer_number_of_neuron(layer);
    }

    return ret;
}

__MLP_VISIBLE__
BrainSignal
mlp_network_get_layer_output_signal(MLPNetwork network, BrainUint layer_index)
{
    double* ret = NULL;

    const MLPLayer layer = get_network_layer(network, layer_index);
    if (BRAIN_ALLOCATED(layer))
    {
        ret = get_layer_output(layer);
    }

    return ret;
}

__MLP_VISIBLE__
BrainSignal
mlp_network_get_input_signal(MLPNetwork network)
{
    return get_network_input_signal(network);
}
