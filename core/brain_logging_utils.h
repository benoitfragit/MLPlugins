#ifndef BRAIN_LOGGING_UTILS_H
#define BRAIN_LOGGING_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "brain_config.h"

void BRAIN_CRITICAL(const char* format, ...);
void BRAIN_DEBUG   (const char* format, ...);
void BRAIN_INFO    (const char* format, ...);

#endif /* BRAIN_LOGGING_UTILS_H */
