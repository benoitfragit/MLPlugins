#ifndef BRAIN_LOGGING_UTILS_H
#define BRAIN_LOGGING_UTILS_H

#include "brain_core_types.h"

#define BRAIN_INPUT(func)  BRAIN_DEBUG(">>>> Brain ENTERING:\t%s", #func);
#define BRAIN_OUTPUT(func) BRAIN_DEBUG("<<<< Brain LEAVING:\t%s", #func);

void BRAIN_CRITICAL(BrainString format, ...);
void BRAIN_DEBUG   (BrainString format, ...);
void BRAIN_INFO    (BrainString format, ...);
void BRAIN_WARNING (BrainString format, ...);

#endif /* BRAIN_LOGGING_UTILS_H */
