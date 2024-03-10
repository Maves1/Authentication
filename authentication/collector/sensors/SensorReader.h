#pragma once

#include <QObject>

class QSensor;
class QSensorReading;

class SensorReader : public QObject
{
    Q_OBJECT
public:
    SensorReader(QSensor *sensor);
    virtual ~SensorReader();

public slots:
    virtual void startReading();
    virtual void stopReading();
    virtual void processReading();

signals:
    void readingReady(const QSensorReading *reading);

protected:
    QSensor *m_sensor;
};
