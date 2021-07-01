#ifndef DYNOSTREAM_SERIALLOADCELL_H
#define DYNOSTREAM_SERIALLOADCELL_H


#include "LoadCellDriver.h"

class SerialLoadCell : public LoadCellDriver {
public:
    SerialLoadCell();

    unsigned int UpdateData(float *buffer, unsigned int buffer_size) override;

    void Start(float sample_rate) override;

    void Stop() override;

    std::vector<float> GetSampleRates() override;

    ~SerialLoadCell();

private:
    int _fd;
};


#endif //DYNOSTREAM_SERIALLOADCELL_H
