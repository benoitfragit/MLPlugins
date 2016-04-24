#include "brain_logging_utils.h"
#include <string.h>

#ifdef ENABLE_GLIB_EXTRA
#include <glib.h>
#endif

#ifdef ENABLE_GLIB_EXTRA
GLogLevelFlags
BRAIN_LOG_LEVEL(const char* level)
{
    if (!strcmp(level, "info"))
    {
        return G_LOG_LEVEL_INFO;
    }
    else if (!strcmp(level, "debug"))
    {
        return G_LOG_LEVEL_DEBUG;
    }
    else if (!strcmp(level, "warning"))
    {
        return G_LOG_LEVEL_WARNING;
    }
    else if (!strcmp(level, "critical"))
    {
        return G_LOG_LEVEL_CRITICAL;
    }
    else if (!strcmp(level, "error"))
    {
        return G_LOG_LEVEL_ERROR;
    }

    return G_LOG_LEVEL_INFO;
}

void
BRAIN_LOG(const char* log_domain, const char* log_level, const char *format, ...)
{
    char buffer[1000];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    g_log(log_domain, BRAIN_LOG_LEVEL(log_level), buffer);
    va_end(args);
}
#else
void
BRAIN_LOG(const char *log_domain, const char* level, const char* format)
{
    va_list args;
    va_start(args, format);
    vprintf("<%s, %s>: %s\n", log_domain, level, format, args);
    va_end(args);
}
#endif
