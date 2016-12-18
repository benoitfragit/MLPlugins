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

/**
 * \fn BrainNetwork new_network_from_context(Context context)
 * \brief Fonction to create a BrainNetwork from an XML context
 *
 * \param context an XML context
 * \return a new allocated BrainNetwork or NULL if it failed
 */
BrainNetwork new_network_from_context(Context context);
/**
 * \fn void initialize_network_from_context(BrainNetwork network, Context context)
 * \brief load previously trained neural network's weight
 *
 * \param network BrainNetwork to be initialized
 * \param context Context to load all weight
 */
void initialize_network_from_context(BrainNetwork network,
                                     Context      context);
/**
 * \fn BrainData new_data_from_context(Context context)
 * \brief Load training data from an XML context
 *
 * \param context the XML context to use
 * \return a newly allocated BrainData
 */
BrainData new_data_from_context(Context context);

#endif /* BRAIN_BUILDER_H */
