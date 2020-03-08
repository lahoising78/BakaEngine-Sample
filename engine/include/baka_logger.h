#ifndef _BAKA_LOGGER_H_
#define _BAKA_LOGGER_H_

#define bakalog(...) _baka_log(__FILE__,__LINE__,__VA_ARGS__)
void _baka_log(const char *f,int l,const char *msg,...);

#endif