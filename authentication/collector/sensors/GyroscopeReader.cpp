#include <QGyroscope>
#include <QLoggingCategory>

#include "GyroscopeReader.h"
#include "filters/GyroscopeRateFilter.h"

Q_LOGGING_CATEGORY(collectorGyroscope, "collector.gyroscope")

GyroscopeReader::GyroscopeReader(QObject *parent) : SensorReader(new QGyroscope())
{
    qCDebug(collectorGyroscope) << "ctor";

    auto gyroscopeFilter = new GyroscopeRateFilter();
    gyroscopeFilter->setSamplingRate(100);

    m_sensor->addFilter(gyroscopeFilter);
}
