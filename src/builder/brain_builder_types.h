#ifndef BRAIN_BUILDER_H

#include "brain_types.h"

#include <stdio.h>
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
