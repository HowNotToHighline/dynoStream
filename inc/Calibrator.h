#ifndef DYNOSTREAM_CALIBRATOR_H
#define DYNOSTREAM_CALIBRATOR_H


class Calibrator {
public:
    void AddMeasurement(float raw, float force);
    void CalculateParams();

private:
};


#endif //DYNOSTREAM_CALIBRATOR_H
