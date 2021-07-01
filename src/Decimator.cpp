#include <stdexcept>
#include "Decimator.h"

Decimator::Decimator(unsigned int decimation) {
    _decimation = decimation;
    _output_buffer = new float[4096];
}

unsigned int Decimator::Exec(const float *input_buffer, unsigned int samples_available) {
    if (_output_buffer == nullptr) throw std::runtime_error("Buffer isn't initialized");

    float sum = _intermediate_sum;
    unsigned int index = _intermediate_sum_items;
    unsigned int output_items = 0;

    for (unsigned int i = 0; i < samples_available; i++) {
        sum += input_buffer[i];
        // == should be enough, but I'm paranoid for strange edge cases that might somehow skip one cycle
        if (++index >= _decimation) {
            _output_buffer[output_items++] = sum / static_cast<float>(_decimation);
            sum = 0;
            index = 0;
        }
    }

    _intermediate_sum = sum;
    _intermediate_sum_items = index;

    return output_items;
}

float *Decimator::GetBuffer() {
    return _output_buffer;
}

Decimator::~Decimator() {
    delete _output_buffer;
}
