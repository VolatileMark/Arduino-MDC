#pragma once

#include <Arduino.h>

class DoubleBuffer
{
public:
    DoubleBuffer(size_t size);
    ~DoubleBuffer();
    void push(double value);
    double pop(void);
    void put(double value);
    size_t getSize(void);
    double* getBuffer(void);
    void print(void);
    void clear(void);
private:
    void shiftRight(void);
    void shiftLeft(void);
    double* buffer;
    size_t size;
    uint32_t index;
};