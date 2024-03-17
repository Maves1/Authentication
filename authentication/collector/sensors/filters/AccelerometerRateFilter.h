#pragma once

#include <QAccelerometerFilter>

class AccelerometerRateFilter : public QAccelerometerFilter
{
public:
    AccelerometerRateFilter();

    bool filter(QAccelerometerReading *reading) override;

    void setSamplingRate(int hz);
    int getSamplingRate();

private:
    int m_samplingRate;
    quint64 m_lastTimestamp;
    quint64 m_filterLength;
};
