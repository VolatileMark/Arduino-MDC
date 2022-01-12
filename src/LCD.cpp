#include "LCD.h"
#include "MDC.h"
#include "Log.h"
#include <LiquidCrystal_I2C.h>
#include <stdarg.h>

static LiquidCrystal_I2C lcd(0x27, 16, 2);
static MDC_STATE currentState;

void initLCD(void)
{
    lcd.init();
    lcd.backlight();
    lcd.clear();
    currentState = MDC_STANDING;
}

void printOnLCD(const char* s, ...)
{
    uint8_t x, y;
    char* p;
    va_list ap;

    va_start(ap, s);

    lcd.clear();
    lcd.setCursor(0, 0);

    for 
    (
        p = *((char**) &s), x = 0, y = 0; 
        *p != '\0'; 
        p++
    )
    {
        if (*p == '\n')
        {
            ++y;
            x = 0;
            continue;
        }

        lcd.setCursor(x++, y);

        if (*p != '%')
        {
            lcd.print(*p);
            continue;
        }

        switch (*(++p))
        {
        case 's':
            lcd.print(va_arg(ap, char*));
            break;
        case 'p':
        case 'X':
            lcd.print(va_arg(ap, uint32_t), 16);
            break;
        case 'c':
        case 'i':
        case 'd':
            lcd.print(va_arg(ap, int));
            break;
        case 'u':
            lcd.print(va_arg(ap, uint32_t));
            break;
        case 'f':
            lcd.print(va_arg(ap, double), 6);
            break;
        case '%':
            lcd.print('%');
            break;
        }
    }

    va_end(ap);
}

void updateState(MDC_STATE state)
{
    if (state == currentState)
        return;
    printState(state);
}

void printState(MDC_STATE state)
{
    switch (state)
    {
    case MDC_STANDING:
        printOnLCD("Standing");
        info("Detected: standing");
        break;
    case MDC_SITTING:
        printOnLCD("Sitting");
        info("Detected: sitting");
        break;
    case MDC_LAYING_B:
        printOnLCD("Laying\nback");
        info("Detected: laying on back");
        break;
    case MDC_LAYING_F:
        printOnLCD("Laying\nfront");
        info("Detected: laying on front");
        break;
    case MDC_LAYING_L:
        printOnLCD("Laying\nleft");
        info("Detected: laying on left side");
        break;
    case MDC_LAYING_R:
        printOnLCD("Laying\nright");
        info("Detected: laying on right side");
        break;
    case MDC_WALKING:
        printOnLCD("Walking");
        info("Detected: walking");
        break;
    }

    currentState = state;
}

void clearLCD(void)
{
    lcd.clear();
}