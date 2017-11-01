#include "brain_builder.h"

BrainInt
main(BrainInt argc, BrainChar** argv)
{
    BrainInt ret = EXIT_FAILURE;

    BrainBuilder builder = new_brain_builder("install/debug/libbrain-1.0.0/api/mlp.xml");

    if (builder)
    {
        ret = EXIT_SUCCESS;
    }

    return ret;
}
