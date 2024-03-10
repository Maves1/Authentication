#pragma once

#include <QObject>
#include <QList>
#include <QMap>
#include <QThread>
#include <QString>

#include "SensorReader.h"

class QSensorReading;

class SensorManager : public QObject
{
    Q_OBJECT
    QThread workerThread;

public:
    SensorManager(QObject *parent = nullptr);
    ~SensorManager();

    void startRecording();
    void finishRecording();
    bool isRecording();

public slots:
    void handleResults(const QSensorReading *);
signals:
    void startReading();

private:
    bool m_isReading;
    bool m_recordingMode;
    int m_readingHz;

    QList<SensorReader *> m_sensorReaders;
    QMap<QString, QList<QString>> m_keyToSensorReadings; // todo: replace QString with another type
                                                         // I guess it's a bad idea to keep hardcoded values as keys

    void addSensorReader(SensorReader *reader);
};
