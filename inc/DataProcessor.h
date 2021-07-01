#ifndef DYNOSTREAM_DATAPROCESSOR_H
#define DYNOSTREAM_DATAPROCESSOR_H


#include <loadCells/LoadCellDriver.h>
#include "Decimator.h"
#include <thread>
#include <mutex>
#include <fstream>

class DataProcessor {
public:
    explicit DataProcessor(bool unused = false);

    void Run();

    void Stop();

    struct Config {
        float start_level;
        float stop_level;
        bool auto_start;
        bool auto_stop;
        std::string label;

        float load_cell_sample_rate;
        unsigned int decimation;
    };

    void Start(DataProcessor::Config config);

    void Tare();

    void SetScale(float scale);

    void SetOffset(float offset);

    float GetLatestSample() const;

    float GetPeakForce() const;

    ~DataProcessor();

private:
    bool recording = false;
    bool started = false;
    int start_sample_index = 0;
    float _offset = 0;
    float _scale = 1;
    float _latest_sample_value = 0;
    float _peak_force = 0;

    LoadCellDriver *loadCellDriver;
    Decimator *decimator = nullptr;

    std::thread *activeThread = nullptr;
    std::mutex _mutex;

    std::ofstream new_file;

    DataProcessor::Config _config;
};


#endif //DYNOSTREAM_DATAPROCESSOR_H
