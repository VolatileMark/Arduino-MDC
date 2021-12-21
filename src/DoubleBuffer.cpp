#include "DoubleBuffer.h"

DoubleBuffer::DoubleBuffer(size_t size)
{
    this->buffer = (double*) malloc(size);
    this->size = size;
    memset(this->buffer, 0, this->size);
}

DoubleBuffer::~DoubleBuffer()
{
    free(this->buffer);
}

void DoubleBuffer::shiftRight(void)
{
    for (size_t i = 1; i < size; i++) { buffer[i] = buffer[i - 1]; }
    buffer[0] = 0;
}

void DoubleBuffer::shiftLeft(void)
{
    for (size_t i = 0; i < size - 1; i++) { buffer[i] = buffer[i + 1]; }
    buffer[size - 1] = 0;
}

void DoubleBuffer::push(double value)
{
    shiftLeft();
    buffer[size - 1] = value;
}

double DoubleBuffer::pop(void)
{
    double value = buffer[size - 1];
    shiftRight();
    return value;
}

size_t DoubleBuffer::getSize(void)
{
    return size;
}

double* DoubleBuffer::getBuffer(void)
{
    return buffer;
}