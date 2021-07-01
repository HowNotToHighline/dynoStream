#include <cmath>
#include <stdexcept>
#include "loadCells/VirtualLoadCell.h"

VirtualLoadCell::VirtualLoadCell() : LoadCellDriver() {
}

unsigned int VirtualLoadCell::UpdateData(float *buffer, unsigned int buffer_size) {
    if(buffer == nullptr) throw std::runtime_error("Buffer isn't initialized!");

    buffer[0] = 0;
    buffer[1] = 1;
    buffer[2] = 2;
    buffer[3] = 10;
    buffer[4] = 1;
    buffer[5] = 0.1;
    buffer[6] = -0.1;

    return 7;
}

void VirtualLoadCell::Start(float sample_rate) {
    // Inform hardware to start streaming
}

void VirtualLoadCell::Stop() {
    // Inform hardware to stop streaming
}

std::vector<float> VirtualLoadCell::GetSampleRates() {
    return {
        10,
        80,
        100000,
        400000,
    };
}
