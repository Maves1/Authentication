#pragma once

#include "SensorReader.h"

class AccelerometerReader : public SensorReader
{
    Q_OBJECT
public:
    AccelerometerReader(QObject *parent = nullptr);
};
