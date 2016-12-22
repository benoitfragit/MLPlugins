/** \internal
 * \file brain_builder_types.h
 * \brief Define all BrainBuilder types
 * \author Benoit F.
 * \date 11 decembre 2016
 *
 * This file contains all predefines types used in this library
 */
#ifndef BRAIN_BUILDER_H

#include "brain_types.h"

#include <libxml/tree.h>
#include <libxml/xpath.h>

#include "brain_utils_config.h"

/**
 * \brief define a XML Context
 */
typedef xmlNodePtr Context;
/**
 * \brief define an XML Document
 */
typedef xmlDocPtr Document;
/**
 * \brief Define an XML buffer
 */
typedef xmlChar*  Buffer;

#endif /* BRAIN_BUILDER_H */
