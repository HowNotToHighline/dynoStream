#ifndef DYNOSTREAM_DECIMATOR_H
#define DYNOSTREAM_DECIMATOR_H


class Decimator {
public:
    explicit Decimator(unsigned int decimation);

    unsigned int Exec(const float *input_buffer, unsigned int samples_available);

    float *GetBuffer();

    ~Decimator();

private:
    float _intermediate_sum = 0;
    unsigned int _intermediate_sum_items = 0;
    unsigned int _decimation;

    float *_output_buffer{nullptr};
    unsigned int _output_buffer_length = 0;
};


#endif //DYNOSTREAM_DECIMATOR_H
