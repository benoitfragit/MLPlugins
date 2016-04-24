#ifndef BRAIN_LOGGING_UTILS_H
#define BRAIN_LOGGING_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "brain_config.h"

void BRAIN_LOG(const char *log_domain,
               const char* level,
               const char* format,
               ...);

#endif /* BRAIN_LOGGING_UTILS_H */
