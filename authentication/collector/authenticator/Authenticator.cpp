#include <QUdpSocket>
#include <QByteArray>
#include <QDataStream>
#include <QLoggingCategory>
#include "Authenticator.h"

Q_LOGGING_CATEGORY(collectorAuthenticator, "collector.authenticator")

Authenticator::Authenticator(QString address) : m_socket(new QUdpSocket(this)),
                                                m_address(new QHostAddress(address))
{
    qCDebug(collectorAuthenticator) << "ctor";
}

Authenticator::~Authenticator()
{
    m_socket->close();
}

void Authenticator::sendAuthData(int type, quint64 timestamp, qreal x, qreal y, qreal z)
{
    // qCDebug(collectorAuthenticator) << "send sensor auth data";

    QByteArray byteArray;
    QDataStream out(&byteArray, QIODevice::WriteOnly);
    out << type << timestamp << x << y << z;

    m_socket->writeDatagram(byteArray, *m_address, m_port);
}

void Authenticator::sendAuthData(int type, qint64 pressTime, qint64 releaseTime)
{
    // qCDebug(collectorAuthenticator) << "send keyboard auth data";

    QByteArray byteArray;
    QDataStream out(&byteArray, QIODevice::WriteOnly);
    out << type << pressTime << releaseTime;

    m_socket->writeDatagram(byteArray, *m_address, m_port);
}
