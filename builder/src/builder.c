#include "brain_builder.h"

BrainInt
main(BrainInt argc, BrainChar** argv)
{
    BrainInt ret = EXIT_FAILURE;

    BrainBuilder builder = new_brain_builder("Brain",
                                             "new_network_from_context",
                                             "configure_network_with_context",
                                             "serialize_network",
                                             "deserialize_network",
                                             "train_network",
                                             "predict");

    if (builder)
    {
        ret = EXIT_SUCCESS;
    }

    return ret;
}
