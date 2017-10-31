#include "brain_logging_utils.h"
#include "brain_core_types.h"
#include <string.h>

#define MESSAGE() va_list args;                                        \
                  va_start(args, format);                              \
                  char buffer[1000];                                   \
                  vsprintf(buffer, format, args);                      \
                  printf("%s\n", buffer);                              \
                  va_end(args);

typedef enum BrainLogLevel
{
    LOG_NONE     = 0,
    LOG_INFO     = 1,
    LOG_DEBUG    = 2,
    LOG_WARNING  = 3,
    LOG_CRITICAL = 4
} BrainLogLevel;

static BrainString _log_levels[] = {
    "info",
    "debug",
    "warning",
    "critical"
};

static BrainLogLevel
getBrainLogLevel(BrainString log_level)
{
    BrainLogLevel level = LOG_NONE;

    if (log_level)
    {
        BrainUint i = 0;

        for (i = 0; i < 3; ++i)
        {
            if (!strcmp(_log_levels[i], log_level))
            {
                level = (BrainLogLevel)(i + 1);
            }
        }
    }

    return level;
}

void
BRAIN_DEBUG(BrainString format, ...)
{
    const BrainLogLevel level = getBrainLogLevel(getenv("BRAIN_LOG_LEVEL"));

    if (level <= LOG_DEBUG)
    {
        MESSAGE()
    }
}

void
BRAIN_INFO(BrainString format, ...)
{
    const BrainLogLevel level = getBrainLogLevel(getenv("BRAIN_LOG_LEVEL"));

    if (level <= LOG_INFO)
    {
        MESSAGE()
    }
}

void
BRAIN_WARNING(BrainString format, ...)
{
    const BrainLogLevel level = getBrainLogLevel(getenv("BRAIN_LOG_LEVEL"));

    if (level <= LOG_WARNING)
    {
        MESSAGE()
    }
}

void
BRAIN_CRITICAL(BrainString format, ...)
{
    const BrainLogLevel level = getBrainLogLevel(getenv("BRAIN_LOG_LEVEL"));

    if (level <= LOG_CRITICAL)
    {
        MESSAGE()
    }
}
