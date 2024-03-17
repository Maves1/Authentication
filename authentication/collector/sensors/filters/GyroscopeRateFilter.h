#pragma once

#include <QGyroscopeFilter>

class GyroscopeRateFilter : public QGyroscopeFilter
{
public:
    GyroscopeRateFilter();

    bool filter(QGyroscopeReading *reading) override;

    void setSamplingRate(int hz);
    int getSamplingRate();

private:
    int m_samplingRate;
    quint64 m_lastTimestamp;
    quint64 m_filterLength;
};
