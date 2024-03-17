#pragma once

#include <QObject>

class QUdpSocket;

class KeyboardReader : public QObject
{
    Q_OBJECT
public:
    KeyboardReader(QObject *parent = nullptr);
    ~KeyboardReader();

public slots:
    void receiveReading();

signals:
    void keypressReceived(qint64 pressTime, qint64 releaseTime);

private:
    QUdpSocket *m_socket;
    int m_port = 2024;
};
