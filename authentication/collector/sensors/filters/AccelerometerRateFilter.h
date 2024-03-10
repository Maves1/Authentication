#pragma once

#include <QAccelerometerFilter>

class AccelerometerRateFilter : public QAccelerometerFilter
{
public:
    bool filter(QAccelerometerReading *reading) override;

    void setSamplingRate(int hz);
    int getSamplingRate();

private:
    int m_samplingRate;
    quint64 m_lastTimestamp = 0;
    quint64 m_filterLength = 10;
};
