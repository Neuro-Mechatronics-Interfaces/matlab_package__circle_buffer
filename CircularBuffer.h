#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include "mex.h"
#include <vector>

class CircularBuffer {
public:
    CircularBuffer(size_t numChannels, size_t bufferSize);
    ~CircularBuffer();
    
    void add(const float* data, size_t numSamples);
    void get(float* data, size_t numSamples, size_t channel, size_t startSample) const;
    void getMostRecent(float* data, size_t numSamples) const;
    
    size_t getNumChannels() const { return numChannels; }
    size_t getBufferSize() const { return bufferSize; }
    
private:
    std::vector<std::vector<float>> buffer;
    size_t bufferSize;
    size_t numChannels;
    size_t head;
    size_t tail;
    bool isFull;
};

#endif // CIRCULARBUFFER_H
