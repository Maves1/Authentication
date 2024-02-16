#include <QSensor>
#include <QLoggingCategory>
#include <QSensorReading>
#include <QGyroscopeReading>

#include "SensorManager.h"
#include "GyroscopeReader.h"

Q_LOGGING_CATEGORY(collectorSensorManager, "collector.sensormanager")

SensorManager::SensorManager(QObject *parent) : QObject(parent), m_isReading(false)
{
    qCDebug(collectorSensorManager) << "ctor";

    // Creating necessary sensor readers
    GyroscopeReader *gyroscopeReader = new GyroscopeReader(this);

    qCDebug(collectorSensorManager) << "gyroscopereader created";

    this->m_sensorReaders.append(gyroscopeReader);

    qCDebug(collectorSensorManager) << "sensorreaders created";

    for (int i = 0; i < this->m_sensorReaders.size(); i++)
    {
        this->m_sensorReaders[i]->moveToThread(&workerThread);

        // Connecting signals and slots
        connect(&workerThread, &QThread::finished, this->m_sensorReaders[i], &QObject::deleteLater);
        // All SensorReaders start reading when SensorManager emits startReading()
        connect(this, &SensorManager::startReading, this->m_sensorReaders[i], &SensorReader::startReading);
        // SensorManager processes all readings from SensorReaders
        connect(this->m_sensorReaders[i], &SensorReader::readingReady, this, &SensorManager::handleResults);
    }

    qCDebug(collectorSensorManager) << "starting workerThread";

    // Starting the thread
    workerThread.start();
}

SensorManager::~SensorManager()
{
    workerThread.quit();
    workerThread.wait();
}

void SensorManager::handleResults(const QSensorReading *reading)
{
    const QGyroscopeReading *gReading = qobject_cast<const QGyroscopeReading *>(reading);

    qCDebug(collectorSensorManager) << gReading->timestamp() << " x: " << gReading->x() << " y: " << gReading->y() << " z: " << gReading->z();
}
