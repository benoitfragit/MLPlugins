#ifndef BRAIN_PROBE_H
#define BRAIN_PROBE_H

#include "brain_logging_utils.h"
#include <time.h>

#define BRAIN_PROBE_START(func) clock_t t = clock();                   \
                                BRAIN_DEBUG("[%s:%d] Start probing", #func, __LINE__);
#define BRAIN_PROBE_STOP(func)  BrainDouble elapsed = ((BrainDouble)(clock() - t)) / CLOCKS_PER_SEC;\
                                BRAIN_DEBUG("[%s:%d] Elapsed time: %.8lf", #func, __LINE__, elapsed);

#endif /* BRAIN_PROBE_H */
