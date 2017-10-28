#ifndef BRAIN_LOGGING_UTILS_H
#define BRAIN_LOGGING_UTILS_H

#include "brain_core_types.h"

void BRAIN_CRITICAL(BrainString format, ...);
void BRAIN_DEBUG   (BrainString format, ...);
void BRAIN_INFO    (BrainString format, ...);
void BRAIN_WARNING (BrainString format, ...);

#endif /* BRAIN_LOGGING_UTILS_H */
