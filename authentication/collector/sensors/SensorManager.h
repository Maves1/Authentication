#pragma once

#include <QObject>
#include <QList>
#include <QMap>
#include <QThread>
#include <QString>

#include "SensorReader.h"

class QSensorReading;
class KeyboardReader;

class SensorManager : public QObject
{
    Q_OBJECT
    QThread workerThread;

public:
    SensorManager(QObject *parent = nullptr);
    ~SensorManager();

    void startRecording(int);
    void finishRecording();
    bool isRecording();

public slots:
    void handleSensorReading(const QSensorReading *);
    void handleKeyboardPress(qint64, qint64);
signals:
    void startReading();

private:
    bool m_isReading;
    bool m_recordingMode;
    int m_readingHz;
    qint64 m_bootTimestampMs;

    QList<SensorReader *> m_sensorReaders;
    KeyboardReader *m_keyboardReader;

    QMap<QString, QList<QString>> m_keyToSensorReadings; // todo: replace QString with another type
                                                         // I guess it's a bad idea to keep hardcoded values as keys
    QList<QString> m_keyboardReadings;

    void addSensorReader(SensorReader *reader);
    void initReaders();
};
