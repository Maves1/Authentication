#include <QGyroscope>
#include <QLoggingCategory>

#include "GyroscopeReader.h"

Q_LOGGING_CATEGORY(collectorGyroscope, "collector.gyroscope")

GyroscopeReader::GyroscopeReader(QObject *parent) : SensorReader(new QGyroscope())
{
    qCDebug(collectorGyroscope) << "ctor";
}
