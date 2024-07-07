#include "CircularBuffer.h"
#include <algorithm>
#include <cstring>

CircularBuffer::CircularBuffer(size_t numChannels, size_t bufferSize)
    : bufferSize(bufferSize), numChannels(numChannels), head(0), tail(0), isFull(false) {
    buffer.resize(numChannels, std::vector<float>(bufferSize, 0.0f));
}

CircularBuffer::~CircularBuffer() {
    // Destructor
}

void CircularBuffer::add(const float* data, size_t numSamples) {
    for (size_t i = 0; i < numSamples; ++i) {
        for (size_t ch = 0; ch < numChannels; ++ch) {
            buffer[ch][head] = data[ch + i * numChannels];
        }
        head = (head + 1) % bufferSize;
        if (isFull) {
            tail = (tail + 1) % bufferSize;
        }
        if (head == tail) {
            isFull = true;
        }
    }
}

void CircularBuffer::get(float* data, size_t numSamples, size_t channel, size_t startSample) const {
    if (channel >= numChannels) {
        mexErrMsgIdAndTxt("MATLAB:CircularBuffer:get", "Invalid channel index.");
    }
    size_t currentSample = (tail + startSample) % bufferSize;
    for (size_t i = 0; i < numSamples; ++i) {
        data[i] = buffer[channel][currentSample];
        currentSample = (currentSample + 1) % bufferSize;
    }
}

void CircularBuffer::getMostRecent(float* data, size_t numSamples) const {
    if (numSamples > bufferSize) {
        mexErrMsgIdAndTxt("MATLAB:CircularBuffer:getMostRecent", "Number of samples requested exceeds buffer size.");
    }
    size_t startSample = (head + bufferSize - numSamples) % bufferSize;
    size_t endSample = (head == 0) ? bufferSize - 1 : head - 1;
    size_t numSamplesFirstPart = (endSample >= startSample) ? numSamples : bufferSize - startSample;
    size_t numSamplesSecondPart = numSamples - numSamplesFirstPart;

    // Copy the first part of the samples
    for (size_t ch = 0; ch < numChannels; ++ch) {
        std::copy(buffer[ch].begin() + startSample, buffer[ch].begin() + startSample + numSamplesFirstPart, data + ch * numSamples);
        if (numSamplesSecondPart > 0) {
            std::copy(buffer[ch].begin(), buffer[ch].begin() + numSamplesSecondPart, data + ch * numSamples + numSamplesFirstPart);
        }
    }
}
