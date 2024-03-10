#include "AccelerometerRateFilter.h"

bool AccelerometerRateFilter::filter(QAccelerometerReading *reading)
{
    quint64 currTimestampMs = reading->timestamp() / 1000;

    if (currTimestampMs - m_lastTimestamp >= m_filterLength)
    {
        m_lastTimestamp = currTimestampMs;
        return true;
    }
    return false;
}

int AccelerometerRateFilter::getSamplingRate()
{
    return m_samplingRate;
}

void AccelerometerRateFilter::setSamplingRate(int hz)
{
    m_samplingRate = hz;
    m_filterLength = 1000 / m_samplingRate; // 1000ms
}
