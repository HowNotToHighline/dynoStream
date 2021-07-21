#include <cmath>
#include <stdexcept>
#include <iostream>
#include <thread>
#include <filesystem>
#include <utility>
#include <loadCells/VirtualLoadCell.h>
#include <loadCells/SerialLoadCell.h>
#include "DataProcessor.h"

DataProcessor::DataProcessor(bool unused/* = false*/) {
//    loadCellDriver = new VirtualLoadCell();
    loadCellDriver = new SerialLoadCell();
//    decimator = unused ? new Decimator(1) : nullptr;
    decimator = new Decimator(60);
}

void DataProcessor::Run() {
    // Main event loop
    while (started) {
        if (first_buffer_block == nullptr || last_buffer_block == nullptr ||
            last_buffer_block->buffer_size == last_buffer_index)
            CreateBufferBlock();
        unsigned int samples_available = last_buffer_block->buffer_size - last_buffer_index;
        float *data = last_buffer_block->buffer + last_buffer_index;

        // This is (or should be) a blocking, throttling call
        samples_available = loadCellDriver->UpdateData(data, samples_available);
        if (decimator != nullptr) {
            samples_available = decimator->Exec(data, samples_available);
        }

        if (_config.auto_start && !recording) {
            for (int i = 0; i < samples_available; ++i) {
                if (data[i] > _config.start_level) {
                    const std::lock_guard<std::mutex> lock(_mutex);

                    recording = true;
                    start_sample_index = i;
                    std::cout << "Sample exceeded autoStartLevel\n";
                    break;
                }
            }
        }


        for (int i = 0; i < samples_available; ++i) {
            if (i == samples_available - 1) _latest_sample_value = data[i];
            float force = (data[i] - _offset) * _scale;
            printf("%d: %f\n", i, force);
            // TODO: Add initialization
            if (force > _peak_force) {
                // TODO: Peak force callback
                _peak_force = force;
            }
            last_buffer_index++;
        }

        if (!recording) TrimBufferBlocks();
        // TODO: Store buffer(s) to file when recording ends

        // Only start looking for stop samples after the start sample
        if (_config.auto_stop && recording) {
            for (int i = start_sample_index; i < samples_available; ++i) {
                if (data[i] < _config.stop_level) {
                    std::cout << "Sample dropped below autoStopLevel\n";
                    Stop();
                }
            }
        }

        start_sample_index = 0;
    }
}

void DataProcessor::Start(DataProcessor::Config config) {
    const std::lock_guard<std::mutex> lock(_mutex);
    if (started) throw std::runtime_error("Already started");

    _config = std::move(config);
    // If not auto starting, start recording immediately
    recording = !_config.auto_start && !_config.calibration;

    // Enable data stream from load loadCellDriver driver
    loadCellDriver->Start(_config.load_cell_sample_rate);

    // TODO: Disable block system for calibration

    if (!_config.calibration) {
        time_t t = time(nullptr);
        char timestamp_prefix[80];
        strftime(timestamp_prefix, 80, "%d-%m-%Y_%H:%M:%S_", localtime(&t));
        std::string directory = "./";
        std::string file_name = directory + std::string(timestamp_prefix) + _config.label + ".dat";

        // TODO: Deal with this better
        if (std::filesystem::exists(file_name)) throw std::runtime_error("Log file already exists!");
        new_file.open(file_name);
        if (!new_file) throw std::runtime_error("Log file creation failed");
    }

    started = true;

    activeThread = new std::thread(&DataProcessor::Run, this);
}

void DataProcessor::Stop() {
    const std::lock_guard<std::mutex> lock(_mutex);

    started = false;
    recording = false;

    // Prevent dead lock when called from thread
    // This is possible because Stop only gets called by Run at the end of a cycle, so the file handle and load cell driver won't be used again
    if (activeThread != nullptr && activeThread->get_id() != std::this_thread::get_id()) activeThread->join();

    loadCellDriver->Stop();
    if (new_file.is_open()) {
        BufferBlock *ptr = first_buffer_block;
        while (ptr != nullptr) {
            unsigned int limit = ptr == last_buffer_block ? last_buffer_index : ptr->buffer_size;
            for (int i = 0; i < limit; ++i) {
                new_file << ptr->buffer[i] << "\n";
            }
            ptr = ptr->next;
        }
        new_file.flush();
        new_file.close();
    }

    // TODO: deallocate memory blocks
    BufferBlock *ptr = first_buffer_block;
    while (ptr != nullptr) {
        BufferBlock *next = ptr->next;
        delete[] ptr->buffer;
        delete ptr;
        ptr = next;
    }
    first_buffer_block = nullptr;
    last_buffer_block = nullptr;
}


void DataProcessor::Tare() {
    const std::lock_guard<std::mutex> lock(_mutex);
    if (_config.load_cell_sample_rate / static_cast<float>(_config.decimation) > 1)
        std::cout << "warning: The samplerate is above 1Hz, this might cause tare inaccuracies\n";
    _offset = _latest_sample_value;
}

void DataProcessor::SetScale(float scale) {
    const std::lock_guard<std::mutex> lock(_mutex);
    _scale = scale;
}

void DataProcessor::SetOffset(float offset) {
    const std::lock_guard<std::mutex> lock(_mutex);
    _offset = offset;
}

float DataProcessor::GetLatestSample() const {
    printf("latest_sample_value: %f\n", _latest_sample_value);
    return _latest_sample_value;
}

float DataProcessor::GetPeakForce() const {
    return _peak_force;
}

void DataProcessor::CreateBufferBlock() {
    auto prev_last_buffer_block = last_buffer_block;
    last_buffer_block = new BufferBlock;
    unsigned int size = std::max(MINBLOCKSIZE, static_cast<unsigned int>(_config.load_cell_sample_rate /
                                                                         static_cast<float>(_config.decimation)));
    last_buffer_block->buffer = new float[size];
    last_buffer_block->buffer_size = size;
    if (prev_last_buffer_block != nullptr) prev_last_buffer_block->next = last_buffer_block;
    if (first_buffer_block == nullptr) first_buffer_block = last_buffer_block;
    last_buffer_index = 0;
    nr_data_blocks++;
//    printf("nr_data_blocks: %d\n", nr_data_blocks);
}

void DataProcessor::TrimBufferBlocks() {
    // TODO: Make it possible te remove multiple blocks
    unsigned int seconds_look_back = 2;
    // TODO: samples_look_back currently can't become less than first_buffer_block->buffer_size
    unsigned int samples_look_back = seconds_look_back * static_cast<unsigned int>(_config.load_cell_sample_rate /
                                                                                   static_cast<float>(_config.decimation));
    // The running samples only count towards the count if the running block isn't also the first one
    unsigned int samples_without_first_block = first_buffer_block == last_buffer_block ? 0 : last_buffer_index;
    BufferBlock *ptr = first_buffer_block->next;
    if (ptr != nullptr) {
        while (ptr != last_buffer_block) {
            samples_without_first_block += ptr->buffer_size;
            ptr = ptr->next;
        }
    }

//    printf("samples_without_first_block: %d\n", samples_without_first_block);
    if (samples_without_first_block >= samples_look_back) {
        auto prev_first_buffer_block = first_buffer_block;
        first_buffer_block = first_buffer_block->next;
        delete[] prev_first_buffer_block->buffer;
        delete prev_first_buffer_block;
        nr_data_blocks--;
//        printf("nr_data_blocks: %d\n", nr_data_blocks);
    }
}

DataProcessor::~DataProcessor() {
    // TODO
//    Stop();
    delete loadCellDriver;
    delete decimator;
    delete activeThread;
}
