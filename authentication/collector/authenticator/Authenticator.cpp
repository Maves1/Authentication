#include <QUdpSocket>
#include <QByteArray>
#include <QDataStream>
#include "Authenticator.h"

Authenticator::Authenticator(QString address) : m_socket(new QUdpSocket(this)),
                                                m_address(new QHostAddress(address))
{
}

Authenticator::~Authenticator()
{
    m_socket->close();
}

void Authenticator::sendAuthData(int type, quint64 timestamp, qreal x, qreal y, qreal z)
{
    QByteArray byteArray;
    QDataStream out(&byteArray, QIODevice::WriteOnly);
    out << type << timestamp << x << y << z;

    m_socket->writeDatagram(byteArray, *m_address, m_port);
}

void Authenticator::sendAuthData(int type, qint64 pressTime, qint64 releaseTime)
{
    QByteArray byteArray;
    QDataStream out(&byteArray, QIODevice::WriteOnly);
    out << type << pressTime << releaseTime;

    m_socket->writeDatagram(byteArray, *m_address, m_port);
}
