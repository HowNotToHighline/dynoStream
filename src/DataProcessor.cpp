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
    decimator = unused ? new Decimator(1) : nullptr;
}

void DataProcessor::Run() {
    // Main event loop
    while (started) {
        // This is (or should be) a blocking, throttling call
        float data[4096];
        unsigned int samples_available = loadCellDriver->UpdateData(data, 4096);
        if (decimator != nullptr) {
            samples_available = decimator->Exec(data, samples_available);
        }

        for (int i = 0; i < samples_available; ++i) {
            if (_config.auto_start && !recording && data[i] > _config.start_level) {
                const std::lock_guard<std::mutex> lock(_mutex);

                recording = true;
                start_sample_index = i;
                std::cout << "Sample exceeded autoStartLevel\n";
                break;
            }
        }

        if (recording) {
            for (int i = 0; i < samples_available; ++i) {
                if(i == samples_available - 1) _latest_sample_value = data[i];
                float force = (data[i] - _offset) * _scale;
                printf("%d: %f\n", i, force);
                new_file << force << "\n";
                if(force > _peak_force) {
                    // TODO: Peak force callback
                    _peak_force = force;
                }
            }
            // TODO: Add x seconds look back
        }

        // Only stop recording after saving
        // Only start looking for stop samples after the start sample
        for (int i = start_sample_index; i < samples_available; ++i) {
            if (_config.auto_stop && recording && data[i] < _config.stop_level) {
                std::cout << "Sample dropped below autoStopLevel\n";
                Stop();
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
    recording = !_config.auto_start;

    // Enable data stream from load loadCellDriver driver
    loadCellDriver->Start(_config.load_cell_sample_rate);

    // TODO: Add timestamp
    std::string file_name = "test-" + _config.label + ".dat";
    // TODO: Deal with this better
    if (std::filesystem::exists(file_name)) throw std::runtime_error("Log file already exists!");
    new_file.open(file_name);
    if (!new_file) throw std::runtime_error("Log file creation failed");

    started = true;

    activeThread = new std::thread(&DataProcessor::Run, this);
}

void DataProcessor::Stop() {
    const std::lock_guard<std::mutex> lock(_mutex);

    started = false;
    // TODO: Maybe turn on again
//    recording = false;

    // Prevent dead lock when called from thread
    // This is possible because Stop only gets called by Run at the end of a cycle, so the file handle and load cell driver won't be used again
    if (activeThread != nullptr && activeThread->get_id() != std::this_thread::get_id()) activeThread->join();

    loadCellDriver->Stop();
    if (new_file.is_open()) {
        new_file.flush();
        new_file.close();
    }
}



void DataProcessor::Tare() {
    const std::lock_guard<std::mutex> lock(_mutex);
    if(_config.load_cell_sample_rate / static_cast<float>(_config.decimation) > 1) std::cout << "warning TODO";
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
    return _latest_sample_value;
}

float DataProcessor::GetPeakForce() const {
    return _peak_force;
}

DataProcessor::~DataProcessor() {
    delete loadCellDriver;
    delete decimator;
}
