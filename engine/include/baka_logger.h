#ifndef _BAKA_LOGGER_H_
#define _BAKA_LOGGER_H_

#define bakalog(...) _baka_log(__FILE__,__LINE__,"","",__VA_ARGS__)
#define bakaerr(...) _baka_log(__FILE__, __LINE__, "\033[1;31m", "\033[0m", __VA_ARGS__)
#define bakawarn(...) _baka_log(__FILE__, __LINE__, "\033[1;33m", "\033[0m", __VA_ARGS__)

void _baka_log(const char *f, int l, const char *colorStart, const char *colorEnd, const char *msg, ...);

#endif