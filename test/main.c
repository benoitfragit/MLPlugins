#include "brain.h"
#include "brain_mapper.h"

int
main(int argc, char** argv)
{
    Connections* connections = brain_map_from_file("/home/benoit/Documents/projets/C/libBrain/test/mapping.xml");


    return EXIT_SUCCESS;
}
