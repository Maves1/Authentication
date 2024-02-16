#include <QSensor>
#include <QLoggingCategory>

#include "SensorReader.h"

Q_LOGGING_CATEGORY(collectorSensorReader, "collector.sensorreader")

SensorReader::SensorReader(QSensor *sensor) : QObject(nullptr), m_sensor(sensor)
{
    qCDebug(collectorSensorReader) << "ctor";
}

SensorReader::~SensorReader(){};

void SensorReader::startReading()
{
    if (!m_sensor->isActive())
    {
        m_sensor->start();
        connect(m_sensor, &QSensor::readingChanged, this, &SensorReader::processReading);
    }
}

void SensorReader::stopReading()
{
    if (m_sensor->isActive())
    {
        m_sensor->stop();
    }
}

void SensorReader::processReading()
{
    emit readingReady(m_sensor->reading());
}
