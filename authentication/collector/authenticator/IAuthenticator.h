#pragma once

#include <QObject>

class IAuthenticator : public QObject
{
    Q_OBJECT
public:
    enum DataType
    {
        Accelerometer = 0,
        Gyroscope = 1,
        Keyboard = 2
    };

    virtual void sendAuthData(int type, quint64 timestamp, qreal x, qreal y, qreal z) = 0;
    virtual void sendAuthData(int type, qint64 pressTime, qint64 releaseTime) = 0;
};
