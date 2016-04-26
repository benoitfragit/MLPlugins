#include "brain_logging_utils.h"
#include <string.h>

#ifdef ENABLE_GLIB_EXTRA
#include <glib.h>
#endif

void
BRAIN_DEBUG(const char *format, ...)
{
	va_list args;
    va_start(args, format);
	char buffer[1000];
	vsprintf(buffer, format, args);
#ifdef ENABLE_GLIB_EXTRA
	g_log(BRAIN_DOMAIN, G_LOG_LEVEL_DEBUG, buffer);
#else
    vprintf("<%s>: %s\n", BRAIN_DOMAIN, buffer);
#endif
	va_end(args);
}

void
BRAIN_INFO(const char *format, ...)
{
	va_list args;
    va_start(args, format);
	char buffer[1000];
	vsprintf(buffer, format, args);
#ifdef ENABLE_GLIB_EXTRA
	g_log(BRAIN_DOMAIN, G_LOG_LEVEL_INFO, buffer);
#else
    vprintf("<%s>: %s\n", BRAIN_DOMAIN, buffer);
#endif
	va_end(args);
}

void
BRAIN_CRITICAL(const char *format, ...)
{
	va_list args;
    va_start(args, format);
	char buffer[1000];
	vsprintf(buffer, format, args);
#ifdef ENABLE_GLIB_EXTRA
	g_log(BRAIN_DOMAIN, G_LOG_LEVEL_CRITICAL, buffer);
#else
    vprintf("<%s>: %s\n", BRAIN_DOMAIN, buffer);
#endif
	va_end(args);
}


