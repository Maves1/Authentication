#include <QSensor>
#include <QLoggingCategory>
#include <QSensorReading>
#include <QGyroscopeReading>
#include <QAccelerometerReading>
#include <QFile>
#include <QTimer>
#include <QDateTime>

#include "SensorManager.h"
#include "GyroscopeReader.h"
#include "AccelerometerReader.h"
#include "keyboard/KeyboardReader.h"

#include <chrono>
#include <fstream>

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

    qint64 calculateBootTimestamp()
    {
        std::chrono::milliseconds uptimeMs(0u);
        double uptimeSeconds;
        if (std::ifstream("/proc/uptime", std::ios::in) >> uptimeSeconds)
        {
            uptimeMs = std::chrono::milliseconds(
                static_cast<unsigned long long>(uptimeSeconds * 1000.0));

            qint64 absTimestamp = QDateTime::currentMSecsSinceEpoch();

            return absTimestamp - uptimeMs.count();
        }

        return 0;
    }
}

SensorManager::SensorManager(QObject *parent) : QObject(parent), m_isReading(false), m_recordingMode(false)
{
    qCDebug(collectorSensorManager) << "ctor";

    // Firstly, let's calculate a boot timestamp
    m_bootTimestampMs = calculateBootTimestamp();

    qCDebug(collectorSensorManager) << "boot timestamp ms:" << m_bootTimestampMs;
    qCDebug(collectorSensorManager) << "curr timestamp ms:" << QDateTime::currentMSecsSinceEpoch();
    qCDebug(collectorSensorManager) << "diff:" << (QDateTime::currentMSecsSinceEpoch() - m_bootTimestampMs);

    // Creating necessary sensor readers
    this->initReaders();

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

void SensorManager::startRecording(int msec)
{
    m_keyToSensorReadings.clear();
    m_keyboardReadings.clear();
    m_recordingMode = true;

    qCDebug(collectorSensorManager) << "recording started";

    // Temporary timer to check that saving works.
    // Will be removed when GUI is added.
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &SensorManager::finishRecording);

    timer->start(msec);
}

void SensorManager::finishRecording()
{
    m_recordingMode = false;

    QString path = "";
    QString sensorsCsvHeader = "timestamp,x,y,z";
    QString keyboardCsvHeader = "press_time,release_time";

    // Saving sensor readings
    for (auto it = m_keyToSensorReadings.begin(); it != m_keyToSensorReadings.end(); ++it)
    {
        QString filename = path + it.key() + ".csv";

        QFile file(filename);

        if (file.open(QIODevice::WriteOnly))
        {
            QTextStream stream(&file);
            stream << sensorsCsvHeader << endl;

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

    // Saving keyboard readings
    QString filename = path + "keyboard.csv";
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
        stream << keyboardCsvHeader << endl;

        for (QString row : m_keyboardReadings)
        {
            stream << row << endl;
        }

        file.close();
    }
    else
    {
        qCCritical(collectorSensorManager) << "could not open file for writing!";
    }

    qCDebug(collectorSensorManager) << "Session has been saved successfully!";
}

void SensorManager::handleSensorReading(const QSensorReading *reading)
{
    const QGyroscopeReading *gReading = qobject_cast<const QGyroscopeReading *>(reading);
    if (gReading)
    {
        QString csvEntry = composeQStringCSVRow(gReading->timestamp() / 1000, gReading->x(), gReading->y(), gReading->z());

        if (isRecording())
        {
            m_keyToSensorReadings["gyroscope"].append(csvEntry);
            // qCDebug(collectorSensorManager) << csvEntry;
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
            // qCDebug(collectorSensorManager) << csvEntry;
        }
    }
}

void SensorManager::handleKeyboardPress(qint64 pressTime, qint64 releaseTime)
{
    if (isRecording())
    {
        pressTime = pressTime - m_bootTimestampMs;
        releaseTime = releaseTime - m_bootTimestampMs;
        qCDebug(collectorSensorManager) << pressTime << releaseTime << (releaseTime - pressTime);
        m_keyboardReadings.append(QString::number(pressTime) + "," + QString::number(releaseTime));
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
    connect(reader, &SensorReader::readingReady, this, &SensorManager::handleSensorReading);
}

void SensorManager::initReaders()
{
    this->addSensorReader(new GyroscopeReader(this));
    this->addSensorReader(new AccelerometerReader(this));

    this->m_keyboardReader = new KeyboardReader(this);
    this->m_keyboardReader->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, this->m_keyboardReader, &QObject::deleteLater);
    connect(this->m_keyboardReader, &KeyboardReader::keypressReceived, this, &SensorManager::handleKeyboardPress);
}
