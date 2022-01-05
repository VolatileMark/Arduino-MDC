#pragma once

#include "srprintf.h"

#ifdef __DEBUG__
    #define logmsg(lvl, msg, ...) srprintf("[" lvl "] @ (%s:%d) " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
    #define logmsg(lvl, msg, ...) ((void)0)
#endif

#define info(msg, ...) logmsg("INFO", msg, ##__VA_ARGS__)
#define warn(msg, ...) logmsg("WARN", msg, ##__VA_ARGS__)
#define err(msg, ...) logmsg("ERRO", msg, ##__VA_ARGS__)
#define logaxis(x, y, z) logmsg("AXIS", "%f / %f / %f", x, y, z);