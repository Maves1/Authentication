#include "GyroscopeRateFilter.h"

bool GyroscopeRateFilter::filter(QGyroscopeReading *reading)
{
    quint64 currTimestampMs = reading->timestamp() / 1000;

    if (currTimestampMs - m_lastTimestamp >= m_filterLength)
    {
        m_lastTimestamp = currTimestampMs;
        return true;
    }
    return false;
}

int GyroscopeRateFilter::getSamplingRate()
{
    return m_samplingRate;
}

void GyroscopeRateFilter::setSamplingRate(int hz)
{
    m_samplingRate = hz;
    m_filterLength = 1000 / m_samplingRate; // 1000ms
}
