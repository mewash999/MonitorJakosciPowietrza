#ifndef SENSORHANDLER_H
#define SENSORHANDLER_H

#include <QJsonArray>
#include <QListWidget>
#include <QJsonObject>
#include <algorithm>

class SensorHandler
{
public:
    static void handleSensorsData(const QJsonArray &array, QListWidget *sensorList, QVector<QPair<int, QString>> &currentSensors);
};

#endif
