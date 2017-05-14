/**
 * \file brain_builder.h
 * \brief Define the API to load a networkand  from an XML file
 * \author Benoit F.
 * \date 16 decembre 2016
 *
 * All public methods to read build a network and load training data from an XML context
 */
#ifndef BRAIN_BUILDER_H

#include "brain_types.h"

/**
 * \fn BrainNetwork new_network_from_context(BrainString filepath)
 * \brief Fonction to create a BrainNetwork from an XML context
 *
 * \param filepath XML Network definition file
 * \return a new allocated BrainNetwork or NULL if it failed
 */
BrainNetwork new_network_from_context(BrainString filepath);
/**
 * \fn void deserialize(BrainNetwork network, BrainString filepath)
 * \brief load previously trained neural network's weight
 *
 * \param network BrainNetwork to be initialized
 * \param filepath XML file to load
 */
void deserialize(BrainNetwork network,
                 BrainString filepath);
/**
 * \fn BrainData new_data_from_context(BrainString filepath)
 * \brief Load training data from an XML context
 *
 * \param filepath the XML context to use
 * \return a newly allocated BrainData
 */
BrainData new_data_from_context(BrainString filepath);
/**
 * \fn void new_settings_from_context(BrainString filepath)
 * \brief Load settings from an XML context
 *
 * \param filepath the XML context to use
 */
void new_settings_from_context(BrainString filepath);
/**
 * \fn void serialize(const BrainNetwork network, const BrainString filepath)
 * \brief serialize a network to a file
 *
 * \param network the BrainNetwork to serialize
 * \param filepath the file that will be created
 */
void serialize(const BrainNetwork network, const BrainString filepath);
#endif /* BRAIN_BUILDER_H */
