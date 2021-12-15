#include "srprintf.h"
#include <Arduino.h>
#include <stdarg.h>

void srprintf(const char* s, ...)
{
    char* p;
    va_list ap;

    va_start(ap, s);

    for (p = *((char**) &s); *p != '\0'; p++)
    {
        if (*p != '%')
        {
            Serial.print(*p);
            continue;
        }

        switch (*(++p))
        {
        case 's':
            Serial.print(va_arg(ap, char*));
            break;
        case 'p':
        case 'X':
            Serial.print(va_arg(ap, uint32_t), 16);
            break;
        case 'c':
        case 'i':
        case 'd':
            Serial.print(va_arg(ap, int));
            break;
        case 'u':
            Serial.print(va_arg(ap, uint32_t));
            break;
        case 'f':
            Serial.print(va_arg(ap, double));
            break;
        case '%':
            Serial.print('%');
            break;
        }
    }

    va_end(ap);
}