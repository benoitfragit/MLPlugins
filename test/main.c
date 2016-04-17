#include "brain.h"
#include "brain_descriptor.h"

int
main(int argc, char** argv)
{
    NetworkDescriptor* descriptor = brain_from_file("/home/benoit/Documents/projets/C/libBrain/test/mapping.xml");
    Network* network = createNetwork(descriptor->_number_of_layers,
                                     descriptor->_number_of_neuron_per_layers,
                                     descriptor->_number_of_input_per_neuron);

    printNetworkDescriptor(descriptor);

    deleteDescriptor(descriptor);
    deleteNetwork(network);
    return EXIT_SUCCESS;
}
