#pragma once

#include "srprintf.h"

#ifdef __DEBUG__
    #define logmsg(lvl, msg, ...) srprintf("[" lvl "] @ (%s:%d) " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
    #define logmsg(lvl, msg, ...) ((void)0)
#endif

#define INFO(msg, ...) logmsg("INFO", msg, ##__VA_ARGS__)
#define WARN(msg, ...) logmsg("WARN", msg, ##__VA_ARGS__)
#define ERR(msg, ...) logmsg("ERRO", msg, ##__VA_ARGS__)
#define LOG_AXIS(x, y, z) logmsg("AXIS", "%f / %f / %f", x, y, z);