#include <iostream>

#include <QtCore>
#include <QLoggingCategory>

#include "sensors/SensorManager.h"

Q_LOGGING_CATEGORY(collector, "collector")

int main(int argc, char **argv)
{
    QCoreApplication application(argc, argv);

    QCoreApplication::setOrganizationName("ru.mavesoft");
    QCoreApplication::setApplicationName("sensor-collector");

    qCDebug(collector) << "QCoreApplication params set";

    SensorManager manager;
    qCDebug(collector) << "SensorManager created";

    int req_frequency = 100; // Hz
    emit manager.startReading();

    qCDebug(collector) << "startReading() emitted";

    return application.exec();
}
