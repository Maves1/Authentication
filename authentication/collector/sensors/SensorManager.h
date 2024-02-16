#pragma once

#include <QObject>
#include <QList>
#include <QThread>

#include "SensorReader.h"

class QSensorReading;

class SensorManager : public QObject
{
    Q_OBJECT
    QThread workerThread;

public:
    SensorManager(QObject *parent = nullptr);
    ~SensorManager();

public slots:
    void handleResults(const QSensorReading *);
signals:
    void startReading();

private:
    bool m_isReading;
    int m_readingHz;

    QList<SensorReader *> m_sensorReaders;
};
