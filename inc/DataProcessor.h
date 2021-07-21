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
        bool calibration;
        float start_level;
        float stop_level;
        bool auto_start;
        bool auto_stop;
        std::string label;

        float load_cell_sample_rate;
        unsigned int decimation;
    };

    struct BufferBlock {
        unsigned int buffer_size = 0;
        float *buffer = nullptr;
        BufferBlock *next = nullptr;
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
    float _offset = 27164.7615480649;
//    float _scale = 1/2182.44996132977;
    float _scale = 4.49341344533039E-06;
    float _latest_sample_value = 0;
    float _peak_force = 0;

    LoadCellDriver *loadCellDriver;
    Decimator *decimator = nullptr;

    std::thread *activeThread = nullptr;
    std::mutex _mutex;

    std::ofstream new_file;

    DataProcessor::Config _config;

    BufferBlock *first_buffer_block = nullptr;
    BufferBlock *last_buffer_block = nullptr;
    unsigned int last_buffer_index = 0;
    unsigned int nr_data_blocks = 0;
#define MINBLOCKSIZE 100u
    void CreateBufferBlock();
    void TrimBufferBlocks();
};


#endif //DYNOSTREAM_DATAPROCESSOR_H
