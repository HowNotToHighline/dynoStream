#include <stdexcept>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "loadCells/SerialLoadCell.h"

SerialLoadCell::SerialLoadCell() : LoadCellDriver() {
    const char *dev = "/dev/ttyACM1";
    _fd = open(dev, O_RDWR);
    if (_fd < 0) throw std::runtime_error("Failed opening serial port");

    // Configure the port to raw mode (disable buffering until newline)
    struct termios settings{};
    tcgetattr(_fd, &settings);
    cfmakeraw(&settings);
    tcsetattr(_fd, TCSANOW, &settings);
}

unsigned int SerialLoadCell::UpdateData(float *buffer, unsigned int buffer_size) {
    uint8_t serial_buf[40];
    unsigned int read_bytes = read(_fd, serial_buf, std::min(40u, buffer_size * 4));

    unsigned int index = 0;
    uint32_t value = 0;
    unsigned int samples_read = 0;

//    printf("Parsing serial stuff\n");

//    printf("Serial buffer: ");
//    for (int i = 0; i < read_bytes; ++i) printf("%02X", serial_buf[i]);
//    printf("\n");

    for (int i = 0; i < read_bytes; ++i) {
        if((serial_buf[i] & 0b11000000) == 0b10000000) {
//            printf("Detected byte with first bit set\n");
            value = 0;
            index = 0;
        }
        else index++;

        switch (index) {
            case 0:
                value = (serial_buf[i] & 0x3F) << 18;
//                printf("First byte: 0x%02X\n", serial_buf[i]);
                break;
            case 1:
                value |= (serial_buf[i] & 0x7F) << 11;
//                printf("Second byte: 0x%02X\n", serial_buf[i]);
                break;
            case 2:
                value |= (serial_buf[i] & 0x7F) << 4;
//                printf("Third byte: 0x%02X\n", serial_buf[i]);
                break;
            case 3:
                value |= (serial_buf[i] & 0x78) >> 3;
//                printf("Fourth byte: 0x%02X\tvalue: %d\n", serial_buf[i], value);
                buffer[samples_read++] = static_cast<float>(value);
                break;
            default:
                break;
        }
    }

    return samples_read;
}

void SerialLoadCell::Start(float sample_rate) {
    ioctl(_fd, TCFLSH, 0); // Clear unread data
    write(_fd, "a", 1);
    ioctl(_fd, TCFLSH, 1); // Force flush output buffer
}

void SerialLoadCell::Stop() {
    write(_fd, "b", 1);
    ioctl(_fd, TCFLSH, 1);  // Force flush output buffer
    ioctl(_fd, TCFLSH, 0); // Clear unread data
}

std::vector<float> SerialLoadCell::GetSampleRates() {
    return {
            80,
    };
}

SerialLoadCell::~SerialLoadCell() {
    close(_fd);
}
