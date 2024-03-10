#include <QSensor>
#include <QLoggingCategory>
#include <QSensorReading>
#include <QGyroscopeReading>
#include <QAccelerometerReading>
#include <QFile>
#include <QTimer>

#include "SensorManager.h"
#include "GyroscopeReader.h"
#include "AccelerometerReader.h"

Q_LOGGING_CATEGORY(collectorSensorManager, "collector.sensormanager")

namespace
{
    QString composeQStringCSVRow(quint64 timestamp, double x, double y, double z)
    {
        std::string stdRowReading = std::to_string(timestamp) + "," +
                                    std::to_string(x) + "," +
                                    std::to_string(y) + "," +
                                    std::to_string(z);
        return QString::fromStdString(stdRowReading);
    }
}

SensorManager::SensorManager(QObject *parent) : QObject(parent), m_isReading(false), m_recordingMode(false)
{
    qCDebug(collectorSensorManager) << "ctor";

    // Creating necessary sensor readers
    this->addSensorReader(new GyroscopeReader(this));
    this->addSensorReader(new AccelerometerReader(this));

    qCDebug(collectorSensorManager) << "sensorreaders created";

    // Starting the thread
    workerThread.start();

    qCDebug(collectorSensorManager) << "workerThread started";
}

SensorManager::~SensorManager()
{
    qCDebug(collectorSensorManager) << "dtor";

    workerThread.quit();
    workerThread.wait();
}

bool SensorManager::isRecording()
{
    return m_recordingMode;
}

void SensorManager::startRecording()
{
    m_keyToSensorReadings.clear();
    m_recordingMode = true;

    qCDebug(collectorSensorManager) << "recording started";

    // Temporary timer to check that saving works.
    // Will be removed when GUI is added.
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &SensorManager::finishRecording);

    timer->start(10000);
}

void SensorManager::finishRecording()
{
    m_recordingMode = false;

    QString path = "";
    QString csvHeader = "timestamp,x,y,z";

    for (auto it = m_keyToSensorReadings.begin(); it != m_keyToSensorReadings.end(); ++it)
    {
        QString filename = path + it.key() + ".csv";

        QFile file(filename);

        if (file.open(QIODevice::WriteOnly))
        {
            QTextStream stream(&file);
            stream << csvHeader << endl;

            for (QString row : it.value())
            {
                stream << row << endl;
            }

            file.close();
        }
        else
        {
            qCCritical(collectorSensorManager) << "could not open file for writing!";
        }
    }

    qCDebug(collectorSensorManager) << "Session has been saved successfully!";
}

void SensorManager::handleResults(const QSensorReading *reading)
{
    const QGyroscopeReading *gReading = qobject_cast<const QGyroscopeReading *>(reading);
    if (gReading)
    {
        QString csvEntry = composeQStringCSVRow(gReading->timestamp() / 1000, gReading->x(), gReading->y(), gReading->z());

        if (isRecording())
        {
            m_keyToSensorReadings["gyroscope"].append(csvEntry);
            qCDebug(collectorSensorManager) << csvEntry;
        }

        return;
    }

    const QAccelerometerReading *aReading = qobject_cast<const QAccelerometerReading *>(reading);
    if (aReading)
    {
        QString csvEntry = composeQStringCSVRow(aReading->timestamp() / 1000, aReading->x(), aReading->y(), aReading->z());

        if (isRecording())
        {
            m_keyToSensorReadings["accelerometer"].append(csvEntry);
            qCDebug(collectorSensorManager) << csvEntry;
        }
    }
}

void SensorManager::addSensorReader(SensorReader *reader)
{
    this->m_sensorReaders.append(reader);

    reader->moveToThread(&workerThread);

    // Connecting signals and slots
    connect(&workerThread, &QThread::finished, reader, &QObject::deleteLater);
    // All SensorReaders start reading when SensorManager emits startReading()
    connect(this, &SensorManager::startReading, reader, &SensorReader::startReading);
    // SensorManager processes all readings from SensorReaders
    connect(reader, &SensorReader::readingReady, this, &SensorManager::handleResults);
}
