#ifndef DYNOSTREAM_LOADCELLDRIVER_H
#define DYNOSTREAM_LOADCELLDRIVER_H

#include <vector>


class LoadCellDriver {
public:
    virtual unsigned int UpdateData(float *buffer, unsigned int buffer_size) = 0;

    virtual void Start(float sample_rate) = 0;

    virtual void Stop() = 0;

    virtual std::vector<float> GetSampleRates() = 0;

    virtual ~LoadCellDriver() = default;

protected:
private:
};

#endif //DYNOSTREAM_LOADCELLDRIVER_H
