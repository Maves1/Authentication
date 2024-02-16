#pragma once

#include "SensorReader.h"

class GyroscopeReader : public SensorReader {
    Q_OBJECT
public:
    GyroscopeReader(QObject *parent = nullptr);
};
