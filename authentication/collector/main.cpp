#include <iostream>

#include <QtCore>
#include <QString>
#include <QLoggingCategory>

#include "sensors/SensorManager.h"

Q_LOGGING_CATEGORY(collector, "collector")

int main(int argc, char **argv)
{
    QCoreApplication application(argc, argv);

    QCoreApplication::setOrganizationName("ru.mavesoft");
    QCoreApplication::setApplicationName("sensor-collector");

    qCDebug(collector) << "QCoreApplication params set";

    QString authenticatorAddress;
    if (argc == 2)
    {
        authenticatorAddress = QString(argv[1]);
    }
    else
    {
        authenticatorAddress = QString("127.0.0.1");
    }

    SensorManager manager(nullptr, authenticatorAddress);
    qCDebug(collector) << "SensorManager created";

    int req_frequency = 100; // Hz
    emit manager.startReading();
    qCDebug(collector) << "startReading() emitted";

    int recordingTime = 7 * 60 * 1000; // 7 mins in milliseconds
    // manager.startRecording(recordingTime);
    manager.startPredicting();

    return application.exec();
}
