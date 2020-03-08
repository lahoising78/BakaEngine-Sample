#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "baka_logger.h"

const char *logger_get_filename(const char *filename, char separator = '/')
{
    const char *sepPos = strrchr(filename, separator);

    return sepPos? sepPos + 1 : "";
}

void _baka_log(const char *f, int l, const char *colorStart, const char *colorEnd, const char *msg, ...)
{
    va_list va;
    
    va_start(va, msg);
    printf("%s%s:%d:%s ", colorStart, logger_get_filename(f), l, colorEnd);
    vprintf(msg, va);
    printf("\n");
    va_end(va);
}