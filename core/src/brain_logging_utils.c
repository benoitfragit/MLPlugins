#include "brain_logging_utils.h"
#include "brain_core_types.h"
#include "brain_enum_utils.h"
#include <string.h>

#define MESSAGE(level)  if (_current_log_level <= level)               \
                        {                                              \
                            va_list args;                              \
                            va_start(args, format);                    \
                            char buffer[1000];                         \
                            vsprintf(buffer, format, args);            \
                            printf("%s\n", buffer);                    \
                            va_end(args);                              \
                        }

typedef enum BrainLogLevel
{
    LOG_DEBUG    = 0,
    LOG_INFO     = 1,
    LOG_WARNING  = 2,
    LOG_CRITICAL = 3,
    LOG_NONE     = 4
} BrainLogLevel;

static BrainLogLevel _current_log_level = LOG_NONE;

static BrainString _log_levels[] = {
    "debug",
    "info",
    "warning",
    "critical"
};

void
brain_logging_init()
{
    BrainString log_level = getenv("BRAIN_LOG_LEVEL");

    _current_log_level =  get_enum_values(_log_levels,
                                            LOG_DEBUG,
                                            LOG_NONE,
                                            log_level);
}

void
BRAIN_DEBUG(BrainString format, ...)
{
#if BRAIN_ENABLE_LOGGING
    MESSAGE(LOG_DEBUG)
#endif /* BRAIN_ENABLE_LOGGING */
}

void
BRAIN_INFO(BrainString format, ...)
{
#if BRAIN_ENABLE_LOGGING
    MESSAGE(LOG_INFO)
#endif /* BRAIN_ENABLE_LOGGING */
}

void
BRAIN_WARNING(BrainString format, ...)
{
#if BRAIN_ENABLE_LOGGING
    MESSAGE(LOG_WARNING)
#endif /* BRAIN_ENABLE_LOGGING */
}

void
BRAIN_CRITICAL(BrainString format, ...)
{
#if BRAIN_ENABLE_LOGGING
    MESSAGE(LOG_CRITICAL)
#endif /* BRAIN_ENABLE_LOGGING */
}
