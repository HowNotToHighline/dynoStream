#include <stdexcept>
#include <unistd.h>
#include "loadCells/VirtualLoadCell.h"

VirtualLoadCell::VirtualLoadCell() : LoadCellDriver() {
}

unsigned int VirtualLoadCell::UpdateData(float *buffer, unsigned int buffer_size) {
//    buffer[0] = 0;
//    buffer[1] = 1;
//    buffer[2] = 2;
//    buffer[3] = 10;
//    buffer[4] = 1;
//    buffer[5] = 0.1;
//    buffer[6] = -0.1;
//
//    return 7;

    static float counter = 0;
    counter += 0.1;
    buffer[0] = counter;
    usleep(1000*10); // Sleep 10ms to throttle rate
    return 1;
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
