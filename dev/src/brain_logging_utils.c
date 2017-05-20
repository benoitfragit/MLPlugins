#include "brain_logging_utils.h"
#include "brain_utils_config.h"
#include <string.h>


void
BRAIN_DEBUG(BrainString format, ...)
{
    va_list args;
    va_start(args, format);
    char buffer[1000];
    vsprintf(buffer, format, args);
    printf("<%s>: %s\n", BRAIN_DOMAIN, buffer);
    va_end(args);
}

void
BRAIN_INFO(BrainString format, ...)
{
    va_list args;
    va_start(args, format);
    char buffer[1000];
    vsprintf(buffer, format, args);
    printf("<%s>: %s\n", BRAIN_DOMAIN, buffer);
    va_end(args);
}

void
BRAIN_CRITICAL(BrainString format, ...)
{
    va_list args;
    va_start(args, format);
    char buffer[1000];
    vsprintf(buffer, format, args);
    printf("<%s>: %s\n", BRAIN_DOMAIN, buffer);
    va_end(args);
}
