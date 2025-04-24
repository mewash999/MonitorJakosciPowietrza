#ifndef STATIONHANDLER_H
#define STATIONHANDLER_H

#include <QJsonArray>
#include <QListWidget>
#include <QLabel>
#include <QJsonObject>
#include <QString>
#include <algorithm>
#include <QJsonObject>

class StationHandler
{
public:
    static void handleStationsData(const QJsonArray &array, QListWidget *stationList, QLabel *lblStationCount, QList<QPair<QString, QJsonObject>> &allStations);
    static void updateStationList(const QString &filter, QListWidget *stationList, QLabel *lblStationCount, const QList<QPair<QString, QJsonObject>> &allStations);
};

#endif
