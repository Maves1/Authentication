#pragma once

#include <QString>

#include "IAuthenticator.h"

class QUdpSocket;
class QHostAddress;

class Authenticator : public IAuthenticator
{
    Q_OBJECT
public:
    Authenticator(QString address);
    virtual ~Authenticator();

    void sendAuthData(int type, quint64 timestamp, qreal x, qreal y, qreal z) override;
    void sendAuthData(int type, qint64 pressTime, qint64 releaseTime) override;

private:
    QUdpSocket *m_socket;
    QHostAddress *m_address;
    int m_port = 2025;
};
