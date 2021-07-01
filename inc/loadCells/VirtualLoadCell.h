#ifndef DYNOSTREAM_VIRTUALLOADCELL_H
#define DYNOSTREAM_VIRTUALLOADCELL_H


#include "LoadCellDriver.h"

class VirtualLoadCell : public LoadCellDriver {
public:
    VirtualLoadCell();

    unsigned int UpdateData(float *buffer, unsigned int buffer_size) override;

    void Start(float sample_rate) override;

    void Stop() override;

    std::vector<float> GetSampleRates() override;

private:
};


#endif //DYNOSTREAM_VIRTUALLOADCELL_H
