#pragma once

#include <Arduino.h>

class DoubleBuffer
{
public:
    DoubleBuffer(size_t size);
    ~DoubleBuffer();
    void push(double value);
    double pop(void);
    size_t getSize(void);
    double* getBuffer(void);
private:
    void shiftRight(void);
    void shiftLeft(void);
    double* buffer;
    size_t size;
};