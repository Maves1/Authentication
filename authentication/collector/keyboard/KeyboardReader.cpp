#include <QLoggingCategory>
#include <QDataStream>
#include <QByteArray>
#include <QUdpSocket>
#include <QHostAddress>

#include "KeyboardReader.h"

#include <cstring>

Q_LOGGING_CATEGORY(collectorKeyboard, "collector.keyboard")

KeyboardReader::KeyboardReader(QObject *parent)
{
    qCDebug(collectorKeyboard) << "ctor";

    m_socket = new QUdpSocket(this);
    m_socket->bind(QHostAddress::LocalHost, m_port);
    connect(m_socket, &QUdpSocket::readyRead, this, &KeyboardReader::receiveReading);
}

KeyboardReader::~KeyboardReader()
{
    m_socket->close();
}

void KeyboardReader::receiveReading()
{
    qCDebug(collectorKeyboard) << "keyboard reading";

    QByteArray datagram;
    datagram.resize(m_socket->pendingDatagramSize());

    QHostAddress address;
    qint64 recSize = m_socket->readDatagram(datagram.data(), datagram.size(), &address);

    QDataStream in(&datagram, QIODevice::ReadOnly);

    qint64 downTime, upTime;
    in >> downTime;
    in >> upTime;

    emit keypressReceived(downTime, upTime);
}
