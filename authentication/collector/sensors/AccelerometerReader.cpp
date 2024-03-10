#include <QAccelerometer>
#include <QLoggingCategory>

#include "AccelerometerReader.h"
#include "filters/AccelerometerRateFilter.h"

Q_LOGGING_CATEGORY(collectorAccelerometer, "collector.accelerometer")

AccelerometerReader::AccelerometerReader(QObject *parent) : SensorReader(new QAccelerometer())
{
    qCDebug(collectorAccelerometer) << "ctor";

    auto accelerometerFilter = new AccelerometerRateFilter();
    accelerometerFilter->setSamplingRate(100);

    m_sensor->addFilter(accelerometerFilter);
}
