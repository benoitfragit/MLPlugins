#include "brain_builder.h"

int
main(int argc, char** argv)
{
    int ret = EXIT_FAILURE;

    BrainBuilder builder = new_brain_builder("Brain",
                                             "new_network_from_context",
                                             "configure_network_with_context",
                                             "serialize_network",
                                             "deserialize_network",
                                             "train_network",
                                             "predict");

    if (builder)
    {
        printf("Ok, builder is loaded\n");

        ret = EXIT_SUCCESS;
    }

    return ret;
}
