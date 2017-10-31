/**
 * \file brain_builder.h
 * \brief Define the API to load a networkand  from an XML file
 * \author Benoit F.
 * \date 16 decembre 2016
 *
 * All public methods to read build a network and load training data from an XML context
 */
#ifndef BRAIN_BUILDER_H

#include "brain_builder_types.h"

BrainBuilder new_brain_builder(BrainString plugin_definition_file);

#endif /* BRAIN_BUILDER_H */
