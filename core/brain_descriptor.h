#ifndef BRAIN_MAPPER_H
#define BRAIN_MAPPER_H
#include "brain_type.h"
#include <libxml/tree.h>
#include <libxml/xpath.h>

NetworkDescriptor* brain_from_file(const char* xml_file);
void printNetworkDescriptor(const NetworkDescriptor* descriptor);
void deleteDescriptor(NetworkDescriptor* descriptor);

#endif /* BRAIN_MAPPER_H */
